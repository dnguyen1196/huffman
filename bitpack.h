#ifndef _BITPACK_H
#define _BITPACK_H

#include <assert.h>
#include <iostream>
using namespace std;

bool bitpack_fitu(uint64_t n, unsigned int width) {
    return ((n >> width) == 0);
}

uint64_t bitpack_newu(uint64_t n, uint64_t i, unsigned int l, unsigned w) {
    assert((l + w) <= 64);
    assert(bitpack_fitu(i, w));

    uint64_t mask = ((n >> l) << (64 - w)) >> (64 - w - l);
    uint64_t repl = i << l;

    return ((n & (~mask)) | repl);    

}

uint64_t bitpack_getu(uint64_t n, unsigned l, unsigned w) {
    assert((l + w) <= 64);
    return (n << (64 - w + l)) >> (64- w);
}

#endif