#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <format>
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
    std::cout << msg << std::endl;
};
void print_err(std::string msg){
    std::cout << msg << std::endl;
};

std::vector<std::string> split(const std::string &s,const char separator=' '){
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(s);
    while (std::getline(tokenStream,token,separator)){
        tokens.push_back(token);
    };
    return tokens;
};

int main(int argc,char** argv){
    if (argc<2){
        std::cout << "Usage: " << argv[0] << "<filemesh path> <opt: output OBJ path>";
        return 1;
    };
    std::string path = argv[1];
    FILE* fd=fopen(path.c_str(), "rb");
    if (!fd){
        print_err("Failed to open file "+path);
        return 1;
    };
    char ver[32]={0};
    //I hate this line with a burning passion but its all we got so.
    fgets(ver,sizeof(ver),fd);
    std::string vStr(ver);
    size_t space_pos=vStr.find(" ");
    std::string version=(space_pos!= std::string::npos) ? vStr.substr(space_pos + 1) : "";
    if (!version.empty() && version.back()=="\n"){
        version.pop_back();
    };
    bool supported=false;
    for (const char* v:supportedVersions){
        if (version==v){
            supported=true;
            break;
        };
    };
    if (!supported){
        print_err("File Version not supported.");
        fclose(fd);
        return 1;
    };
    print_info("FileMesh v"+version+" file detected. Parsing...");

};
