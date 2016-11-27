#include <Hdecompressor.h>
#include <iostream>

using namespace std;

Hdecompressor::Hdecompressor(){

}

Hdecompressor::~Hdecompressor(){

}

void Hdecompressor::generateEncodingScheme(ifstream& in) {
    this->tree.createTreeFromScheme(in);
}

void Hdecompressor::decompressFile(ifstream& in, ofstream& out) {
    this->tree.decodeFile(in, out);
}