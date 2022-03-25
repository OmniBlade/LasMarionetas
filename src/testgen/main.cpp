#include "genrandom.h"
#include <stdio.h>
#include <windows.h>

int __stdcall Main_Func(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    FILE *fp = fopen("test_data.txt", "w");
    GenRandom(fp);
    GenSuperRandom(fp);

    return 0;
}
