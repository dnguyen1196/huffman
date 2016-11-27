#include <HuffmanTree.h>
#include <fstream>

class Hdecompressor {
public:
    Hdecompressor();
    ~Hdecompressor();
    void generateEncodingScheme(std::ifstream& in);
    void decompressFile(std::ifstream& in, std::ofstream& out);

private:
    HuffmanTree tree;
};