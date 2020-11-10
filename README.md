**Las Marionetas** is an open source re-implementation of the second installment of a popular people
simulation game created by Maxis.

This project is a bottom-up rewrite using the original binary to provide functions that have not been implemented yet.
The intention is to allow the fixing of bugs, implementation of new features and porting to platforms unsupported by the original.

Please note that the Las Marionetas developers have no affiliation with EA. EA has not endorsed and does not support this product.

## Chat

You can chat and discuss the development of Las Marionetas on our [Discord channel](https://discord.gg/UnWK2Tw).

## Running Las Marionetas

Currently Las Marionetas can only be run on Windows.

To run Las Marionetas, you first need to have The Sims 2: Ultimate Edition installed. The game was only available via origin and is unfortunately no longer available for purchase.
Some sites dealing with abandonware offer downloads such as [here](https://www.games4theworld.club/t24221-must-read-overview-topic-for-the-sims-2#) but they are not authorised to do so and may be violating copyright.
Use your own judgement if you don't already have the game on Origin.

### Disclaimer

Las Marionetas is in heavy development and as such, development builds may not behave correctly compared to the original game. When we create numbered releases we will try to ensure that are as free from bugs as possible but even they may have some unexpected behaviour.

### Anti-virus warning

Anti-virus software like Windows Defender could mark Las Marionetas as a virus. This is a false-positive. We can assure you that Las Marionetas is fully safe to use. If you are still unsure about this, you can just [build Las Marionetas yourself](#building-Las Marionetas-yourself).

## Building Las Marionetas yourself

In order to generate a working binary, currently you must build the project using
a version of the Microsoft Visual Studio compiler (2017 and above). Building with Clang against a Windows SDK is in theory
possible but is untested for a cross compile solution.

It is also possible to build a standalone binary if you want to test compiling on other platforms.

### Linux and macOS

Native support for Linux and macOS is planned for the future, but because of how
the project is developed, a fully playable native binary will not be possible for some time.
In the mean time, using Wine on Linux and macOS, should be possible but
is currently untested.

If you are interested in developing the cross platform aspect of the project
it is now possible to build the project standalone by passing ```-DSTANDALONE=TRUE``` to CMake when configuring the project
but the resulting binary currently has very limited functionality.

## Contributing

If you are interested in contributing to Las Marionetas, you will need some knowledge of C++
as a minimum requirement. Join the developer chat listed above for more information on
what else you will need, such as a current assembly map of the binary.

## Licence

The source code provided in this repository for
Las Marionetas is licenced under the [GNU General Public License version 2](https://www.gnu.org/licenses/old-licenses/gpl-2.0.html)
or later with an exception allowing the resulting code to be linked against a closed source
binary which will be in place until Las Marionetas no longer relies on the original.

## Other Open-Game Projects

Below is a list of similar projects and their respective original games:

 * [OpenRCT2](https://github.com/OpenRCT2/OpenRCT2) - RollerCoaster Tycoon 2
 * [OpenTTD](https://www.openttd.org) - Transport Tycoon Deluxe
 * [OpenMC2](https://github.com/LRFLEW/OpenMC2) - Midnight Club 2
 * [OpenDUNE](https://github.com/OpenDUNE/OpenDUNE) - Dune 2
 * [OpenFodder](https://github.com/OpenFodder/openfodder) - Cannon Fodder
 * [Devilution](https://github.com/diasurgical/devilution) - Diablo
 * [Thyme](https://github.com/TheAssemblyArmada/Thyme) - Command & Conquer Generals: Zero Hour
 * [Alive](https://github.com/AliveTeam/alive_reversing) - Abe's Exodus
 * [Dethrace](https://github.com/jeff-1amstudios/dethrace) - Carmageddon

There is also a [Wikipedia page for open source games](https://en.wikipedia.org/wiki/List_of_open-source_video_games).
