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
    uint facs_dat_form;
    uint facs_dat_size;
} mesh5Head;

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

/*
typedef struct {
	QuantizedMatrix px;
	QuantizedMatrix py;
	QuantizedMatrix pz;
	QuantizedMatrix rx;
	QuantizedMatrix ry;
	QuantizedMatrix rz;
} QuantizedTransforms;
 */
typedef struct {	
    uint sizeof_face_bone_names_blob;
    uint sizeof_face_ctrl_names_blob;
    unsigned long sizeof_quant_tf;
    uint sizeof_two_pose_correctives;
    uint sizeof_three_pose_correctives;
    byte* face_bone_names_blob;
    byte* face_ctrl_names_blob;

} meshFacsData;

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
typedef struct {
    mesh5Head header;
    meshVertex* verts;
    meshSkinning* skinning;
    meshFace* faces;
    uint* lod_offsets;
    meshBone* bones;
    byte* bone_names;
    meshSubset* subsets;

} mesh5;

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
        meshVertex v=mesh.verts[i];
        objData+="v "+std::to_string(v.px)+" "+std::to_string(v.py)+" "+std::to_string(v.pz)+" "+std::to_string(v.r)+" "+std::to_string(v.g)+" "+std::to_string(v.b)+"\n# alpha: "+std::to_string(v.a)+"\n";
        objData+="vn "+std::to_string(v.nx)+" "+std::to_string(v.ny)+" "+std::to_string(v.nz)+""+"\n";
        objData+="vt "+std::to_string(v.tu)+" "+std::to_string(v.tv)+"\n";
    };
    for (uint i=0;i<mesh.header.face_cnt;i++){
        objData+="f "+std::to_string(mesh.faces[i].a+1)+"/"+std::to_string(mesh.faces[i].a+1)+"/"+std::to_string(mesh.faces[i].a+1)+" "+
                          std::to_string(mesh.faces[i].b+1)+"/"+std::to_string(mesh.faces[i].b+1)+"/"+std::to_string(mesh.faces[i].b+1)+" "+
                          std::to_string(mesh.faces[i].c+1)+"/"+std::to_string(mesh.faces[i].c+1)+"/"+std::to_string(mesh.faces[i].c+1)+"\n";
    };
    return objData;
};
std::string convert_to_obj(mesh3& mesh,bool preserve_LOD){
    std::string objData="# Generated from FileMesh v3.00/v3.01\n";
    for (uint i=0;i<mesh.header.vert_cnt;i++){
        meshVertex v=mesh.verts[i];
        objData+="v "+std::to_string(v.px)+" "+std::to_string(v.py)+" "+std::to_string(v.pz)+" "+std::to_string(v.r)+" "+std::to_string(v.g)+" "+std::to_string(v.b)+"\n# alpha: "+std::to_string(v.a)+"\n";
        objData+="vn "+std::to_string(v.nx)+" "+std::to_string(v.ny)+" "+std::to_string(v.nz)+""+"\n";
        objData+="vt "+std::to_string(v.tu)+" "+std::to_string(v.tv)+"\n";
    };
    if (preserve_LOD){
        int meshesWritten=0;
        for (uint i=0;i<mesh.header.lod_offset_cnt;i++){
            objData+="# LOD Mesh "+std::to_string(i)+" Offset: "+std::to_string(mesh.lod_offsets[i])+"\n";
            meshesWritten++;
            uint startOffset=mesh.lod_offsets[i];
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
    }else{
        uint startOffset=mesh.lod_offsets[0];
        uint endOffset=(1<mesh.header.lod_offset_cnt) ? mesh.lod_offsets[1] : mesh.header.face_cnt;
        for (uint mi=startOffset;mi<endOffset;mi++){
            objData+="f "+std::to_string(mesh.faces[mi].a+1)+"/"+std::to_string(mesh.faces[mi].a+1)+"/"+std::to_string(mesh.faces[mi].a+1)+" "+
                                std::to_string(mesh.faces[mi].b+1)+"/"+std::to_string(mesh.faces[mi].b+1)+"/"+std::to_string(mesh.faces[mi].b+1)+" "+
                                std::to_string(mesh.faces[mi].c+1)+"/"+std::to_string(mesh.faces[mi].c+1)+"/"+std::to_string(mesh.faces[mi].c+1)+"\n";
        };
    };
    return objData;
};
std::string convert_to_obj(mesh4& mesh,bool preserve_LOD){
    std::string objData="# Generated from FileMesh v4.00/v4.01\n";
    for (uint i=0;i<mesh.header.vert_cnt;i++){
        meshVertex v=mesh.verts[i];
        objData+="v "+std::to_string(v.px)+" "+std::to_string(v.py)+" "+std::to_string(v.pz)+" "+std::to_string(v.r)+" "+std::to_string(v.g)+" "+std::to_string(v.b)+"\n# alpha: "+std::to_string(v.a)+"\n";
        objData+="vn "+std::to_string(v.nx)+" "+std::to_string(v.ny)+" "+std::to_string(v.nz)+""+"\n";
        objData+="vt "+std::to_string(v.tu)+" "+std::to_string(v.tv)+"\n";
    };
    if (preserve_LOD){
        int meshesWritten=0;
        for (uint i=0;i<mesh.header.lod_offset_cnt;i++){
            objData+="# LOD Mesh "+std::to_string(i)+" Offset: "+std::to_string(mesh.lod_offsets[i])+"\n";
            meshesWritten++;
            uint startOffset=mesh.lod_offsets[i];
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
    }else{
        uint startOffset=mesh.lod_offsets[0];
        uint endOffset=(1<mesh.header.lod_offset_cnt) ? mesh.lod_offsets[1] : mesh.header.face_cnt;
        for (uint mi=startOffset;mi<endOffset;mi++){
            objData+="f "+std::to_string(mesh.faces[mi].a+1)+"/"+std::to_string(mesh.faces[mi].a+1)+"/"+std::to_string(mesh.faces[mi].a+1)+" "+
                            std::to_string(mesh.faces[mi].b+1)+"/"+std::to_string(mesh.faces[mi].b+1)+"/"+std::to_string(mesh.faces[mi].b+1)+" "+
                            std::to_string(mesh.faces[mi].c+1)+"/"+std::to_string(mesh.faces[mi].c+1)+"/"+std::to_string(mesh.faces[mi].c+1)+"\n";
        };
    };
    return objData;
};

std::string convert_to_obj(mesh5& mesh,bool preserve_LOD){
    std::string objData="# Generated from FileMesh v5.00\n";
    for (uint i=0;i<mesh.header.vert_cnt;i++){
        meshVertex v=mesh.verts[i];
        objData+="v "+std::to_string(v.px)+" "+std::to_string(v.py)+" "+std::to_string(v.pz)+" "+std::to_string(v.r)+" "+std::to_string(v.g)+" "+std::to_string(v.b)+"\n# alpha: "+std::to_string(v.a)+"\n";
        objData+="vn "+std::to_string(v.nx)+" "+std::to_string(v.ny)+" "+std::to_string(v.nz)+""+"\n";
        objData+="vt "+std::to_string(v.tu)+" "+std::to_string(v.tv)+"\n";
    };
    if (preserve_LOD){
        int meshesWritten=0;
        for (uint i=0;i<mesh.header.lod_offset_cnt;i++){
            objData+="# LOD Mesh "+std::to_string(i)+" Offset: "+std::to_string(mesh.lod_offsets[i])+"\n";
            meshesWritten++;
            uint startOffset=mesh.lod_offsets[i];
            uint endOffset=(i+1<mesh.header.lod_offset_cnt) ? mesh.lod_offsets[i+1] : mesh.header.face_cnt;
            print_info(std::to_string(endOffset)+" "+std::to_string(startOffset));
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
    }else{
        uint startOffset=mesh.lod_offsets[0];
        uint endOffset=(1<mesh.header.lod_offset_cnt) ? mesh.lod_offsets[1] : mesh.header.face_cnt;
        print_info(std::to_string(endOffset)+" "+std::to_string(startOffset));
        for (uint mi=startOffset;mi<endOffset;mi++){
            objData+="f "+std::to_string(mesh.faces[mi].a+1)+"/"+std::to_string(mesh.faces[mi].a+1)+"/"+std::to_string(mesh.faces[mi].a+1)+" "+
                            std::to_string(mesh.faces[mi].b+1)+"/"+std::to_string(mesh.faces[mi].b+1)+"/"+std::to_string(mesh.faces[mi].b+1)+" "+
                            std::to_string(mesh.faces[mi].c+1)+"/"+std::to_string(mesh.faces[mi].c+1)+"/"+std::to_string(mesh.faces[mi].c+1)+"\n";
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
        }else if(argv[i]==std::string("--no-preserve-LOD")){
            preserve_LOD=false;
        }else if(argv[i]==std::string("-o")){
            outputOffset=i+1;
            i++; //skip next arg as its the output path
        }else if(argv[i]==std::string("-h") or argv[i]==std::string("--help")){
            print_usage(argv);
            return 0;
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
        if (!no_output){
            std::string objData=convert_to_obj(mesh);
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
        if (!no_output){
            std::string objData=convert_to_obj(mesh,preserve_LOD);
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
        if (readBytes!=mesh.header.vert_cnt) {
            print_err("Failed to read vertexes.");
            return 1;
        };
        if (mesh.header.bone_cnt>0){
            mesh.skinning=new meshSkinning[mesh.header.vert_cnt];
            readBytes=fread(mesh.skinning,sizeof(meshSkinning),mesh.header.vert_cnt,fd);
            if (mesh.header.bone_cnt>0){
                mesh.skinning=new meshSkinning[mesh.header.vert_cnt];
                readBytes=fread(mesh.skinning,sizeof(meshSkinning),mesh.header.vert_cnt,fd);
                    if (readBytes!=mesh.header.vert_cnt) {
                    print_err("Failed to read skinning.");
                    return 1;
                };
            };
        };
        mesh.faces=new meshFace[mesh.header.face_cnt];
        print_info("Polygon Count (triangles): "+std::to_string(mesh.header.face_cnt));
        readBytes=fread(mesh.faces,sizeof(meshFace),mesh.header.face_cnt,fd);
        if (readBytes!=mesh.header.face_cnt) {
            print_err("Failed to read faces.");
            return 1;
        };
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
        readBytes=fread(mesh.bone_names,1,mesh.header.sizeof_bone_names,fd);
        if (readBytes!=mesh.header.sizeof_bone_names) {
            print_err("Failed to read bone metadata.");
            return 1;
        };
        mesh.subsets=new meshSubset[mesh.header.subset_cnt];
        readBytes=fread(mesh.subsets,sizeof(meshSubset),mesh.header.subset_cnt,fd);
        if (readBytes!=mesh.header.subset_cnt) {
            print_err("Failed to read subsets.");
            return 1;
        };
        if (!no_output){
            std::string objData=convert_to_obj(mesh,preserve_LOD);
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
    }else if(version=="5.00"){
        mesh5 mesh;
        size_t readBytes=fread(&mesh.header,sizeof(mesh5Head),1,fd);
        mesh.verts=new meshVertex[mesh.header.vert_cnt];
        readBytes=fread(mesh.verts,sizeof(meshVertex),mesh.header.vert_cnt,fd);
        if (readBytes!=mesh.header.vert_cnt) {
            print_err("Failed to read vertexes.");
            return 1;
        };
        if (mesh.header.bone_cnt>0){
            mesh.skinning=new meshSkinning[mesh.header.vert_cnt];
            readBytes=fread(mesh.skinning,sizeof(meshSkinning),mesh.header.vert_cnt,fd);
            if (readBytes!=mesh.header.vert_cnt) {
                print_err("Failed to read skinning.");
                return 1;
            };
        };
        mesh.faces=new meshFace[mesh.header.face_cnt];
        print_info("Polygon Count (triangles): "+std::to_string(mesh.header.face_cnt));
        readBytes=fread(mesh.faces,sizeof(meshFace),mesh.header.face_cnt,fd);
        if (readBytes!=mesh.header.face_cnt) {
            print_err("Failed to read faces.");
            return 1;
        };
        mesh.lod_offsets = new uint[mesh.header.lod_offset_cnt];
        readBytes=fread(mesh.lod_offsets,4,mesh.header.lod_offset_cnt,fd);
        for (uint i = 0; i < mesh.header.lod_offset_cnt; i++) {
            mesh.lod_offsets[i] /= sizeof(meshFace);
        };
        for (uint i=0;i<mesh.header.lod_offset_cnt;i++){
            print_info("Detected LOD offset: "+std::to_string(mesh.lod_offsets[i]));
        };
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
        readBytes=fread(mesh.bone_names,1,mesh.header.sizeof_bone_names,fd);
        if (readBytes!=mesh.header.sizeof_bone_names) {
            print_err("Failed to read bone metadata.");
            return 1;
        };
        mesh.subsets=new meshSubset[mesh.header.subset_cnt];
        readBytes=fread(mesh.subsets,sizeof(meshSubset),mesh.header.subset_cnt,fd);
        if (readBytes!=mesh.header.subset_cnt) {
            print_err("Failed to read subsets.");
            return 1;
        };
        if (mesh.header.facs_dat_size > 0 && mesh.header.facs_dat_form == 1) {
            fseek(fd,mesh.header.facs_dat_size,SEEK_CUR); // skip the FACS blob
        };
        /*if (!no_output){
            std::string objData=convert_to_obj(mesh,preserve_LOD);
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
        */
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