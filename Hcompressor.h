#include <HuffmanTree.h>
#include <fstream>

class Hcompressor {
public:
    Hcompressor();
    ~Hcompressor();
    void validateFile(std::ifstream& fp);
    void compressFile(std::ifstream& in, std::ofstream& out);

private:
    HuffmanTree tree;
};