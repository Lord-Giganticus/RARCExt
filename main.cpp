#include "Yaz0.h"
#include <fstream>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        BinaryReader* reader = new BinaryReader(argv[i], EndianSelect::Little);
        u8* buffer = reader->readAllBytes();
        u32 size;
        delete(reader);
        u8* newbuf = Yaz0::Decode(buffer, size);
        reader = new BinaryReader(newbuf, size, EndianSelect::Little);
        Rarc::RARC archive(reader);
        delete(reader);
        for (auto file : archive.Files) {
            file.Save();
        }
    }
}