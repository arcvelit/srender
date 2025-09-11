#define SR_DEV_MODE
#include "srender.h"

#include <stdio.h>

#define FRAME_HEIGHT 200
#define FRAME_WIDTH  200

int main(void) {
    
    Canvas canvas = {0};
    Color* frame = sr_frame_alloc(FRAME_HEIGHT, FRAME_WIDTH);
    sr_init_canvas(&canvas, frame, FRAME_HEIGHT, FRAME_WIDTH, FRAME_WIDTH);

    sr_canvas_fill_uniform(&canvas, COLOR_RED);
    sr_canvas_save_as_ppm(&canvas, "out.ppm");


    sr_frame_free(frame);

    return 0;
}