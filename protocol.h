#pragma once

#include <efi.h>
#include <efilib.h>


#define EFI_CUSTOM_PROTOCOL_GUID                           \
    {                                                      \
        0x0ca4886e, 0x2a4d, 0x4369,                        \
        {                                                  \
            0x97, 0x23, 0x27, 0x8c, 0xf4, 0xf8, 0x3f, 0x9f \
        }                                                  \
    }

typedef struct
{
    UINTN data;
} CUSTOM_PROTOCOL;
