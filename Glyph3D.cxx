/**
 * remeshing
 */

#include "remesh.h"

using namespace std;



int main(int argc, char *argv[]) {
    std::string inputFilename = argv[1];
    std::string geoName;

#ifdef old
    std::string min_len_s;
    std::string max_len_s;
    if (argc == 3){
        geoName = argv[2];
        remesh::generateBgMesh(inputFilename,geoName);
    }else if (argc == 7){
        geoName =  argv[2];
        min_len_s = argv[3];
        max_len_s = argv[4];
        const long double minlen = stod(min_len_s);
        const long double maxlen = stod(max_len_s);

        min_len_s = argv[5];
        max_len_s = argv[6];
        const double fmin = stod(min_len_s);
        const double fmax = stod(max_len_s);

        remesh::generateBgMesh(inputFilename,geoName, minlen,maxlen,fmin,fmax);
    }else if (argc > 7) {
        geoName =  argv[2];
        min_len_s = argv[3];
        max_len_s = argv[4];
        const long double minlen = stod(min_len_s);
        const long double maxlen = stod(max_len_s);

        min_len_s = argv[5];
        max_len_s = argv[6];
        const double fmin = stod(min_len_s);
        const double fmax = stod(max_len_s);
        string mFIleName = argv[7];
        remesh::generateBgMesh(inputFilename,geoName, minlen,maxlen,fmin,fmax,mFIleName);
    } else {
        remesh::generateBgMesh(inputFilename);
    }
#else
    if (argc == 3) {
        geoName = argv[2];
        remesh::generateBgMesh(inputFilename, geoName);
    }
    if (argc == 4) {
        geoName = argv[2];
        std::string mshname = argv[3];
//        double dj = stod(min_dj);
        remesh::generateBgMesh(inputFilename, geoName,mshname);
    }
    if (argc == 5) {
        geoName = argv[2];
        std::string mshname = argv[3] ;
        std::string min_dj = argv[4];
        double dj = stod(min_dj);

        remesh::generateBgMesh(inputFilename, geoName,mshname,dj);
    }
    if (argc == 6) {
        geoName = argv[2];
        std::string mshname = argv[3];
        std::string min_dj = argv[4];
        string min_char = argv[5];
        double dj = stod(min_dj);
        double minChar = stod(min_char);
        remesh::generateBgMesh(inputFilename, geoName,mshname,dj,minChar);
    }
    if (argc == 7) {
        geoName = argv[2];
        std::string mshname = argv[3];
        std::string min_dj = argv[4];
        string min_char = argv[5];
        double dj = stod(min_dj);
        double minChar = stod(min_char);
        remesh::generateBgMesh(inputFilename, geoName,mshname,dj,minChar, 1);
    }
#endif
    remesh::runGMSH();
//    remesh::cleanUp();
    return EXIT_SUCCESS;
}