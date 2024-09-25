#pragma once
#include <efi.h>

/* Public types --------------------------------------------------------------*/

/**
 * @brief   - Kernel boot parameters structure. This structure will be shared
 *            with uefi os loader code. The loader will pass this param to 
 *            kernel when it's passing the control to kernel.
 */
typedef struct 
{
    EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE graphic_out_protocol;
} boot_params_t;

typedef void (*kernel_entry)(boot_params_t *params);

/* Public function prototypes ------------------------------------------------*/
