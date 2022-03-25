#include "bitmapgimex.h"
#include "hooker.h"
#include "jpeggimex.h"
#include "pnggimex.h"
#include "shpgimex.h"
#include "targagimex.h"
#include <gimex.h>

// This global controls which address is used when needed inline in code.
const BinaryType geBinaryMode = BINARY_EDITOR;

void SetupHooks()
{
    SetupCommonHooks();

    // Gimex FSH
    Hook_Function(0x004F29EB, FSH_wopen);
    Hook_Function(0x004F2A68, FSH_write);
    Hook_Function(0x004F2A9B, FSH_wclose);
    Hook_Function(0x004F2AE4, FSH_about);
    Hook_Function(0x004F2BDD, FSH_is);
    Hook_Function(0x004F2C3C, FSH_open);
    Hook_Function(0x004F2CB1, FSH_info);
    Hook_Function(0x004F3914, FSH_read);
    Hook_Function(0x004F394A, FSH_close);

    // Gimex JPG
    Hook_Function(0x004EF8DD, JPG_wopen);
    Hook_Function(0x004F203A, JPG_write);
    // Hook_Function(0x00762DCC, JPG_wclose);
    Hook_Function(0x004F248F, JPG_about);
    Hook_Function(0x004F2563, JPG_is);
    Hook_Function(0x004F25C2, JPG_open);
    Hook_Function(0x004F2608, JPG_info);
    Hook_Function(0x004F2866, JPG_read);
    Hook_Function(0x004F227A, JPG_close); // Also hooks JPG_wclose and PNG_wclose

    // Gimex IJL
    Hook_Function(0x004F159C, IJL_wopen);
    Hook_Function(0x004F15CE, IJL_write);
    // Hook_Function(0x0075ED21, IJL_wclose);
    Hook_Function(0x004F18D5, IJL_about);
    Hook_Function(0x004F19DF, IJL_is);
    Hook_Function(0x004F1A47, IJL_open);
    Hook_Function(0x004F1AE4, IJL_info);
    Hook_Function(0x004F1C8C, IJL_read);
    Hook_Function(0x004F1EAE, IJL_close);

    // Gimex PNG
    // Hook_Function(0x004EF8DD, PNG_wopen);
    Hook_Function(0x004F0BC5, PNG_write);
    // Hook_Function(0x004F227A, PNG_wclose);
    Hook_Function(0x004F0FEF, PNG_about);
    Hook_Function(0x004F10A5, PNG_is);
    Hook_Function(0x004F10F1, PNG_open);
    Hook_Function(0x004F11EC, PNG_info);
    Hook_Function(0x004F146E, PNG_read);
    Hook_Function(0x004F1569, PNG_close);

    // Gimex Bitmap
    // Hook_Function(0x004EF8DD, BMP_wopen);
    Hook_Function(0x004EF912, BMP_write);
    // Hook_Function(0x004F17EE, BMP_wclose);
    Hook_Function(0x004F02E4, BMP_about);
    Hook_Function(0x004F03A1, BMP_is);
    Hook_Function(0x004F03DA, BMP_open);
    Hook_Function(0x004F0420, BMP_info);
    Hook_Function(0x004F07C7, BMP_read);
    Hook_Function(0x004F17EE, BMP_close);

    // Gimex TGA
    // Hook_Function(0x004EF8DD, TGA_wopen);
    Hook_Function(0x004EE692, TGA_write);
    // Hook_Function(0x004F17EE, TGA_wclose);
    Hook_Function(0x004EEE7F, TGA_about);
    Hook_Function(0x004EEF63, TGA_is);
    Hook_Function(0x004EF074, TGA_open);
    Hook_Function(0x004EF0BA, TGA_info);
    Hook_Function(0x004EF4D5, TGA_read);
    // Hook_Function(0x004F17EE, TGA_close);

    // Gimex
    Hook_Function(0x004EDFD8, GIMEX_about);
    Hook_Function(0x004EDFE7, GIMEX_is);
    Hook_Function(0x004EDFF6, GIMEX_open);
    Hook_Function(0x004EE005, GIMEX_wopen);
    Hook_Function(0x004EE014, GIMEX_info);
    Hook_Function(0x004EE023, GIMEX_read);
    Hook_Function(0x004EE04B, GIMEX_close);
    Hook_Function(0x004EE067, GIMEX_wclose);
    Hook_Function(0x004EE083, GIMEX_write);
    Hook_Function(0x004EE0AB, GIMEX_get);
    Hook_Function(0x004EE0CA, GIMEX_set);
    Hook_Function(0x004EE0E6, GIMEX_find);
    Hook_Function(0x004EE1C5, galloc);
    Hook_Function(0x004EE1D2, gfree);
    Hook_Function(0x004EE1E1, gread);
    Hook_Function(0x004EE208, gwrite);
    // Hook_Function(0x004EE22F, gseek);
    // Hook_Function(0x004EE248, glen);
}
