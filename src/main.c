#define SR_DEV_MODE
#include "srender.h"

#include <stdio.h>

#define FRAME_WIDTH  500
#define FRAME_HEIGHT 500

int main(void) {
    
    Canvas canvas = {0};
    uint32_t* frame = frame_alloc(FRAME_WIDTH, FRAME_HEIGHT);
    canvas_init(&canvas, frame, FRAME_WIDTH, FRAME_HEIGHT, FRAME_WIDTH);

    canvas_fill_uniform(&canvas, COLOR_BLACK);
    canvas_fill_rect(&canvas, 50, 50, 200, 100, RGBA(255, 0, 0, 255));
    canvas_save_as_ppm(&canvas, "out.ppm");


    frame_free(frame);

    return 0;
}