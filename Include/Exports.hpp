#include "JKRArchive.h"

extern "C" {
    void UnpackRarc(const char*);
    void PackRarc(const char*, const char*);
    bool is_file(const char*);
    bool is_dir(const char*);
    bool exists(const char*);
}