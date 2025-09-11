#define SR_DEV_MODE
#include "srender.h"

#include <stdio.h>

#define FRAME_HEIGHT 500
#define FRAME_WIDTH  500

int main(void) {
    
    Canvas canvas = {0};
    uint32_t* frame = frame_alloc(FRAME_HEIGHT, FRAME_WIDTH);
    canvas_init(&canvas, frame, FRAME_HEIGHT, FRAME_WIDTH, FRAME_WIDTH);

    canvas_fill_uniform(&canvas, COLOR_BLUE);
    canvas_save_as_ppm(&canvas, "out.ppm");


    frame_free(frame);

    return 0;
}