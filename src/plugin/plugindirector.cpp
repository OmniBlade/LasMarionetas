#define PLUGIN_DLL 1
#include "igzframework.h"
#include "rzcomdlldirector.h"

#ifdef _WIN32
#include <windows.h>
#define DEMOMSG(x) MessageBoxA(NULL, (x), "PluginDllDirector", MB_OK)

BOOL WINAPI DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
            MessageBoxA(NULL, "Plugin DLL Loaded.", "PluginDllDirector", MB_OK);
            break;
        case DLL_PROCESS_DETACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        default:
            break;
    }

    return TRUE;
}
#else
#define DEMOMSG(x) ((void)0)
#endif

constexpr uint32_t kPluginDLLDirectorID = 0x12E4369E;

class PluginDLLDirector : public cRZCOMDllDirector
{
public:
    PluginDLLDirector()
    {
        // Do stuff here, register class construction function, other directors and such.
        DEMOMSG(__CURRENT_FUNCTION__);
    }

    virtual ~PluginDLLDirector() override {}

    virtual uint32_t GetDirectorID() const override { return kPluginDLLDirectorID; }

    virtual bool OnStart(cIGZCOM *pCOM) override
    {
        DEMOMSG(__CURRENT_FUNCTION__);
        RZGetFramework()->AddHook(static_cast<cIGZFrameworkHooks *>(this));
        return true;
    }

    virtual bool PreFrameWorkInit() override { return true; }
    virtual bool PreAppInit() override
    {
        DEMOMSG(__CURRENT_FUNCTION__);
        return true;
    }
    virtual bool PostAppInit() override
    {
        DEMOMSG(__CURRENT_FUNCTION__);
        return true;
    }
    virtual bool PreAppShutdown() override { return true; }
    virtual bool PostAppShutdown() override { return true; }
    virtual bool PostSystemServiceShutdown() override { return true; }
    virtual bool AbortiveQuit() override { return true; }
    virtual bool OnInstall() override { return true; }

    static cIGZUnknown *CreateSomeObject()
    {
        cIGZUnknown *unknown = nullptr;

        return unknown;
    }
};

cRZCOMDllDirector *RZGetCOMDllDirector()
{
    static PluginDLLDirector sDirector;
    return &sDirector;
}
