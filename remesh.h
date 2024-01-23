//
// Created by paul on 9/20/23.
// input: the field file and the .geo file of the structure
// outputs: the remeshed structure

#ifndef GLYPH3D_REMESH_H
#define GLYPH3D_REMESH_H

#include <vtkNew.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkDoubleArray.h>
#include <vtkGenericDataObjectReader.h>
#include <vtkUnstructuredGrid.h>
#include <vtkInformation.h>
#include <cmath>
#include <string>
#include <iostream>
#include <fstream>
#include "Point.h"
#include <functional>
#include <regex>
#include <utility>
#include <filesystem>
#include <vtkIndent.h>
#include <vtkInformationIterator.h>
#include <vtkInformationKey.h>

//#define old

typedef struct RET_JD {
    vtkIdType PID;
    double deltaj;
    double percChange;

    RET_JD(vtkIdType PID, double deltaj, double percChange) : PID(PID), deltaj(deltaj), percChange(percChange) {};
} RET_JD;

namespace fs = std::filesystem;

#define B_PT(v) {v[0], v[1], v[3]}


namespace remesh {

    std::string meshFileName;

    RET_JD getJdelta(vtkIdType Pid, vtkPoints *PointArray, vtkDoubleArray *D_array, vtkUnstructuredGrid *output) {
        // get cells connected to point

        Point Jc = B_PT(D_array->GetTuple3(Pid));
        vtkIdType ncells;
        vtkIdType *cells;
        output->GetPointCells(Pid, ncells, cells);
        double max = -10000;
        vtkIdType pid = Pid;
        //loop through cells
//        cout << "--------------" << endl;
//        cout << "Pid" << ": "<< Pid << " Jc: " << Jc << endl;
        for (vtkIdType i = 0; i < ncells; i++) {
            // loop through points in cell
            vtkIdType npts;
            vtkIdType const *pts;
            output->GetCellPoints(cells[i], npts, pts);
            for (vtkIdType k = 0; k < npts; k++) {
                // if pid isn't pid calc change in J between id and pid
                if (pts[k] != Pid) {
                    // compare with current max
                    Point J = B_PT(D_array->GetTuple3(pts[k]));
                    double n1 = Jc.norm();
                    double n2 = J.norm();
                    double diff_norm = n1 - n2;
//                    cout << "pid: "<< pts[k] << " " <<"J: " << J << " Diff: "<< n1 << " - " << n2 << " = " << diff_norm << endl;


                    if (diff_norm > max) {
                        max = diff_norm;
                        pid = pts[k];
                    }
                }
            }

        }
        double percChange = max / Jc.norm();
        return RET_JD(pid, max, percChange);
    }

    double getDist(vtkIdType pid1, vtkIdType pid2, vtkPoints *PointArray) {
        double *pt = PointArray->GetPoint(pid1);
        Point ptloc1 = Point(pt[0], pt[1], pt[2]);
        pt = PointArray->GetPoint(pid2);
        Point ptloc2 = Point(pt[0], pt[1], pt[2]);
        Point diff = ptloc1 - ptloc2;

        if (pid1 == pid2) { return 1e-6; }
        return diff.norm();
    }

#ifdef old
    void generateBgMesh(const std::string& fileName,
                        const std::string& geoFileName = "washerFIELD.geo",
                        double mindj = 2e10,
                        const long  double min_len = 0.9E-7f,
                        const long double max_len = 8E-6f,
                        const double frac_min_norm = 0.01,
                        const double frac_max_norm = 0.2,
                        std::string mFileName = "washer"){
#else

    void generateBgMesh(const std::string &fileName,
                        const std::string &geoFileName = "washerFIELD.geo",
                        std::string mFileName = "washer",
                        double mindj = 0.1,
                        double minChar = 1e-8,
                        int debug = 0
    ) {
#endif
        using namespace std;
        meshFileName = std::move(mFileName);
        //read in file
        vtkNew<vtkGenericDataObjectReader> reader;
        reader->SetFileName(fileName.c_str());
        reader->Update();

        if (reader->IsFileUnstructuredGrid()) {
            ofstream POS(meshFileName + "RM.geo");
            std::cout << "Reading file" << std::endl;
            auto output = reader->GetUnstructuredGridOutput();
            output->BuildLinks();
            vtkPoints *PointArray = output->GetPoints();
            // get data array
            vtkDoubleArray *D_array = vtkDoubleArray::SafeDownCast(
                    output->GetPointData()->GetArray("10.000GHz_fone_imag"));


            POS << "View \"background mesh\" {" << endl;
#ifdef  old
            const double max_norm =  D_array->GetMaxNorm() * frac_max_norm;
            const double min_norm = D_array->GetMaxNorm() * frac_min_norm;
#endif
            // loop through each cell
            auto num = output->GetNumberOfCells();
            for (vtkIdType i = 0; i < num; i++) {
#ifdef old
                vtkIdType npts;
                vtkIdType const * pts;
                // Then use cell ids to get points
                output->GetCellPoints(i,npts,pts);
                POS << "ST(";
                for (vtkIdType k = 0; k < npts-1; k++){
                    double * pt = PointArray->GetPoint(pts[k]);
                    Point p = (Point) {pt[0], pt[1], pt[2]};
//                    p.z = std::round(p.z * 1e-50) / 1e-50;

                    POS << p.x << "," << p.y << "," << p.z << ',';
                }
                double * pt = PointArray->GetPoint(pts[npts-1]);
                Point p = (Point) {pt[0], pt[1], pt[2]};
                POS << p.x << "," << p.y << "," << p.z;

                double p1 = ((Point) B_PT(D_array->GetTuple3(pts[0]))).norm();
                double p2 = ((Point) B_PT(D_array->GetTuple3(pts[1]))).norm();
                double p3 = ((Point) B_PT(D_array->GetTuple3(pts[2]))).norm();

                function<long double(double)> map = [&max_norm, &min_norm, &max_len, &min_len](double x) ->long  double {
                    if (x < min_norm) {
                        return max_len;
                    } else if (x > max_norm) {
                        return min_len;
                    } else {
                        return ((max_len - min_len)/(min_norm - max_norm))*(x - max_norm) + min_len;
                    }
                };

//            cout << 1/(p1*1E8) << endl;
                POS << "){" << map(p1) << "," << map(p2) << "," << map(p3) << "};" << endl;
#endif
                vector<pair<Point, double>> pcl;
                // loop through each point in cell
                vtkIdType npts;
                vtkIdType const *pts;
                // Then use cell ids to get points
                output->GetCellPoints(i, npts, pts);
                vector<vtkIdType> ptsvec;
                for (vtkIdType k = 0; k < npts; k++) {
                    ptsvec.push_back(pts[k]);
                }
                if (npts == 4) { POS << "SS("; } else { POS << "ST("; }

                // loop through each point
                for (vtkIdType k = 0; k < ptsvec.size(); k++) {
                    RET_JD ret = getJdelta(ptsvec[k], PointArray, D_array, output);
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
                    double *pt = PointArray->GetPoint(ptsvec[k]);
                    Point ptloc = Point(pt[0], pt[1], pt[2]);
                    double charlen = (getDist(ptsvec[k], ret.PID, PointArray)) / (n);
                    if (charlen < minChar) { charlen = minChar; }
//                    cout << charlen << endl;
                    pair<Point, double> v(ptloc, charlen);
//                    cout << charlen << endl;
                    if (debug) {
                        cout << charlen << endl;
                    }
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
        } else {
            cout << "Only unstructured grids are supported" << endl;
        }

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
        string run = "gmsh " + meshFileName + "RM.geo -0";
        system(run.c_str());
        cout << "gmsh finished" << endl;
    }


}


#endif //GLYPH3D_REMESH_H
