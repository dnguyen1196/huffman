#include <HuffmanTree.h>
#include <queue>
#include <bitpack.h>
#include <iostream>

using namespace std;

HuffmanTree::HuffmanTree(){
    root = NULL;
}

void postorder_delete(Node* node);
HuffmanTree::~HuffmanTree(){
    /* Recursively delete all the nodes in the tree postorder_delete */
    postorder_delete(root);
}

void postorder_delete(Node* node) {
    if (node == NULL) return;
    else {
        if (node->left == NULL && node->right == NULL){
             delete node;
             node = NULL;
        } else {
            postorder_delete(node->left);
            postorder_delete(node->right);
            delete node;
            node = NULL;
        }
    }
}


/*
 * FUNCTIONS TO GENERATE HUFFMAN TREE FROM A FILE
 * *********************************************/

/* Struct to provide comparison between two nodes */
struct WeightCompare {
    bool operator()(const Node* lhs, const Node* rhs) const{
        return ((lhs->f) > (rhs->f));
    }
};

Node* generate_parent(Node* left, Node* right);
void HuffmanTree::createTreeFromFile(ifstream& fp){
    /* To create the tree, first need to count the frequency of all 
    characters in the text file */
    vector<unsigned int> charCount(256);
    
    /* Method 2 */
    char* c = new char;
    while (!fp.eof()) {
        fp.read(c, 1);
        charCount[(unsigned int) *c]++;
    }
    delete c;

    /* 
    Create a priority queue this priority queue performs comparison 
    based on the f count of the chars
    Populate the priority queue with all the node
    */
    priority_queue<Node*, vector<Node*>, WeightCompare> Q;
    for (int i = 0; i < 255; i++) {
        if (charCount[i] > 0) {
            Node* node = new Node;
            node->ch = (uint64_t) i;
            node->f = charCount[i];
            node->left = node->right = NULL;
            /* Be careful of uninitialized value */
            Q.push(node);
        }
    }
    /* The EOF needs to be part of the tree from the beginning */
    /* Actually assume that the text file does not contain character 255 */
    Node* eof = new Node;
    eof->ch = 255;
    eof->f = 1;
    eof->left = eof->right = NULL;
    Q.push(eof);

    /*
    Generate the full tree from the priority queue
    while the size of the priority queue is > 1
    remove the first two item (smallest f)
    combine them into a new tree
    insert the Node back into the queue
    */
    while (Q.size() > 1) {
        /* Get the two nodes with the smallest frequency */
        Node* left = Q.top();
        Q.pop();
        Node* right = Q.top();
        Q.pop();
        /* Create a new tree that contains these nodes */
        Node* parent = generate_parent(left, right);
        Q.push(parent); /* Push the new tree back */
    }
    /* The last node becomes the root */
    this->root = Q.top(); 
    Q.pop();
}

/* 
Generate parents from two nodes left and right 
*/
Node* generate_parent(Node* left, Node* right) {
    Node* parent = new Node;
    parent->ch = 0;
    parent->f = left->f + right->f;
    parent->left = left;
    parent->right = right;
    return parent;
}


/* 
 * FUNCTIONS TO IMPLEMENT THE HUFFMAN COMPRESSION ALGORITHM 
 **********************************************************
 */
struct bitcode{ 
    uint64_t ch;
    uint64_t bit;
};

void find_encoding(Node* node, uint64_t ch, uint64_t bit, vector<bitcode>& table);
void dispatch(ifstream& in, ofstream& out, vector<bitcode>& table);
void bitpack_dispatch(ofstream& out, bitcode& res, uint64_t& bus, 
                bitcode& pkg, uint64_t& ubit);
void encoding_scheme_output(ofstream& out, vector<bitcode>& table);

void HuffmanTree::encodeFile(ifstream& in, ofstream& out){
    /* From the Huffman Tree, generate a 'symbol table' for the characters
    and its binary encoding 
    Also, the function assumes that the input and output file has been 
    opened in advance */
    vector<bitcode> table(256);
    find_encoding(this->root, 0x0, 0, table);

    /* Reset input file to the beginning*/
    in.clear();
    in.seekg(0, ios::beg);
    /* Output encoded scheme to file - have to do this later */
    encoding_scheme_output(out, table);
    /* Output the encoded file according to the encoding table */
    dispatch(in, out, table);
}

void encoding_scheme_output(ofstream& out, vector<bitcode>& table) {
    int count = 0;
    for (int i = 0; i < 256; i++) {
        if (table[i].bit != 0) {
            count++;
        }
    }
    
    out << count << endl; /* Number of encoded characters */
    cout << count << endl; /* Testing purposes */

    uint64_t* ch = new uint64_t;
    char* in = new char;
    char* bit = new char;

    /* This is INCLUDING pseudo EOF - char 255 */
    /* Output the number of character encodings so the compressor 
    knows how many character encodings to expect */
    for (int i = 0; i < 256; i++) {
        if (table[i].bit != 0) {
            /* Change from outputting the encoding scheme
            line by line, output everything in a stream */
            *in = (char) i;
            *ch = table[i].ch;
            *bit = table[i].bit;
            /* Change from using the << operator to using fwrite 
                Note that the character representation is larger than 8 bits
                therefore, it is safe to represent as a uint64t
                Note also that this is only for the encoding scheme
            */

            /* The coding scheme is
                char repr bitno
                8    64   8
             */
            out.write(in, sizeof(char));
            out.write((char*)ch, sizeof(uint64_t));
            out.write(bit, sizeof(char));

            cout << (unsigned int) i << " " << (unsigned int) table[i].ch 
            << " " <<  (unsigned int) table[i].bit << endl;
        }
    }
    delete ch;
    delete in;
    delete bit;
}

/* Is it not strange to think about how the characters require more than 8 bits to actually 
code for??? */
void find_encoding(Node* node, uint64_t ch, uint64_t bit, 
        vector<bitcode>& table) {
    /* The table refers to the symbol table */
    if (node == NULL) {return;}
    if (node->ch != 0x0) { 
        /* Already at the leaves, update the encoding table */
        int index = node->ch;
        table[index].ch = ch;
        table[index].bit = bit;

        // cout << index << " " << (int) ch << " " << (int) bit << endl;
        /* Note that for the left most leaf, it will be a NULL, however, 
        its bit will not be 0 */
    } else {
        uint64_t left = (ch << 1); // Insert bit 0
        find_encoding(node->left, left, bit + 1, table);

        uint64_t right = (ch << 1) + 1; // Insert bit 1
        find_encoding(node->right, right, bit + 1, table);
    }
}

/*
Functions to read in characters from the input file and output to 
the out file BIG ENDIAN order
Need to encode the EOF 
*/
void outputByteData(ofstream& out, uint64_t bus);
void dispatch(ifstream& in, ofstream& out, vector<bitcode>& table) {
    /* Use a uint64_t bus and ubit to keep track of the bus to dispatch
     and the number of bit used thus far */
    uint64_t bus = 0;
    /* ubit is the number of bits used to represent a certain encoding */
    uint64_t ubit = 0;
    /* data is the holder for the character from file */
    char* data;
    unsigned char ch = 0;
    /* Need to initialize bitcode or else memory error */
    bitcode res;
    res.ch = 0;
    res.bit = 0;

    data = new char;
    while (true){
        in.read(data, sizeof(char));
        ch = *data;
        if (in.eof()) {
            /* Package the EOF bit and then break */
            /* Note that if we reference ch after reading eof, 
            it shows the last character read*/
            bitcode pkg = table[255]; /* Pseudo-EOF */
            bitpack_dispatch(out, res, bus, pkg, ubit);
            break;
        } else {
            if (res.ch != 0x0) {
                /* If there are some reserved bits, pack the reserve into the bus */
                bitpack_dispatch(out, res, bus, res, ubit);
                /* free the reserve code. This can be loose because the reserve adds at most
                7 bits into the 64-bit bus
                We only have reserve when the previous packing nears 64 bits
                */
                res.ch = 0x0; /* Reset the reserve */
                res.bit = 0;
            }
            /* Get the encoding for character ch anyway */
            bitcode pkg = table[(int) ch];
            /* Package the bitcode into the bus */
            bitpack_dispatch(out, res, bus, pkg, ubit);
        }
    }
    /* Last check for reserve bitcode, mostly for the case where by the pseudo eof
    is not packaged fully in*/
    if (res.ch != 0x0) bitpack_dispatch(out, res, bus, res, ubit);
    outputByteData(out, bus); /* Output the last bus that contains EOF */
}

void bitpack_dispatch(ofstream& out, bitcode& res, uint64_t& bus, 
                                    bitcode& pkg, uint64_t& ubit) {

    /* Number of bits to represent, get it as char and later cast it as uint64*/
    uint64_t w = pkg.bit; 
    uint64_t rep = pkg.ch; /* The character encoding to represent */
    uint64_t PACKET_SIZE = 64;

    if (ubit + w < PACKET_SIZE) { /* If the bus still has extra space after packing */
        bus = bitpack_newu(bus, rep, PACKET_SIZE - ubit - w, w);
        ubit += w; 
        /* Do not output the bus because it is not full yet */
    } else { /* Will output one bus and reset the bus */
        if (ubit + w == PACKET_SIZE) {
            /* Just full bus */
            bus = bitpack_newu(bus, rep, 0, w);
        } else {
            /* partially bitpack rep into bus */
            /* Find the range to actually bit pack */
            unsigned int used =  PACKET_SIZE - ubit; /* The number of bits used */
            unsigned int unused = w - used; /* Number of bits unused */
            unsigned char pkg = rep >> unused; /* Get the bits that are used part */
            /* Pack the used bits into the bus */
            bus = bitpack_newu(bus, pkg, 0, used);
            /* update reserve */
            unsigned char reserve = rep << (PACKET_SIZE - unused);
            res.ch = reserve >> (PACKET_SIZE - unused); /* Get the unused part */
            res.bit = unused; /* The number of bits unused, to be used next time */
            // cout << "Bus: " << bus << " pkg: " << (int)pkg << " used: " << used << " w: " << w << endl;
            // cout << "rep: " << (int) rep << " resch: " << (int)res.ch << " resbit: " << (int)res.bit << endl;
        }
        outputByteData(out, bus); /* Output the filled bus, note that the reserve is not empty */
        bus = 0; /* Reset the bus */
        ubit = 0; 
    }
}

void outputByteData(ofstream& out, uint64_t bus) {
    char* ptr = (char*) &bus; /*Assuming BigEndian order */
    int SIZE = (int) sizeof(uint64_t);

    for (int i = SIZE-1; i >= 0; i--) {
        out.write(ptr + i, sizeof(char));
    }
    // out.write(ptr, SIZE);
}

/*
 * FUNCTIONS TO IMPLEMENT THE DECODING ALGORITHM
 * **************************************************************
 */


void insertEncodingScheme(char* code, Node* node);
void insertPseudoEOFScheme(Node* node);

void HuffmanTree::createTreeFromScheme(std::ifstream& in) {
    string line;
    int count = 0;
    char* data;
    int CODE_SIZE = 10; 
    /* Note that encoding schemee is char(1) - repr(8) - bit(1) */

    try{
        getline(in, line);
        cout << line << endl; // Testing purposes
    } catch (std::ios_base::failure) {
        cerr << "The compressed file format is incorrect" << endl;
        exit(1);
    }
    try{
        count = stoi(line, NULL, 10); /* Get the number of encoded chars */
    } catch (const std::out_of_range& oor) {
        cerr << "The compressed file format is incorrect" << endl;
        exit(1);
    }

    postorder_delete(this->root); /* Remove possible existing tree */
    this->root = new Node; /* Initialize the root to a NULL value */
    this->root->left = this->root->right = NULL;
    this->root->f = 0;
    this->root->ch = 0;

    /* Get all the encoding scheme including the EOF */
    data = new char[CODE_SIZE];
    for (int i = 0; i < count; i++) {
        try{
            /* Change from getting the encoding scheme from line by line
            to triplets of char */
            in.read(data, CODE_SIZE);
            insertEncodingScheme(data, this->root);
        } catch (std::ios_base::failure) {
            cerr << "The compressed file format is incorrect" << endl;
        }
    }
    delete[] data;
}

/* Functions to insert a particular encoding into the Huffman Tree
  the node passed into the function is assumed to be the root of the tree
  Expect each code to have exactly 3 bytes
*/
void insertEncodingScheme(char* code, Node* node) {
    /* Note that encoding scheme is char - repr - bit */
    /* Extract the encoded character*/
    unsigned char ch = *code;
    /* Extract the encoding representation */
    uint64_t* encode_ptr = (uint64_t*) (code+1);
    uint64_t encode = *(encode_ptr);
    /* Extract number of bits used to represent the character */
    uint64_t bit = *(code+9);

    /* Testing purposes */
    cout << (uint64_t) ch << " " << (uint64_t) encode << " " << bit << endl;
    Node* cur = node; /* At the root */

    for(int i = bit - 1; i > 0; i--) { /* Go to the 2nd last bit */
        if((encode >> i)&0x1) { /* If 1 go right */
            if (cur->right == NULL) { /* Initialize a new node */
                cur->right = new Node;
                cur->right->ch = 0;
                cur->right->f = 0;
                cur->right->left = cur->right->right = NULL;
                cur = cur->right;
            } else {
                if (cur->right->f != 0) {
                    cerr << "Corrupted encoding scheme" << endl;
                    exit(1);
                }
                cur = cur->right;
            }
        } else { /* If 0 go left */
            if (cur->left == NULL) {
                cur->left = new Node;
                cur->left->ch = 0;
                cur->left->f = 0;
                cur->left->left = cur->left-> right = NULL;
                cur = cur->left;
            } else {
                if (cur->left->f != 0) {
                    cerr << "Corrupted encoding scheme" << endl;
                    exit(1);
                }
                cur = cur->left;
            }
        }
    }
    /* The last bit - node that signals character output */
    if (encode & 0x1) { /* go right */
        if (cur->right != NULL) {
            cerr << "Corrupted encoding scheme" << endl;
            exit(1);
        }
        cur->right = new Node;
        cur->right->left = cur->right->right = NULL;
        cur->right->ch = ch;
        cur->right->f = 1; 
        /* 1 Signal that this is the end of a encoding and output the character*/
    } else {
        if (cur->left != NULL) {
            cerr << "Corrupted encoding scheme" << endl;
            exit(1); 
        }
        cur->left = new Node;
        cur->left->left = cur->left->right = NULL;
        cur->left->ch = ch;
        cur->left->f = 1; /* Denote f as the signal */
    }
}

void HuffmanTree::decodeFile(ifstream& in, ofstream& out) {
    /* Read in byte by byte from the file, keep a variable i on the
     * current number of bit, 
     * Keep a char variable on the current byte being analyzed
     * Keep a Node* cur on the current Node in the Huffman Tree
     */
    char byte;
    Node* cur;
    int i;
    bool end;
    cur = this->root;
    end = false; /* Not eof yet */
    char* data = new char; /* Dynamically allocated memory to store input from encoded file */
    char* output = new char;

    while (in && !end) { /* While reading in byte by byte */
        in.read(data, 1);
        byte = *data;
        for (i = 7; i >= 0; i--) { /* Go through each bit */
            if ((byte >> i) & 0x1) { /* If the bit is 1, go right */
                if (cur->right == NULL) {
                    cerr << "Corrupted compressed file" << endl;
                    exit(1);
                } 
                cur = cur->right;
                if (cur->f != 0) {
                    if (cur->ch == 255) { 
                        /* If reached the pseudo EOF, stop the program */
                        end = true;
                        break;
                    }
                    *output = (unsigned char) (cur->ch);
                    out.write(output,1);
                    cur = this->root;
                    continue;
                }
            } else { /* If 0, go left */
                if (cur->left == NULL) {
                    cerr << "Corrupted compressed file" << endl;
                    exit(1);
                } 
                cur = cur->left;
                if (cur->f != 0) {
                    if (cur->ch == 255) {
                        end = true;
                        break;
                    }
                    *output = (unsigned char)(cur->ch);
                    out.write(output,1);
                    cur = this->root;
                    continue;
                }
            }
        }
    } 
    delete data;
    delete output;
}
