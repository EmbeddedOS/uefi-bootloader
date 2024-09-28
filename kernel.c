#include "kernel.h"

void main(boot_params_t *params)
{
    UINT32 *frame_buffer = NULL;
    UINT32 x_res = 0;
    UINT32 y_res = 0;

    /* 1. We get Graphic Output Protocol to change graphic mode in kernel code.
     */
    frame_buffer = (UINT32 *)params->graphic_out_protocol.FrameBufferBase;
    x_res = params->graphic_out_protocol.Info->PixelsPerScanLine;
    y_res = params->graphic_out_protocol.Info->VerticalResolution;

    for (UINT32 y = 0; y < y_res; y++)
    {
        for (UINT32 x = 0; x < x_res; x++)
        {
            frame_buffer[x + y * x_res] = 0xFFCC2222;
        }
    }

        for (UINT32 y = 0; y < y_res/50; y++)
    {
        for (UINT32 x = 0; x < x_res/50; x++)
        {
            frame_buffer[x + y * x_res] = 0xFFCC2222;
        }
    }
}