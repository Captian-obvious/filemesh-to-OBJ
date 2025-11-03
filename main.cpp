#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <unordered_map>

typedef unsigned char byte;
typedef signed char sbyte;

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
    mesh2Head header;
    mesh2Vertex
} mesh2;

int main(int argc,char** argv){
    if (argc>1){
        // adding soon
    };
};
