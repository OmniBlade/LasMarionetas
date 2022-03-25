#include "sha1.h"
#include <fstream>

int wmain(int argc, wchar_t **argv)
{
    SHAEngine sha;
    char buffer[1024];

    std::ifstream in(argv[1], std::ios_base::in | std::ios_base::binary);

    // Get the hash of the binary.
    do {
        in.read(buffer, sizeof(buffer));
        sha.Hash(buffer, int(in.gcount()));
    } while (in.gcount() == sizeof(buffer));

    printf("Hash to use is: \"%s\"", sha.Print_Result());
    return 0;
}
