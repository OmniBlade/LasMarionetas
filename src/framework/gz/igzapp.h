/**
 * @file
 *
 * @brief Interface for the top level application service implementation.
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

static constexpr uint32_t GZIID_cIGZApp = 0x00000066;

class cIGZFramework;
class cIGZSystemService;

class cIGZApp : public cIGZUnknown
{
public:
    /**
     * @return The application instance as an cIGZSystemService interface.
     */
    virtual cIGZSystemService *AsIGZSystemService() = 0;
    /**
     * @param service Pointer to a cIGZSystemService derived object to add as an application service.
     * @return Was the service added successfully?
     */
    virtual bool AddApplicationService(cIGZSystemService *service) = 0;
    /**
     * @return C String containing the name of the module.
     */
    virtual const char *ModuleName() = 0;
    /**
     * @brief Runs any code that needs to run before framework initialisation.
     * @return If the code ran successfully, all known examples unconditionally return true.
     */
    virtual bool PreFrameworkInit() = 0;
    /**
     * @brief Runs any code that needs to run after framework initialisation.
     * @return If the code ran successfully, all known examples unconditionally return true.
     */
    virtual bool PostFrameworkInit() = 0;
    /**
     * @return Always returns false in known examples. Name suggests its the entry point to run the app.
     */
    virtual bool GZRun() = 0;
    /**
     * @brief Runs any code that needs to run before framework shutdown.
     * @return If the code ran successfully, all known examples unconditionally return true.
     */
    virtual bool PreFrameworkShutdown() = 0;
    /**
     * @return The applications framework instance as an cIGZFramework interface.
     */
    virtual cIGZFramework *Framework() = 0;
    /**
     * @brief Called during initialisation, suggested use is to load additional GZCOM dynamic modules.
     */
    virtual void AddDynamicLibrariesHere() = 0;
    /**
     * @brief Called during initialisation, suggested use is to create and install cIGZComDirector objects.
     */
    virtual void AddCOMDirectorsHere() = 0;
    /**
     * @brief Called during initialisation, suggested use is to create and install cIGZSystemService objects.
     */
    virtual void AddApplicationServicesHere() = 0;
    /**
     * @brief Presumably called to load information from the registry based on its name.
     * @return Unconditional true in all known examples.
     */
    virtual bool LoadRegistry() = 0;
};
