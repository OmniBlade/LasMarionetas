/**
 * @file
 *
 * @brief Interface for service management and event processing.
 *
 * @copyright Las Marionetas is free software: you can redistribute it and/or
 *            modify it under the terms of the GNU General Public License
 *            as published by the Free Software Foundation, either version
 *            2 of the License, or (at your option) any later version.
 *            A full copy of the GNU General Public License can be found in
 *            LICENSE
 */
#pragma once

#include "igzunknown.h"

static constexpr uint32_t GZIID_cIGZFramework = 0x00000065;

class cIGZApp;
class cIGZCOM;
class cIGZExceptionNotification;
class cIGZFrameworkHooks;
class cIGZSystemService;
class cIGUnknownEnumerator;

class cIGZFramework : public cIGZUnknown
{
public:
    enum FrameworkState
    {
        kStatePreFrameWorkInit = 1,
        kStatePreAppInit = 3,
        kStatePostAppInit = 6,
        kStatePreAppShutdown = 10,
        kStatePostAppShutdown = 11,
        kStatePostSystemServiceShutdown = 12
    };

    /**
     * @brief Adds a service to the service registry.
     *
     * This will alllow the service to be accessed from other COM directors, allowing code to be shared between modules.
     * If the service needs to process on active or idle ticks, it should be registered for them specifically.
     *
     * @see AddToTick(cIGZSystemService* service)
     * @see AddToOnIdle(cIGZSystemService* service)
     */
    virtual bool AddSystemService(cIGZSystemService *service) = 0;

    /**
     * @brief Removes a system service from the service registry.
     */
    virtual bool RemoveSystemService(cIGZSystemService *service) = 0;

    /**
     * @brief Gets a pointer to a system service using a given interface.
     * @param srvid The service ID to get the pointer for.
     * @param riid The interface ID to the interface the service will be used as.
     * @param service A pointer to the pointer to store the service in.
     * @return Whether the service was successfully retrieved.
     * @see cIGZUnknown::QueryInterface(uint32_t riid, void** obj)
     */
    virtual bool GetSystemService(uint32_t srvid, uint32_t riid, void **service) = 0;

    /**
     * @brief Uses a cIGZUnknownEnumerator to iterate over system services.
     *
     * @warning Structure of cIGZUnknownEnumerator currently unknown so unuseable.
     */
    virtual bool EnumSystemServices(cIGUnknownEnumerator &enumerator, cIGZUnknown *service, uint32_t unk) = 0;

    /**
     * @brief Subscribes an object to framework event callbacks.
     */
    virtual bool AddHook(cIGZFrameworkHooks *hooks) = 0;

    /**
     * @brief Unsubscribes an object from framework event callbacks.
     */
    virtual bool RemoveHook(cIGZFrameworkHooks *hooks) = 0;

    /**
     * @brief Activates the on-tick event for a system service.
     *
     * @warning Running every tick has performance implications, acting on messages may be a better model.
     */
    virtual bool AddToTick(cIGZSystemService *service) = 0;

    /**
     * @brief Deactivates the on-tick event for a system service.
     */
    virtual bool RemoveFromTick(cIGZSystemService *service) = 0;

    /**
     * @brief Activates the on-idle event for a system service.
     * @see AddToTick(cIGZSystemService* service)
     */
    virtual bool AddToOnIdle(cIGZSystemService *service) = 0;

    /**
     * @brief Deactivates the on-idle event for a system service.
     */
    virtual bool RemoveFromOnIdle(cIGZSystemService *service) = 0;

    /**
     * @brief Get the interval at which the idle tick occurs, in milliseconds.
     */
    virtual int32_t GetOnIdleInterval() = 0;

    /**
     * @brief Sets the idle tick interval in miliseconds.
     */
    virtual bool SetOnIdleInterval(int32_t nInterval) = 0;

    /**
     * @brief Called when the game has an active tick.
     * @param time_elapsed Time since the last tick, in milliseconds.
     */
    virtual bool OnTick(uint32_t time_elapsed) = 0;

    /**
     * @brief Called when the game has an idle tick.
     */
    virtual bool OnIdle() = 0;

    /**
     * @brief Whether or not tick events are enabled.
     */
    virtual bool IsTickEnabled() = 0;

    /**
     * @brief Sets whether tick events are enabled.
     * @param tick Whether to enable tick events or not.
     * @return Pointer to the current object.
     */
    virtual cIGZFramework *ToggleTick(bool tick) = 0;

    /**
     * @brief Prepares the game for shutdown for the given reason
     * @return quit_reason
     */
    virtual int32_t Quit(int32_t quit_reason) = 0;

    /**
     * @brief Immediately terminates the game for the given reason.
     */
    virtual void AbortiveQuit(int32_t quit_reason) = 0;

    /**
     * @return The command line passed to the application.
     */
    virtual char *CommandLine() = 0;

    /**
     * @return Whether the game is in an installation-only mode.
     */
    virtual bool IsInstall() = 0;

    /**
     * @return A pointer to the parent COM.
     */
    virtual cIGZCOM *GetCOMObject() = 0;

    virtual FrameworkState GetState() = 0;

    virtual void *GetDebugStream() = 0;
    virtual int32_t DefaultDebugStream() = 0;
    virtual void *DebugStream() = 0;
    virtual bool SetDebugStream(void *debug_stream) = 0;
    virtual bool SetDebugLevel(int32_t level) = 0;
    virtual int32_t GetDebugLevel() = 0;

    virtual int32_t StdOut() = 0;
    virtual int32_t StdErr() = 0;
    virtual int32_t StdIn() = 0;

    virtual void *GetStream() = 0;
    virtual bool SetStream(int32_t unk, cIGZUnknown *stream) = 0;

    /**
     * @brief Sets an application to associate with the framework.
     */
    virtual bool SetApplication(cIGZApp *app) = 0;

    /**
     * @return The application associated with this framework.
     */
    virtual cIGZApp *const Application() = 0;

    /**
     * @brief Reports a fatal error using the text provided.
     */
    virtual void ReportException(char const *except_text) = 0;

    /**
     * @brief Obtains an exception notification object.
     */
    virtual cIGZExceptionNotification *ExceptionNotificationObj() = 0;
};

/**
 * @brief Obtains the cIGZFramework interface from the application/dll.
 *
 * This function should be implemented in the application code.
 */
cIGZFramework *RZGetFramework();
