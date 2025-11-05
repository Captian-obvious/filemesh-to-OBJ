#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <type_traits>
#include <cstdint>
#include <algorithm>
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
    unsigned short sizeof_mesh4Head;
    unsigned short lod_type;
    uint vert_cnt;
    uint face_cnt;
    unsigned short lod_offset_cnt;
    unsigned short bone_cnt;
    uint sizeof_bone_names;
    unsigned short subset_cnt;
    byte hq_lod_cnt;
    byte unused;
} mesh4Head;

enum meshLodType {
    None = 0,
	Unknown = 1,
	RbxSimplifier = 2,
	ZeuxMeshOptimizer = 3,
};

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
    byte subset_indexes;
    byte bone_weights;
} meshSkinning;

typedef struct {
    uint bone_name_ind;
    unsigned short parent_ind;
    unsigned short lod_parent_ind;
    float culling;
	float r00,r01,r02;
	float r10,r11,r12;
	float r20,r21,r22;
	float x,y,z;
} meshBone;

typedef struct {
    uint a;
    uint b;
    uint c;
} meshFace;

typedef struct {
    uint faces_begin;
    uint faces_length;
    uint verts_begin;
    uint verts_length;
    uint bone_ind_cnt;
    unsigned short bone_inds[26];
} meshSubset;

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

typedef struct {
    mesh4Head header;
    meshVertex* verts;
    meshSkinning* skinning;
    meshFace* faces;
    uint* lod_offsets;
    meshBone* bones;
    byte* bone_names;
    meshSubset* subsets;
} mesh4;

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

template<typename MeshType>
struct MeshTraits;

template<>
struct MeshTraits<mesh2> {
    static constexpr const char* version = "FileMesh v2.00";
    static constexpr bool has_lod = false;
    static uint get_vertex_count(const mesh2& m) { return m.header.vert_cnt; }
    static uint get_face_count(const mesh2& m) { return m.header.face_cnt; }
};

template<>
struct MeshTraits<mesh3> {
    static constexpr const char* version = "FileMesh v3.00/v3.01";
    static constexpr bool has_lod = true;
    static uint get_vertex_count(const mesh3& m) { return m.header.vert_cnt; }
    static uint get_face_count(const mesh3& m) { return m.header.face_cnt; }
    static const uint* get_lod_offsets(const mesh3& m) { return m.lod_offsets; }
    static uint get_lod_offset_count(const mesh3& m) { return m.header.lod_offset_cnt; }
};

template<typename MeshType>
std::string convert_to_obj(const MeshType& mesh, bool preserve_LOD = false) {
    std::string objData = "# Generated from " + std::string(MeshTraits<MeshType>::version) + "\n";
    for (uint i = 0; i < MeshTraits<MeshType>::get_vertex_count(mesh); ++i) {
        const auto& v = mesh.verts[i];
        objData += "v " + std::to_string(v.px) + " " + std::to_string(v.py) + " " + std::to_string(v.pz) + " " +
                   std::to_string(v.r) + " " + std::to_string(v.g) + " " + std::to_string(v.b) + "\n# alpha: " +
                   std::to_string(v.a) + "\n";
        objData += "vn " + std::to_string(v.nx) + " " + std::to_string(v.ny) + " " + std::to_string(v.nz) + "\n";
        objData += "vt " + std::to_string(v.tu) + " " + std::to_string(v.tv) + "\n";
    };
    if constexpr (MeshTraits<MeshType>::has_lod) {
        if (preserve_LOD) {
            for (uint i = 0; i < MeshTraits<MeshType>::get_lod_offset_count(mesh); ++i) {
                uint start = MeshTraits<MeshType>::get_lod_offsets(mesh)[i];
                uint end = (i + 1 < MeshTraits<MeshType>::get_lod_offset_count(mesh)) ?
                           MeshTraits<MeshType>::get_lod_offsets(mesh)[i + 1] :
                           MeshTraits<MeshType>::get_face_count(mesh);
                objData += "# LOD Mesh " + std::to_string(i) + " Offset: " + std::to_string(start) + "\n";
                if (i > 0) objData += "# LOD Mesh " + std::to_string(i) + " faces commented out.\n";
                for (uint mi = start; mi < end; ++mi) {
                    if (i > 0) objData += "# ";
                    const auto& f = mesh.faces[mi];
                    objData += "f " + std::to_string(f.a + 1) + "/" + std::to_string(f.a + 1) + "/" + std::to_string(f.a + 1) + " " +
                               std::to_string(f.b + 1) + "/" + std::to_string(f.b + 1) + "/" + std::to_string(f.b + 1) + " " +
                               std::to_string(f.c + 1) + "/" + std::to_string(f.c + 1) + "/" + std::to_string(f.c + 1) + "\n";
                }
            }
        } else {
            uint start = MeshTraits<MeshType>::get_lod_offsets(mesh)[0];
            uint end = (1 < MeshTraits<MeshType>::get_lod_offset_count(mesh)) ?
                       MeshTraits<MeshType>::get_lod_offsets(mesh)[1] :
                       MeshTraits<MeshType>::get_face_count(mesh);
            for (uint mi = start; mi < end; ++mi) {
                const auto& f = mesh.faces[mi];
                objData += "f " + std::to_string(f.a + 1) + "/" + std::to_string(f.a + 1) + "/" + std::to_string(f.a + 1) + " " +
                           std::to_string(f.b + 1) + "/" + std::to_string(f.b + 1) + "/" + std::to_string(f.b + 1) + " " +
                           std::to_string(f.c + 1) + "/" + std::to_string(f.c + 1) + "/" + std::to_string(f.c + 1) + "\n";
            }
        }
    } else {
        for (uint i = 0; i < MeshTraits<MeshType>::get_face_count(mesh); ++i) {
            const auto& f = mesh.faces[i];
            objData += "f " + std::to_string(f.a + 1) + "/" + std::to_string(f.a + 1) + "/" + std::to_string(f.a + 1) + " " +
                       std::to_string(f.b + 1) + "/" + std::to_string(f.b + 1) + "/" + std::to_string(f.b + 1) + " " +
                       std::to_string(f.c + 1) + "/" + std::to_string(f.c + 1) + "/" + std::to_string(f.c + 1) + "\n";
        };
    };
    return objData;
};
void print_usage(char** argv){
    std::cout << "Usage: " << argv[0] << " <filemesh path> <opt: -o output OBJ path> [--no-output] [--no-preserve-LOD]" << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -o <output OBJ path>   Specify output path for the OBJ file. If not provided, outputs to stdout." << std::endl;
    std::cout << "  --no-output            Parses the FileMesh file but does not produce any output." << std::endl;
    std::cout << "  --no-preserve-LOD      Determines if it doesnt preserve Level Of Detail (LOD) of the mesh" << std::endl;
};
int main(int argc,char** argv){
    if (argc<2){
        print_usage(argv);
        return 1;
    };
    std::string path = argv[1];
    bool no_output=false;
    bool preserve_LOD=true;
    int outputOffset=0;
    for (uint i=0;i<argc;i++){
        if (argv[i]==std::string("--no-output")){
            print_info("No output flag detected, exiting after parsing.");
            no_output=true;
        }else if(argv[i]==std::string("-h") or argv[i]==std::string("--help")){
            print_usage(argv);
            return 0;
        }else if(argv[i]==std::string("--no-preserve-LOD")){
            preserve_LOD=false;
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
        std::string objData=convert_to_obj(mesh,preserve_LOD);
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
        mesh4 mesh;
        size_t readBytes=fread(&mesh.header,sizeof(mesh4Head),1,fd);
        mesh.verts=new meshVertex[mesh.header.vert_cnt];
        readBytes=fread(mesh.verts,sizeof(meshVertex),mesh.header.vert_cnt,fd);
        if (mesh.header.bone_cnt>0){
            mesh.skinning=new meshSkinning[mesh.header.vert_cnt];
            readBytes=fread(mesh.skinning,sizeof(meshSkinning),mesh.header.vert_cnt,fd);
        };
        mesh.faces=new meshFace[mesh.header.face_cnt];
        print_info("Polygon Count (triangles): "+std::to_string(mesh.header.face_cnt));
        readBytes=fread(mesh.faces,sizeof(meshFace),mesh.header.face_cnt,fd);
        mesh.lod_offsets = new uint[mesh.header.lod_offset_cnt];
        readBytes=fread(mesh.lod_offsets,4,mesh.header.lod_offset_cnt,fd);
        if (readBytes!=mesh.header.lod_offset_cnt) {
            print_err("Failed to read LOD offsets.");
            return 1;
        };
        mesh.bones=new meshBone[mesh.header.bone_cnt];
        readBytes=fread(mesh.bones,sizeof(meshBone),mesh.header.bone_cnt,fd);
        if (readBytes!=mesh.header.bone_cnt) {
            print_err("Failed to read bones.");
            return 1;
        };
        mesh.bone_names=new byte[mesh.header.sizeof_bone_names];
        readBytes=fread(mesh.bone_names,mesh.header.sizeof_bone_names,1,fd);
        mesh.subsets=new meshSubset[mesh.header.subset_cnt];
        readBytes=fread(mesh.subsets,sizeof(meshSubset),mesh.header.subset_cnt,fd);
        std::string objData=convert_to_obj(mesh,preserve_LOD);
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
        delete[] mesh.verts;
        delete[] mesh.faces;
        delete[] mesh.lod_offsets;
        delete[] mesh.bones;
        delete[] mesh.bone_names;
        delete[] mesh.subsets;
        fclose(fd);
    };
    return 0;
};