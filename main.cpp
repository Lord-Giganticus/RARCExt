#include <iostream>

using namespace std;

extern "C" {
    void UnpackRarc(const char*);
    void PackRarc(const char*, const char*);
    bool is_file(const char*);
    bool is_dir(const char*);
    bool exists(const char*);
}

int main(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        if (is_file(argv[i])) {
            UnpackRarc(argv[i]);
        } else if (is_dir(argv[i])) {
            char* name;
            cout << "Enter a name for the archive:\n";
            cin >> name;
            PackRarc(argv[i], name);
        }
    }
}