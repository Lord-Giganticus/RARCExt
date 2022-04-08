#include "Rarc.h"

namespace Rarc {
    RARCHeader::RARCHeader(BinaryReader* reader) {
    Size = reader->read<u32>();
    HeaderSize = reader->read<u32>();
    DataOffset = reader->read<u32>();
    FileDataSize = reader->read<u32>();
    MRAMSize = reader->read<u32>();
    ARAMSize = reader->read<u32>();
    DVDSize = reader->read<u32>();
    }

    RARCHeader::RARCHeader() {
        
    }

    RARCDataHeader::RARCDataHeader(BinaryReader* reader, u32 pos) {
        DirCount = reader->read<u32>();
        DirOffset = reader->read<u32>() + pos;
        TotalNodeCount = reader->read<u32>();
        NodeOffset = reader->read<u32>() + pos;
        StringTableSize = reader->read<u32>();
        StringTableOffset = reader->read<u32>() + pos;
        NodeCount = reader->read<u16>();
        Sync = reader->read<u8>() == 0x01;
        Padding = reader->readBytes(5, EndianSelect::Little);
    }

    RARCDataHeader::RARCDataHeader() {

    }

    RARCDirectory::RARCDirectory(BinaryReader* reader) {
        ID = reader->read<u32>();
        NameOffset = reader->read<u32>();
        Hash = reader->read<u16>();
        NodeCount = reader->read<u16>();
        FirstNodeOffset = reader->read<u32>();
        Nodes = vector<RARCFile>();
    }

    RARCFile::RARCFile(BinaryReader* reader) {
        ID = reader->read<u16>();
        Hash = reader->read<u16>();
        if (reader->mEndian == EndianSelect::Little) {
            NameOffset = reader->read<u16>();
            reader->skip(1);
            Flags = (FileAttribute)reader->read<u8>();
        } else {
            Flags = (FileAttribute)reader->read<u8>();
            reader->skip(1);
            NameOffset = reader->read<u16>();
        }
        Offset = reader->read<u32>();
        Size = reader->read<u32>();
    }

    bool RARCFile::IsDir() {
        return (((u8)Flags) & 2) >> 1 == 1;
    }

    void RARCFile::Save() {
        BinaryWriter writer(Name, EndianSelect::Little);
        writer.writeBytes(Data, Size);
    }

    void RARCFile::Replace(const string& path) {
        BinaryReader reader(path, EndianSelect::Little);
        u8* buf = reader.readAllBytes();
        u32 size = reader.size();
        Data = buf;
        Size = size;
    }

    void RARCFile::Replace(u8* data, u32 size) {
        Data = data;
        Size = size;
    }

    const u8* RARCFile::Compress(u32* size) {
        return JKRCompression::encodeSZS(Data, Size, size);
    }

    RARC::RARC(BinaryReader* reader) {
        reader->mEndian = EndianSelect::Little;
        auto magic = reader->read<u32>();
        if (magic == 1129464146) {
            reader->mEndian = EndianSelect::Big;
        } else if (magic == 1380012611) {
            reader->mEndian = EndianSelect::Little;
        } else {
            return;
        }
        Endian = reader->mEndian;
        RARCHeader header(reader);
        u32 pos = reader->position();
        RARCDataHeader data(reader, pos);
        Directories = vector<RARCDirectory>();
        reader->seek(data.DirOffset, ios::beg);
        for (int i = 0; i < data.DirCount; i++) {
            Directories.push_back(RARCDirectory(reader));
        }
        for (int i = 0; i < Directories.size(); i++) {
            auto offset = data.StringTableOffset + Directories[i].NameOffset;
            auto fakepos = reader->position();
            reader->seek(offset, ios::beg);
            Directories[i].Name = reader->readNullTerminatedString();
            reader->seek(fakepos, ios::beg);
            for (int n = 0; n < Directories[i].NodeCount; n++) {
                reader->seek(data.NodeOffset + (n + Directories[i].FirstNodeOffset) * 0x14, ios::beg);
                RARCFile entry(reader);
                u32 nameoff = data.StringTableOffset + entry.NameOffset;
                fakepos = reader->position();
                reader->seek(nameoff, ios::beg);
                entry.Name = reader->readNullTerminatedString();
                if (entry.Name == string(".") || entry.Name == string("..")) {
                    continue;
                }
                reader->seek(fakepos, ios::beg);
                reader->seek(pos + header.DataOffset + entry.Offset, ios::beg);
                entry.Data = reader->readBytes(entry.Size, EndianSelect::Little);
                reader->seek(fakepos, ios::beg);
                Files.push_back(entry);
                Directories[i].Nodes.push_back(entry);
            }
        }
        Name = Directories[0].Name;
        Header = header;
        DataHeader = data;
    }

    RARC::RARC() {

    }

}