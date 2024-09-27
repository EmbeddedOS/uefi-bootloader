#pragma once
#include <efi.h>
#include <protocol.h>

/* Public types --------------------------------------------------------------*/

typedef struct
{
    UINTN mm_size;
    EFI_MEMORY_DESCRIPTOR *mm_descriptor;
    UINTN map_key;
    UINTN descriptor_size;
    UINT32 descriptor_version;
} memory_map_t;

/**
 * @brief   - Kernel boot parameters structure. This structure will be shared
 *            with uefi os loader code. The loader will pass this param to
 *            kernel when it's passing the control to kernel.
 */
typedef struct
{
    memory_map_t mm;
    EFI_RUNTIME_SERVICES *runtime_services;
    EFI_GRAPHICS_OUTPUT_PROTOCOL_MODE graphic_out_protocol;
    UINTN custom_protocol_data;
} boot_params_t;

typedef void (*kernel_entry)(boot_params_t *params);

/* Public function prototypes ------------------------------------------------*/