/**
 * remeshing
 */

#include "remesh.h"
#include "argparse.hpp"

using namespace std;

///home/paul/Skripsie/Code/SkripsieCode/cpp/TryRemeshing/cmake-build-debug/remesh {outFile} {name[:-4]}.geo {name[:-8]} {perc}

/*
 * arguments
 *
 * 1) path to vtk file
 * 2) path to geo file
 * 3) what to name the resulting remeshed file (should be different from the second argument)
 * 4) min characteristic length (prevents over refinement)
 * 5) 3 debug (t/f) not needed t : shows gmsh gui; f : hides gmsh gui
*/
int main(int argc, char *argv[]) {

    argparse::ArgumentParser program("remesh");

    program.add_argument("vtkpath")
        .help("The path to the VTK file containg the J field data for the squid washer")
        .required();

    program.add_argument("geopath")
        .help("The path to the geometry file that is to be optimised")
        .required();

    program.add_argument("-o")
        .help("The name of the optimised geo and msh file should be named (omit the .msh or .geo file extension)")
        .default_value("opt");

    program.add_argument("-mindj")
        .help("The minimum percentage change allowed between points in the mesh")
        .default_value(0.1)
        .scan<'g',double>();

    program.add_argument("-minchar")
        .help("The minimum characteristic length (prevents over refinement set carefully!)")
        .default_value(1e-8)
        .scan<'g',double>();

    program.add_argument("-debug")
        .help("When debug is added remesh will show the mesh in the GMSH window")
        .implicit_value(true)
        .default_value(false);

    program.add_argument("-rg")
            .help("Adding this tag stops the program from running GMSH")
            .implicit_value(true)
            .default_value(false);

    program.add_description(
                            "\"remesh\" is a module that allows the user to optimise the mesh "
                            "based on the parameters described below"
                            );

    try {
        program.parse_args(argc, argv); // Example: ./main --query_point 3.5 4.7 9.2
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        std::cerr << program;
        std::exit(1);
    }


    remesh::generateBgMesh(
            program.get<std::string>("vtkpath"),
            program.get<std::string>("geopath"),
            program.get<std::string>("-o"),
            program.get<double>("-mindj"),
            program.get<double>("-minchar"),
            program.get<bool>("-debug")
            );

//    std::string inputFilename = argv[1];
//    std::string geoName;
//
//
//    if (argc == 3) {
//        geoName = argv[2];
//        remesh::generateBgMesh(inputFilename, geoName);
//    }
//    if (argc == 4) {
//        geoName = argv[2];
//        std::string mshname = argv[3];
////        double dj = stod(min_dj);
//        remesh::generateBgMesh(inputFilename, geoName,mshname);
//    }
//    if (argc == 5) {
//        geoName = argv[2];
//        std::string mshname = argv[3] ;
//        std::string min_dj = argv[4];
//        double dj = stod(min_dj);
//
//        remesh::generateBgMesh(inputFilename, geoName,mshname,dj);
//    }
//    if (argc == 6) {
//        geoName = argv[2];
//        std::string mshname = argv[3];
//        std::string min_dj = argv[4];
//        string min_char = argv[5];
//        double dj = stod(min_dj);
//        double minChar = stod(min_char);
//        remesh::generateBgMesh(inputFilename, geoName,mshname,dj,minChar);
//    }
//    if (argc == 7) {
//        geoName = argv[2];
//        std::string mshname = argv[3];
//        std::string min_dj = argv[4];
//        string min_char = argv[5];
//        double dj = stod(min_dj);
//        double minChar = stod(min_char);
//        remesh::generateBgMesh(inputFilename, geoName,mshname,dj,minChar, 1);
//    }
    if (!program.get<bool>("-rg")) {remesh::runGMSH();}
//    remesh::cleanUp();
    return EXIT_SUCCESS;
}