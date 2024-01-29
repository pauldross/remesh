//
// Created by paul on 9/20/23.
// input: the field file and the .geo file of the structure
// outputs: the remeshed structure

#ifndef GLYPH3D_REMESH_H
#define GLYPH3D_REMESH_H

#include "VTKParser/depend/VTKParser.h"
#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include "Point.h"
#include <functional>
#include <regex>
#include <utility>
#include <filesystem>
#include "VTKParser/depend/VTKParser.h"


//#define old

typedef struct RET_JD {
    int PID;
    double deltaj;
    double percChange;

    RET_JD(int PID, double deltaj, double percChange) : PID(PID), deltaj(deltaj), percChange(percChange) {};
} RET_JD;

bool g_debug;

namespace fs = std::filesystem;

#define B_PT(v) {v[0], v[1], v[3]}
#define PARSE_PT_TO_POINT(V) {V->at(0), V->at(1), V->at(2)}

namespace remesh {

    std::string meshFileName;

    RET_JD getJdelta(int Pid, VTKparser &parser) {
        // get cells connected to point
        auto jc_p = parser.getBFromPID(Pid);
//        Point Jc = B_PT(D_array->GetTuple3(Pid));
        Point Jc = PARSE_PT_TO_POINT(jc_p);


        int ncells;
        auto cells = parser.getCellIDsFromPointIDs(Pid,ncells);
//        output->GetPointCells(Pid, ncells, cells);
        double max = -10000;
        int pid = Pid;
        //loop through cells
//        cout << "--------------" << endl;
//        cout << "Pid" << ": "<< Pid << " Jc: " << Jc << endl;
        for (int i = 0; i < ncells; i++) {
            // loop through points in cell
            int npts;
//            vtkIdType const *pts;
//            output->GetCellPoints(cells->at(i), npts, pts);
            auto pts = parser.getPointIDsFromCellId(cells->at(i), npts);

            for (int k = 0; k < npts; k++) {
                // if pid isn't pid calc change in J between id and pid
                if (pts->at(k) != Pid) {
                    // compare with current max
//                    Point J = B_PT(D_array->GetTuple3(pts[k]));
                    Point J = PARSE_PT_TO_POINT(parser.getBFromPID(pts->at(k)));
                    double n1 = Jc.norm();
                    double n2 = J.norm();
                    double diff_norm = n1 - n2;
//                    cout << "pid: "<< pts[k] << " " <<"J: " << J << " Diff: "<< n1 << " - " << n2 << " = " << diff_norm << endl;


                    if (diff_norm > max) {
                        max = diff_norm;
                        pid = pts->at(k);
                    }
                }
            }

        }
        double percChange = max / Jc.norm();
        return {pid, max, percChange};
    }

    double getDist(int pid1, int pid2, VTKparser &parser) {
//        auto pt = PointArray->GetPoint(pid1);
        auto pt = parser.getVec3FromPID(pid1);
        Point ptloc1 = Point(pt->at(0), pt->at(1), pt->at(2));
//        pt = PointArray->GetPoint(pid2);
        pt = parser.getVec3FromPID(pid2);
        Point ptloc2 = Point(pt->at(0), pt->at(1), pt->at(2));
        Point diff = ptloc1 - ptloc2;

        if (pid1 == pid2) { return 1e-6; }
        return diff.norm();
    }


    void generateBgMesh(const std::string &fileName,
                        const std::string &geoFileName = "washerFIELD.geo",
                        std::string mFileName = "washer",
                        double mindj = 0.1,
                        double minChar = 1e-8,
                        int debug = 0
    ) {
        g_debug = debug;
        using namespace std;
        meshFileName = std::move(mFileName);
        //read in file
//        vtkNew<vtkGenericDataObjectReader> reader;
//        reader->SetFileName(fileName.c_str());
//        reader->Update();
        VTKparser parser(fileName);
        parser.parse();

        ofstream POS(meshFileName + "RM.geo");
        std::cout << "Reading file" << std::endl;

//        vtkPoints *PointArray = output->GetPoints()
        // get data array
//        vtkDoubleArray *D_array = vtkDoubleArray::SafeDownCast(
//                output->GetPointData()->GetArray("10.000GHz_fone_imag"));


        POS << "View \"background mesh\" {" << endl;

        // loop through each cell
//        auto num = output->GetNumberOfCells();
        int num = parser.getNumCells();

        for (int i = 0; i < num; i++) {

            vector<pair<Point, double>> pcl;
            // loop through each point in cell
            int npts;
            std::shared_ptr<std::vector<int>> pts = parser.getPointIDsFromCellId(i,npts);
//            vtkIdType const *pts;
            // Then use cell ids to get points
//            output->GetCellPoints(i, npts, pts);
            vector<int> ptsvec;
            for (int k = 0; k < npts; k++) {
                ptsvec.push_back(pts->at(k));
            }
            if (npts == 4) { POS << "SS("; } else { POS << "ST("; }

            // loop through each point
            for (int k = 0; k < ptsvec.size(); k++) {
                RET_JD ret = getJdelta(ptsvec[k], parser);
                double n = 1;
                // if mindj is a percentage is an actual value
                /*if (ret.deltaj > mindj) {
                    n = ret.deltaj/mindj;
                    n = (n == 1) ?  2 : n;
                }*/
                if (ret.percChange > mindj) {
                    n = ret.percChange / mindj;
                }
                n = (n == 0) ? 1 : n;
//                double *pt = PointArray->GetPoint(ptsvec[k]);
                auto pt = parser.getVec3FromPID(ptsvec[k]);
                Point ptloc = Point(pt->at(0), pt->at(1), pt->at(2));
                double charlen = (getDist(ptsvec[k], ret.PID, parser)) / (n);
                if (charlen < minChar) { charlen = minChar; }
//                    cout << charlen << endl;
                pair<Point, double> v(ptloc, charlen);
//                    cout << charlen << endl;
//                if (debug) {
//                    cout << charlen << endl;
//                }
                pcl.push_back(v);
            }
//                cout << pcl.size() << endl;
            if (pcl.size() == 4) {
                POS << pcl[0].first.x << "," << pcl[0].first.y << "," << pcl[0].first.z << ",";
                POS << pcl[1].first.x << "," << pcl[1].first.y << "," << pcl[1].first.z << ",";
                POS << pcl[2].first.x << "," << pcl[2].first.y << "," << pcl[2].first.z << ",";
                POS << pcl[3].first.x << "," << pcl[3].first.y << "," << pcl[3].first.z << "){";
                POS << pcl[0].second << "," << pcl[1].second << "," << pcl[2].second << "," << pcl[3].second << "};"
                    << endl;
            } else {
                POS << pcl[0].first.x << "," << pcl[0].first.y << "," << pcl[0].first.z << ",";
                POS << pcl[1].first.x << "," << pcl[1].first.y << "," << pcl[1].first.z << ",";
                POS << pcl[2].first.x << "," << pcl[2].first.y << "," << pcl[2].first.z << "){";
                POS << pcl[0].second << "," << pcl[1].second << "," << pcl[2].second << "};"
                    << endl;
            }

        }


        POS << "};" << endl << endl;
        POS << "Background Mesh View[0];" << endl;
        POS << "Mesh.MeshSizeExtendFromBoundary = 0;" << endl << "Mesh.MeshSizeFromPoints = 0;" << endl
            << "Mesh.MeshSizeFromCurvature = 0;" << endl;
        //open the geometry file
        ifstream GEO(geoFileName);
        regex pattern("(Save +\").+(\";)");
        string line;
        while (std::getline(GEO, line)) {
            if (line.find("Save") != string::npos) {
                string replaced = regex_replace(line, pattern, "$1" + meshFileName + ".msh" + "$2");
                line = replaced;
            }
            POS << line << endl;
        }
        GEO.close();
        POS.close();
        cout << "Finished writing file with background mesh" << endl;


    }

    void cleanUp() {
//        remove((meshFileName+".msh").c_str());
        remove((meshFileName + ".geo").c_str());
        try {
            fs::path currentDir = fs::current_path();

            for (const auto &entry: fs::directory_iterator(currentDir)) {
                if (entry.path().extension() == ".geo_unrolled") {
                    fs::remove(entry.path());
                }
            }
        } catch (const fs::filesystem_error &e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

    void runGMSH() {
        using namespace std;
        cout << "starting gmsh: " << "gmsh " + meshFileName + "RM.geo -0 -v 0" << endl;
//        string run = "gmsh "+meshFileName+".geo -0 -v 1";
        string run;
        if (!g_debug) {
            run = "gmsh " + meshFileName + "RM.geo -0";
        } else {
            run = "gmsh " + meshFileName + "RM.geo";
        }
        system(run.c_str());
        cout << "gmsh finished" << endl;
    }


}


#endif //GLYPH3D_REMESH_H
