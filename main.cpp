#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
/*NOTE:
All FileMesh versions start with "version (version)\n", INCLUDING binary ones
*/

typedef unsigned char byte;
typedef signed char sbyte;
// support soon: "1.00", "1.01"
const char* supportedVersions[] = {"2.00","3.00","3.01","4.00","4.01","5.00"};
typedef struct {
    unsigned short sizeof_mesh2Head;
    byte sizeof_meshVertex;
    byte sizeof_meshFace;
    uint vert_cnt;
    uint face_cnt;
} mesh2Head;

typedef struct {
    unsigned short sizeof_mesh3Head;
    byte sizeof_meshVertex;
    byte sizeof_meshFace;
    unsigned short sizeof_lod_offset; //unused for some reason, always 4
    unsigned short lod_offset_cnt;
    uint vert_cnt;
    uint face_cnt;
} mesh3Head;

typedef struct {
    float px,py,pz;
    float nx,ny,nz;
    float tu,tv;
    sbyte tx,ty,tz,ts;
    byte r,g,b,a; // THIS IS NOT ALWAYS INCLUDED!
} meshVertex;

typedef struct {
    float px,py,pz;
    float nx,ny,nz;
    float tu,tv;
    sbyte tx,ty,tz,ts;
} meshVertexNoColor;

typedef struct {
    uint a;
    uint b;
    uint c;
} meshFace;

typedef struct {
    mesh2Head header;
    meshVertex* verts;
    meshFace* faces;
} mesh2;

typedef struct {
    mesh3Head header;
    meshVertex* verts;
    meshFace* faces;
    uint* lod_offsets;
} mesh3;

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
        std::cout << "Usage: " << argv[0] << " <filemesh path> <opt: output OBJ path>" << std::endl;
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
    char* browhy=fgets(ver,sizeof(ver),fd);
    std::string vStr(ver);
    size_t space_pos=vStr.find(" ");
    std::string version=(space_pos!= std::string::npos) ? vStr.substr(space_pos + 1) : "";
    if (!version.empty() && version.back()=='\n'){
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
    if (version=="2.00"){
        mesh2 mesh;
        size_t readBytes=fread(&mesh.header,sizeof(mesh2Head),1,fd);
        meshVertex* verts=new meshVertex[mesh.header.vert_cnt];
        for (uint i=0;i<mesh.header.vert_cnt;i++){
            byte vertexSize=mesh.header.sizeof_meshVertex;
            if (vertexSize==36){
                meshVertexNoColor temp;
                readBytes=fread(&temp,sizeof(meshVertexNoColor),1,fd);
                // the below code is very cursed, but gets the job done for no color vertexes
                verts[i].px = temp.px;
                verts[i].py = temp.py;
                verts[i].pz = temp.pz;
                verts[i].nx = temp.nx;
                verts[i].ny = temp.ny;
                verts[i].nz = temp.nz;
                verts[i].tu = temp.tu;
                verts[i].tv = temp.tv;
                verts[i].tx = temp.tx;
                verts[i].ty = temp.ty;
                verts[i].tz = temp.tz;
                verts[i].ts = temp.ts;
                verts[i].r = 255;
                verts[i].g = 255;
                verts[i].b = 255;
                verts[i].a = 255;
            }else if(vertexSize==40){
                //normal vertex logic
                readBytes=fread(&verts[i],sizeof(meshVertex),1,fd);
            }else{
                print_err("Vertex length invalid.");
                delete[] verts;
                fclose(fd);
                return 1;
            };
        };
        mesh.verts=verts;
        mesh.faces=new meshFace[mesh.header.face_cnt];
        print_info("Polygon Count (triangles): "+std::to_string(mesh.header.face_cnt));
        readBytes=fread(mesh.faces,sizeof(meshFace),mesh.header.face_cnt,fd);
        delete[] verts;
        delete[] mesh.faces;
        fclose(fd);
    }else if (version=="3.00" or version=="3.01"){
        mesh3 mesh;
        size_t readBytes=fread(&mesh.header,sizeof(mesh3Head),1,fd);
        meshVertex* verts=new meshVertex[mesh.header.vert_cnt];
        for (uint i=0;i<mesh.header.vert_cnt;i++){
            byte vertexSize=mesh.header.sizeof_meshVertex;
            if (vertexSize==36){
                meshVertexNoColor temp;
                readBytes=fread(&temp,sizeof(meshVertexNoColor),1,fd);
                // the below code is also very cursed, but gets the job done for no color vertexes
                verts[i].px = temp.px;
                verts[i].py = temp.py;
                verts[i].pz = temp.pz;
                verts[i].nx = temp.nx;
                verts[i].ny = temp.ny;
                verts[i].nz = temp.nz;
                verts[i].tu = temp.tu;
                verts[i].tv = temp.tv;
                verts[i].tx = temp.tx;
                verts[i].ty = temp.ty;
                verts[i].tz = temp.tz;
                verts[i].ts = temp.ts;
                verts[i].r = 255;
                verts[i].g = 255;
                verts[i].b = 255;
                verts[i].a = 255;
            }else if(vertexSize==40){
                //normal vertex logic
                readBytes=fread(&verts[i],sizeof(meshVertex),1,fd);
            }else{
                print_err("Vertex length invalid.");
                delete[] verts;
                fclose(fd);
                return 1;
            };
        };
        mesh.verts=verts;
        mesh.faces=new meshFace[mesh.header.face_cnt];
        print_info("Polygon Count (triangles): "+std::to_string(mesh.header.face_cnt));
        readBytes=fread(mesh.faces,sizeof(meshFace),mesh.header.face_cnt,fd);
        //additional data for LOD stuff
        mesh.lod_offsets = new uint[mesh.header.lod_offset_cnt];
        readBytes=fread(mesh.lod_offsets,mesh.header.sizeof_lod_offset,mesh.header.lod_offset_cnt,fd);
        if (readBytes!=mesh.header.lod_offset_cnt) {
            print_err("Failed to read LOD offsets.");
            return 1;
        };
        delete[] verts;
        delete[] mesh.faces;
        delete[] mesh.lod_offsets;
        fclose(fd);
    }else if(version=="4.00" or version=="4.01"){
        print_info("yes");
    };
    return 0;
};
