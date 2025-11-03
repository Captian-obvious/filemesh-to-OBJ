#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_map>
/*NOTE:
All FileMesh versions start with "version (version)\n", INCLUDING binary ones
*/

typedef unsigned char byte;
typedef signed char sbyte;

const char* supportedVersions[] = {"1.00", "1.01", "2.00"};
typedef struct {
    unsigned short sizeof_mesh2Head;
    byte sizeof_mesh2Vertex;
    byte sizeof_mesh2Face;

    uint vert_cnt;
    uint face_cnt;
} mesh2Head;

typedef struct {
    float px,py,pz;
    float nx,ny,nz;
    float tu,tv;
    sbyte tx,ty,tz,ts;
    byte r,g,b,a; // THIS IS NOT ALWAYS INCLUDED!
} mesh2Vertex;

typedef struct {
    float px,py,pz;
    float nx,ny,nz;
    float tu,tv;
    sbyte tx,ty,tz,ts;
} mesh2VertexNoColor;

typedef struct {
    uint a;
    uint b;
    uint c;
} mesh2Face;

typedef struct {
    mesh2Head header;
    mesh2Vertex* verts;
    mesh2Face* faces;
} mesh2;

void print_info(std::string msg){
    std::cout << msg << std:endl;
};

int main(int argc,char** argv){
    if (argc<2){
        std::cout << "Usage: " << argv[0] << "<filemesh path> <opt: output OBJ path>";
    };
};
