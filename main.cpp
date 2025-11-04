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

std::string convert_to_obj(mesh2& mesh){
    std::string objData="# Generated from FileMesh v2.00\n";
    for (uint i=0;i<mesh.header.vert_cnt;i++){
        objData+="v "+std::to_string(mesh.verts[i].px)+" "+std::to_string(mesh.verts[i].py)+" "+std::to_string(mesh.verts[i].pz)+"\n";
        objData+="vn "+std::to_string(mesh.verts[i].nx)+" "+std::to_string(mesh.verts[i].ny)+" "+std::to_string(mesh.verts[i].nz)+"\n";
        objData+="vt "+std::to_string(mesh.verts[i].tu)+" "+std::to_string(mesh.verts[i].tv)+"\n";
    };
    for (uint i=0;i<mesh.header.face_cnt;i++){
        objData+="f "+std::to_string(mesh.faces[i].a+1)+"/"+std::to_string(mesh.faces[i].a+1)+"/"+std::to_string(mesh.faces[i].a+1)+" "+
                          std::to_string(mesh.faces[i].b+1)+"/"+std::to_string(mesh.faces[i].b+1)+"/"+std::to_string(mesh.faces[i].b+1)+" "+
                          std::to_string(mesh.faces[i].c+1)+"/"+std::to_string(mesh.faces[i].c+1)+"/"+std::to_string(mesh.faces[i].c+1)+"\n";
    };
    return objData;
};
std::string convert_to_obj(mesh3& mesh){
    std::string objData="# Generated from FileMesh v3.00/v3.01\n";
    for (uint i=0;i<mesh.header.vert_cnt;i++){
        objData+="v "+std::to_string(mesh.verts[i].px)+" "+std::to_string(mesh.verts[i].py)+" "+std::to_string(mesh.verts[i].pz)+"\n";
        objData+="vn "+std::to_string(mesh.verts[i].nx)+" "+std::to_string(mesh.verts[i].ny)+" "+std::to_string(mesh.verts[i].nz)+"\n";
        objData+="vt "+std::to_string(mesh.verts[i].tu)+" "+std::to_string(mesh.verts[i].tv)+"\n";
    };
    int meshesWritten=0;
    for (uint i=0;i<mesh.header.lod_offset_cnt;i++){
        objData+="# LOD Mesh "+std::to_string(i)+" Offset: "+std::to_string(mesh.lod_offsets[i])+"\n";
        meshesWritten++;
        startOffset=mesh.lod_offsets[i];
        uint endOffset=(i+1<mesh.header.lod_offset_cnt) ? mesh.lod_offsets[i+1] : mesh.header.face_cnt;
        if (i>0){
            objData+="# LOD Mesh "+std::to_string(i)+" faces commented out.\n";
        };
        for (uint mi=startOffset;mi<endOffset;mi++){
            if (i>0){
                objData+="# ";
            };
            objData+="f "+std::to_string(mesh.faces[mi].a+1)+"/"+std::to_string(mesh.faces[mi].a+1)+"/"+std::to_string(mesh.faces[mi].a+1)+" "+
                            std::to_string(mesh.faces[mi].b+1)+"/"+std::to_string(mesh.faces[mi].b+1)+"/"+std::to_string(mesh.faces[mi].b+1)+" "+
                            std::to_string(mesh.faces[mi].c+1)+"/"+std::to_string(mesh.faces[mi].c+1)+"/"+std::to_string(mesh.faces[mi].c+1)+"\n";
        };
    };
    return objData;
};
void print_usage(char** argv){
    std::cout << "Usage: " << argv[0] << " <filemesh path> <opt: -o output OBJ path> [--no-output]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -o <output OBJ path>   Specify output path for the OBJ file. If not provided, outputs to stdout." << std::endl;
    std::cout << "  --no-output            Parses the FileMesh file but does not produce any output." << std::endl;
};
int main(int argc,char** argv){
    if (argc<2){
        print_usage(argv);
        return 1;
    };
    std::string path = argv[1];
    bool no_output=false;
    int outputOffset=0;
    for (uint i=0;i<argc;i++){
        if (argv[i]==std::string("--no-output")){
            print_info("No output flag detected, exiting after parsing.");
            no_output=true;
        }else if(argv[i]==std::string("-h") or argv[i]==std::string("--help")){
            print_usage(argv);
            return 0;
        }else if(argv[i]==std::string("-o")){
            outputOffset=i+1;
            i++; //skip next arg as its the output path
        };
    };
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
        std::string objData=convert_to_obj(mesh);
        if (!no_output){
            if (argc>=3){
                std::string outPath=argv[outputOffset];
                FILE* outFd=fopen(outPath.c_str(),"w");
                if (!outFd){
                    print_err("Failed to open output file "+outPath);
                }else{
                    fwrite(objData.c_str(),1,objData.size(),outFd);
                    fclose(outFd);
                    print_info("OBJ file written to "+outPath);
                };
            }else{
                std::cout << objData << std::endl;
            };
        };
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
        std::string objData=convert_to_obj(mesh);
        if (!no_output){
            if (argc>=3){
                std::string outPath=argv[outputOffset];
                FILE* outFd=fopen(outPath.c_str(),"w");
                if (!outFd){
                    print_err("Failed to open output file "+outPath);
                }else{
                    fwrite(objData.c_str(),1,objData.size(),outFd);
                    fclose(outFd);
                    print_info("OBJ file written to "+outPath);
                };
            }else{
                std::cout << objData << std::endl;
            };
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