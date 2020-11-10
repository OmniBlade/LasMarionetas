## Creating Patched Binaries.

In order to use the Las Marionetas dll files, you need to create game binaries patched to load the dll files on start.
The actual game binary needed to be patched can be found [here](https://is.gd/8kpgQP) and is a no-cd or no-origin patch for the final stuff pack binary.

Follow the following steps to create patched binaries:

* Unzip patchbinaries.zip into the root folder of the last expansion pack. The root folder is the folder that contains TSBin, CSBin and TSData.
* Double click patchbinaries.bat to apply the patches to the existing binaries.
* If the patch was completely successful, you will have bodyshop.exe and pkginstaller.exe in CSBin and lasmarionetas.exe in TSBin.
* These binaries will load the relevant dll that you can build from the CMake project and will not run without them.
