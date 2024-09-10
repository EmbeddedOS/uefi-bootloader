#pragma once
#include <efi.h>


typedef struct 
{
    EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE graphic_out_protocol;
} boot_params_t;

typedef void (*kernel_entry)(boot_params_t *params);