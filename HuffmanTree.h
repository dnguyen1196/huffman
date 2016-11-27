#ifndef _HUFFMAN_TREE_H
#define _HUFFMAN_TREE_H
#include <fstream>

struct Node {
    uint64_t ch;
    uint64_t f;
    Node* left;
    Node* right;
};

class HuffmanTree {
    public:
        HuffmanTree();

        ~HuffmanTree();

        void createTreeFromFile(std::ifstream& fp);

        void createTreeFromScheme(std::ifstream& fp);

        void encodeFile(std::ifstream& in, std::ofstream& out);

        void decodeFile(std::ifstream& in, std::ofstream& out);

    private:
        Node* root;
};

#endif