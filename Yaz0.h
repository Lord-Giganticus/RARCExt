#include "Rarc.h"

namespace Yaz0 {
    u8* Decode(u8* data, u32& size) {
        if (data[0] != 'Y' || data[1] != 'a' || data[2] != 'z' || data[3] != '0') {
            return nullptr;
        }
        auto fullsize = (data[4] << 24) | (data[5] << 16) | (data[6] << 8) | data[7];
        u8* output = new u8[fullsize];
        int inpos = 16, outpos = 0;
        while (outpos < fullsize) {
            u8 block = data[inpos++];
            for (int i = 0; i < 8; i++) {
                if ((block & 0x80) != 0) {
                    output[outpos++] = data[inpos++];
                } else {
                    u8 b1 = data[inpos++];
                    u8 b2 = data[inpos++];
                    int dist = ((b1 & 0xF) << 8) | b2;
                    int copysrc = outpos - (dist + 1);
                    int nbytes = b1 >> 4;
                    if (nbytes == 0) {
                        nbytes = data[inpos++] + 0x12;
                    } else {
                        nbytes += 2;
                    }
                    for (int j = 0; j < nbytes; j++) {
                        output[outpos++] = output[copysrc++];
                    }
                }
                block <<= 1;
                if (outpos >= fullsize) {
                    break;
                }
            }
        }
        size = fullsize;
        return output;
    }
}