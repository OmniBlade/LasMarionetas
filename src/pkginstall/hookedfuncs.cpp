#include "hooker.h"

// This global controls which address is used when needed inline in code.
const BinaryType geBinaryMode = BINARY_INSTALLER;

void SetupHooks()
{
    SetupCommonHooks();
}
