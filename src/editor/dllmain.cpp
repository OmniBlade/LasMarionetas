
#ifdef _MSC_VER
#include <io.h>
#include <stdio.h>
#include <windows.h>

static constexpr int nBinarySize = 0x007AE000; // Size of game binary

// HANDLE hProcess = GetCurrentProcess();
DWORD OldProtect;

/**
 * @brief Patches exe in memory to replace functions with ours.
 *
 * Defined in hookefuncs.cpp, contains a series of calls to patch a given address with
 * a jump to our own replacement functions.
 */
void SetupHooks();

void StartHooking()
{
    VirtualProtectEx(GetCurrentProcess(), (LPVOID)0x00401000, nBinarySize, PAGE_EXECUTE_READWRITE, &OldProtect);
}

void StopHooking()
{
    DWORD OldProtect2;
    VirtualProtectEx(GetCurrentProcess(), (LPVOID)0x00401000, nBinarySize, OldProtect, &OldProtect2);
}

FARPROC OrignalVariantClear;
FARPROC OrignalVariantInit;
__declspec(naked) void FakeVariantClear()
{
    _asm { jmp[OrignalVariantClear] }
}
__declspec(naked) void FakeVariantInit()
{
    _asm { jmp[OrignalVariantInit] }
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    char path[MAX_PATH];
    HMODULE dll;

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

                printf("\nTS2BodyShop dll injected successfully and console connected.\n");
            }

            /* Forward the D3D functions the game imports so we can MITM it without needing and injector */
            CopyMemory(path + GetSystemDirectoryA(path, MAX_PATH - 10), "\\oleaut32.dll", 11);
            dll = LoadLibraryA(path);
            if (dll == nullptr) {
                MessageBoxA(0, "Cannot load original oleaut32.dll library", "Proxy", MB_ICONERROR);
                ExitProcess(0);
            } else {
                OrignalVariantClear = GetProcAddress(dll, "VariantClear");
                OrignalVariantInit = GetProcAddress(dll, "VariantInit");
            }

            StartHooking();
            SetupHooks();
            break;

        case DLL_PROCESS_DETACH:
            StopHooking();
            break;

        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        default:
            break;
    }

    return TRUE;
}
#endif
