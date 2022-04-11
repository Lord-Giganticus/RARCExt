#include "Exports.hpp"
#include "filesystem.hpp"

using namespace std;

void UnpackRarc(const char* path) {
    u32 size;
    u8* buf = JKRCompression::decode(path, &size);
    JKRArchive* archive = !buf ? new JKRArchive(path) : new JKRArchive(buf, size);
    string p = path;
    u32 lastSlashIdx = p.rfind('\\');
    string dir = p.substr(0, lastSlashIdx);
    dir = dir.substr(0, dir.rfind("."));
    archive->unpack(dir);
    delete archive;
    delete buf;
}


void PackRarc(const char* path, const char* name) {
    JKRFileAttr attr = JKRFileAttr_FILE;
    attr = (JKRFileAttr)(attr | JKRFileAttr_LOAD_TO_MRAM);
    string outputPath = name == nullptr ? path : name;
    outputPath.append(".arc");
    auto compType = JKRCompressionType_SZS;
    JKRArchive* archive = new JKRArchive();
    archive->importFromFolder(path, attr);
    archive->save(outputPath, false);
    delete archive;
    JKRCompression::encode(outputPath, compType, false);
}

bool is_file(const char* p) {
    return ghc::filesystem::is_regular_file(p);
}

bool is_dir(const char* p) {
    return ghc::filesystem::is_directory(p);
}

bool exists(const char* p) {
    return ghc::filesystem::exists(p);
}