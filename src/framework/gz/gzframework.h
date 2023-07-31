/**
 * @file
 *
 * @brief Implementation for a default framework root class.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#pragma once
#include "gzcom.h"
#include "igzframework.h"
#include "igzframeworkhooks.h"
#include "igzframeworkw32.h"
#include "igzistream.h"
#include "igzostream.h"
#include "rzautorefcount.h"
#include "rzcmdline.h"
#include <list>
#include <map>

#ifdef MATCH_ABI // STLPort is needed to match at ABI level and uses none standard hash_map.
#include <hash_map>
#else
#include <unordered_map>
#endif

static constexpr uint32_t kGZCLSID_cGZFrameWork = 0x0000003E8;

class cIGZDebugStream;
class cRZExceptionNotification;

class cGZFramework : public cIGZFramework, public cIGZFrameworkW32
{
    class cServiceInfo
    {
    public:
        bool operator<(const cServiceInfo &other) { return priority < other.priority; }
        uint32_t priority;
        uint32_t id;
    };

    typedef std::list<cRZAutoRefCount<cIGZFrameworkHooks>> tHooksList;
    typedef std::list<cRZAutoRefCount<cIGZSystemService>> tServiceList;
#ifdef MATCH_ABI
    typedef std::hash_map<uint32_t,
        cRZAutoRefCount<cIGZSystemService>,
        std::hash<uint32_t>,
        std::equal_to<uint32_t>,
        cRZFastSTLAllocator<std::pair<const uint32_t, cRZAutoRefCount<cIGZSystemService>>>>
        tServiceIdMap;
#else
    typedef std::unordered_map<uint32_t,
        cRZAutoRefCount<cIGZSystemService>,
        std::hash<uint32_t>,
        std::equal_to<uint32_t>,
        cRZFastSTLAllocator<std::pair<const uint32_t, cRZAutoRefCount<cIGZSystemService>>>>
        tServiceIdMap;
#endif
    typedef std::multimap<cGZFramework::cServiceInfo, cRZAutoRefCount<cIGZSystemService>> tServiceInfoMap;

public:
    cGZFramework();
    virtual ~cGZFramework();

    /**
     * @brief Casts the object to the interface specified by riid
     * @param riid The GUID for the interface type to cast to
     * @param obj A pointer to a void pointer to store the result in
     * @return Whether the object could be successfully cast or not
     */
    virtual bool QueryInterface(uint32_t iid, void **obj) override;
    /**
     * @brief Adds a reference to this object
     * @return The new reference count for this object
     */
    virtual uint32_t AddRef() override;
    /**
     * @brief Removes a reference to this object
     * @return The new reference count for this object
     *
     * If the reference count is zero, the object should delete itself and
     * return zero unless it has pretty good reason not to, such as
     * refcounting for the object being handled by some other service.
     */
    virtual uint32_t Release() override;

    /**
     * @brief Adds a service to the service registry.
     *
     * This will alllow the service to be accessed from other COM directors, allowing code to be shared between modules.
     * If the service needs to process on active or idle ticks, it should be registered for them specifically.
     *
     * @see AddToTick(cIGZSystemService* service)
     * @see AddToOnIdle(cIGZSystemService* service)
     */
    virtual bool AddSystemService(cIGZSystemService *service) override;
    /**
     * @brief Removes a system service from the service registry.
     */
    virtual bool RemoveSystemService(cIGZSystemService *service) override;
    /**
     * @brief Gets a pointer to a system service using a given interface.
     * @param srvid The service ID to get the pointer for.
     * @param riid The interface ID to the interface the service will be used as.
     * @param service A pointer to the pointer to store the service in.
     * @return Whether the service was successfully retrieved.
     * @see cIGZUnknown::QueryInterface(uint32_t riid, void** obj)
     */
    virtual bool GetSystemService(uint32_t srvid, uint32_t riid, void **service) override;
    /**
     * @brief Uses a cIGZUnknownEnumerator to iterate over system services.
     *
     * @warning Structure of cIGZUnknownEnumerator currently unknown so unuseable.
     */
    virtual bool EnumSystemServices(cIGUnknownEnumerator &enumerator, cIGZUnknown *service, uint32_t unk) override;
    /**
     * @brief Subscribes an object to framework event callbacks.
     */
    virtual bool AddHook(cIGZFrameworkHooks *hooks) override;
    /**
     * @brief Unsubscribes an object from framework event callbacks.
     */
    virtual bool RemoveHook(cIGZFrameworkHooks *hooks) override;
    /**
     * @brief Activates the on-tick event for a system service.
     *
     * @warning Running every tick has performance implications, acting on messages may be a better model.
     */
    virtual bool AddToTick(cIGZSystemService *service) override;
    /**
     * @brief Deactivates the on-tick event for a system service.
     */
    virtual bool RemoveFromTick(cIGZSystemService *service) override;
    /**
     * @brief Activates the on-idle event for a system service.
     * @see AddToTick(cIGZSystemService* service)
     */
    virtual bool AddToOnIdle(cIGZSystemService *service) override;
    /**
     * @brief Deactivates the on-idle event for a system service.
     */
    virtual bool RemoveFromOnIdle(cIGZSystemService *service) override;
    /**
     * @brief Get the interval at which the idle tick occurs, in milliseconds.
     */
    virtual int32_t GetOnIdleInterval() override;
    /**
     * @brief Sets the idle tick interval in miliseconds.
     */
    virtual bool SetOnIdleInterval(int32_t nInterval) override;
    /**
     * @brief Called when the game has an active tick.
     * @param time_elapsed Time since the last tick, in milliseconds.
     */
    virtual bool OnTick(uint32_t time_elapsed) override;
    /**
     * @brief Called when the game has an idle tick.
     */
    virtual bool OnIdle() override;
    /**
     * @brief Whether or not tick events are enabled.
     */
    virtual bool IsTickEnabled() override;
    /**
     * @brief Sets whether tick events are enabled.
     * @param tick Whether to enable tick events or not.
     * @return Pointer to the current object.
     */
    virtual cIGZFramework *ToggleTick(bool tick) override;
    /**
     * @brief Prepares the game for shutdown for the given reason
     * @return quit_reason
     */
    virtual int32_t Quit(int32_t quit_reason) override;
    /**
     * @brief Immediately terminates the game for the given reason.
     */
    virtual void AbortiveQuit(int32_t quit_reason) override;
    /**
     * @return The command line passed to the application.
     */
    virtual char *CommandLine() override;
    /**
     * @return Whether the game is in an installation-only mode.
     */
    virtual bool IsInstall() override;
    /**
     * @return A pointer to the parent COM.
     */
    virtual cIGZCOM *GetCOMObject() override;

    /**
     * @return Returns a pointer to the class using the cIZGFrameWork derived interface.
     */
    virtual cIGZFramework *AsIGZFrameWork() override { return static_cast<cIGZFramework *>(this); }
    /**
     * @return Returns the windows instance of the application.
     */
    virtual void *GetWindowsInstance() override { return mModuleHandle; }
    /**
     * @return Returns the main window handle of the application.
     */
    virtual void *GetMainHWND() override { return mnHWND; }
    /**
     * @brief Sets the main window handle of the application.
     */
    virtual void SetMainHWND(void *hwnd) override { mnHWND = hwnd; }

private:
    cGZCOM mCOM;
    int32_t field_7C; // TODO Unused.
    uint32_t mnRefCount;
    tHooksList mHooks;
    tServiceInfoMap mInfoMap;
    tServiceIdMap mIDMap;
    tServiceList mOnIdleList;
    tServiceList mOnIdleRemovalList;
    tServiceList mOnTickList;
    tServiceList mOnTickRemovalList;
    bool mbTicksEnabled;
    uint32_t mnOnIdleInterval;
    uint32_t mnTickFrameCount;
    uint32_t mnIdleFrameCount;
    bool mbIsInstall;
    FrameworkState mState;
    bool mbInfoMapLocked;
    bool mbIDMapLocked;
    bool mbIdleListLocked;
    bool mbTickListLocked;
    bool mbHookListLocked;
    uint32_t mnPendingIdleFrames;
    uint32_t mnPendingTickFrames;
    cRZCriticalSection mCriticalSection;
    cRZCmdLine mCommandLine;
    bool mbFlagToQuit;
    cIGZDebugStream *mpDebugStream;
    cRZAutoRefCount<cIGZOStream> mpStdOut;
    cRZAutoRefCount<cIGZOStream> mpStdErr;
    cRZAutoRefCount<cIGZIStream> mpStdIn;
    int32_t mnDebugLevel;
    cRZExceptionNotification *mpExceptionNotification;
    bool mbQuit;
    int32_t mnExitCode;
    void *mModuleHandle; // Holds Windows ModuleHandle
    void *mnHWND; // Holds Windows window handle

    static cIGZApp *mpApp;
    static cGZFramework *mpFramework;
    static cGZFramework *mpSavedFramework;
    static int32_t mnReturnCode;
};
