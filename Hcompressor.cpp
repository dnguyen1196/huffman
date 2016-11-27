#include <Hcompressor.h>

using namespace std;

Hcompressor::Hcompressor(){

}

Hcompressor::~Hcompressor(){

}

void Hcompressor::validateFile(ifstream& fp){
    /* Validate the file for correct format */
    if (!fp.is_open()) exit(1);

    /* If incorrect format then generates an error msg */
    
}

void Hcompressor::compressFile(ifstream& in, ofstream& out){
    (this->tree).createTreeFromFile(in);
    tree.encodeFile(in, out);
    if (!out.is_open()) exit(1);
}
