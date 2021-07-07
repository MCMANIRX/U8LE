//Litte Endian U8 Unpacker
//Mateo Smith
//7/7/21

#include <iostream>
#include <fstream>
#include <stdint.h>
#include <vector>
#include <string>
#include <direct.h>


struct node {

    uint8_t flag;
    uint32_t nameOffset;
    uint32_t dataOffset;
    uint32_t dirIndex;
    uint32_t dataSize;
    uint32_t nodeIndex;
    std::string fileName;

};

struct nodeArray {
    std::vector<node> files ;
};


std::ifstream r;
std::ofstream w;

uint32_t r32(){
    int ret;
    r.read((char*)&ret,4);
    return ret;
}

uint8_t r8(){
    uint8_t ret;
    r.read((char*)&ret,1);
    return ret;
}

int pos() {return r.tellg();}

void skip(int dist) {
     r.seekg(pos()+dist); }

std::string getName(int npos) {

    char fname[128];

    r.seekg(npos);
    r.getline(fname,128,'\0');

    return std::string(fname);  
     
}



int main(int argc, char** argv) {

        if(argc!=2){
        std::cout << "Invalid amount of input files. Exiting.";
        exit(0);
    }

    char wd[2048];
    getcwd(wd, 2048);

    r.open(argv[1], std::ios::binary | std::ios::in);

    if(r32() != 0x55AA382D){
        std::cout << "Not a valid PS2 U8 archive. Exiting.";
        exit(0);
    }

    std::string path = std::string(argv[1]);

    size_t lastindex = path.find_last_of("."); 
     path = path.substr(0, lastindex)+"/"; 

    std::string concat;

    _mkdir(path.c_str());
    


    int rootNodeOffset = r32();
    int nodeSize = r32();
    int fileOffset = r32();

    r.seekg(pos()+0x10);


    node root;

    r.seekg(pos()+0x8);
    r.read((char*)&root.dataSize,4);

     int nodeCount = 0xc*(root.dataSize-0xc);

    nodeArray nodes;

    int savePos = 0;

    printf("%x",nodeCount);



    for(int i =0 ; i < nodeCount; ++i) {

      nodes.files.push_back(node());

      nodes.files[i].nameOffset = r8() | r8() << 8 | r8() <<16;
      nodes.files[i].flag = r8();

      savePos = r.tellg();
      nodes.files[i].fileName = getName((0xc*root.dataSize)+0x20+nodes.files[i].nameOffset);
      r.seekg(savePos);


      if(nodes.files[i].flag == 1) {

          nodes.files[i].dirIndex = r32();
          nodes.files[i].nodeIndex = r32();

          concat = nodes.files[i].fileName + std::string("/");
          _mkdir((path+concat).c_str());

          savePos+=8;

      }

      else {

          nodes.files[i].dataOffset = r32() ;
          nodes.files[i].dataSize = r32() ;

          savePos+=8;

          
            r.seekg(nodes.files[i].dataOffset);


    w.open((path+concat+std::string("/")+nodes.files[i].fileName).c_str(), std::ios::out | std::ios::binary);
    
    char buf[1024];
    uint32_t diff = 0;
    uint32_t b = 0;

    while(diff < nodes.files[i].dataSize) {

        if(nodes.files[i].dataSize-diff < 1024 )
            b = nodes.files[i].dataSize-diff;
        else b = 1024;
        
        r.read(buf,b);
        w.write(buf,b);
        diff+=b;
    }

        w.close();
        printf("\nFile %d written.",i);
        r.seekg(savePos);

            }

   /* nodes.files.push_back({
    r8() | r8() << 8 | r8() <<16,
    r8(),
    r32(),
    r32(),
    "asdfg"
});*/
  
       
}

}