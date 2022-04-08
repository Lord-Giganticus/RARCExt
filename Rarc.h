#include "JKRCompression.h"
#include <vector>

using namespace std;

namespace Rarc {
    struct RARCHeader {
        u32 Size;
        u32 HeaderSize;
        u32 DataOffset;
        u32 FileDataSize;
        u32 MRAMSize;
        u32 ARAMSize;
        u32 DVDSize;
        RARCHeader(BinaryReader* reader);
        RARCHeader();
    };

    struct RARCDataHeader {
        u32 DirCount;
        u32 DirOffset;
        u32 TotalNodeCount;
        u32 NodeOffset;
        u32 StringTableSize;
        u32 StringTableOffset;
        u16 NodeCount;
        bool Sync;
        u8* Padding;
        RARCDataHeader(BinaryReader* reader, u32 pos);
        RARCDataHeader();
    };

    enum FileAttribute {
        FILE = 0x01,
        DIRECTORY = 0x02,
        COMPRESSED = 0x04,
        PRELOAD_TO_MRAM = 0x10,
        PRELOAD_TO_ARAM = 0x20,
        LOAD_FROM_DVD = 0x40,
        YAZ0_COMPRESSED = 0x80
    };

    static FileAttribute Default() {
        return (FileAttribute)(FileAttribute::FILE | FileAttribute::PRELOAD_TO_MRAM);
    }

    struct RARCFile {
        string Name;
        bool IsDir();
        u16 ID;
        u16 Hash;
        FileAttribute Flags;
        u32 Size;
        u32 Offset;
        u16 NameOffset;
        u8* Data;
        RARCFile(BinaryReader* reader);
        void Save();
        void Replace(const string& path);
        void Replace(u8* data, u32 size);
        const u8* Compress(u32* size);
        u8* Decompress(u32& size);
    };

    struct RARCDirectory {
        string Name;
        u32 ID;
        u32 NameOffset;
        u16 Hash;
        u16 NodeCount;
        u32 FirstNodeOffset;
        vector<RARCFile> Nodes;
        RARCDirectory(BinaryReader* reader);
    };

    struct RARC {
        RARCHeader Header;
        RARCDataHeader DataHeader;
        vector<RARCDirectory> Directories;
        vector<RARCFile> Files;
        EndianSelect Endian;
        string Name;
        RARC(BinaryReader* reader);
        RARC();
    };
}