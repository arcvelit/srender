#define SR_DEV_MODE
#include "srender.h"

#include <stdio.h>

#define FRAME_WIDTH  500
#define FRAME_HEIGHT 500

int main(void) {
    
    Canvas canvas = {0};
    uint32_t* frame = frame_alloc(FRAME_WIDTH, FRAME_HEIGHT);
    canvas_init(&canvas, frame, FRAME_WIDTH, FRAME_HEIGHT, FRAME_WIDTH);

    canvas_fill(&canvas, COLOR_BLUE);
    canvas_draw_rectangle(&canvas, 432, 200, 5, 5, RGBA(255,   0,   0, 255));
    canvas_draw_rectangle(&canvas, 272, 411, 5, 5, RGBA(255,255,255, 255));
    const uint32_t vbo[] = {
        250, 250, 
        300, 100,
        432, 200,
        300, 300,
    };
    sr_canvas_outline_poly(&canvas, vbo, 4, SR_COLOR_BLACK);


    canvas_save_as_ppm(&canvas, "out.ppm");


    frame_free(frame);

    return 0;
}