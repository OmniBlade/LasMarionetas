
#ifdef _MSC_VER
#include <io.h>
#include <stdio.h>
#include <windows.h>

BOOL WINAPI DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            if (AttachConsole(ATTACH_PARENT_PROCESS)) {
                /* We attached successfully, lets redirect IO to the consoles handles if not already redirected */
                if (_fileno(stdout) == -2 || _get_osfhandle(_fileno(stdout)) == -2) {
                    freopen("CONOUT$", "w", stdout);
                }

                if (_fileno(stderr) == -2 || _get_osfhandle(_fileno(stderr)) == -2) {
                    freopen("CONOUT$", "w", stderr);
                }

                if (_fileno(stdin) == -2 || _get_osfhandle(_fileno(stdin)) == -2) {
                    freopen("CONIN$", "r", stdin);
                }

                printf("\Test Data Generator dll injected successfully and console connected.\n");
            }
            break;

        case DLL_PROCESS_DETACH:
            break;

        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        default:
            break;
    }

    return TRUE;
}
#endif
