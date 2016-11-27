#include <iostream>
#include <fstream>
#include <Hcompressor.h>
#include <Hdecompressor.h>

using namespace std;
void print_correct_usage(char* str);

int main(int argc, char** argv) {
    if (argc != 4) {
        print_correct_usage(argv[0]);
        exit(1);
    }
    ifstream input;
    ofstream output;

    if ((string) argv[1] == "-compress") { /* Compress file */
        input.open(argv[2]); /* Open input file */
        if (!input.is_open()) {
            cerr << "Failure to open file " << argv[2] << endl;
            exit(1);
        }
        /* Open output file */
        output.open(argv[3], ios::out);

        /* Invoke compressor */
        Hcompressor compressor;
        compressor.validateFile(input);
        compressor.compressFile(input, output);

    } else if ((string) argv[1] == "-decompress") { /* Decompress file */
        input.open(argv[2]); /* Open input file */
        if (!input.is_open()) {
            cerr << "Failure to open file " << argv[2] << endl;
            exit(1);
        }
        /* Open output file */
        output.open(argv[3], ios::out);
        Hdecompressor compressor;
        compressor.generateEncodingScheme(input);
        compressor.decompressFile(input, output);

    } else {
        print_correct_usage(argv[0]);
        exit(1);
    }

    input.close();
    output.close();
    exit(0);
}

void print_correct_usage(char* str) {
    cout << "Incorrect command" << endl;
    cout << "Example: " << str << " -[option] inputfile outputfile" << endl; 
}