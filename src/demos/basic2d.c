#define SR_DEV_MODE
#include "srender.h"

#define FRAME_WIDTH  500
#define FRAME_HEIGHT 500

int main(void) {
    
    SR_Canvas canvas = {0};
    uint32_t* frame = sr_frame_alloc(FRAME_WIDTH, FRAME_HEIGHT);
    sr_canvas_init(&canvas, frame, FRAME_WIDTH, FRAME_HEIGHT, FRAME_WIDTH);

    sr_canvas_fill(&canvas, SR_COLOR_BLUE);
    sr_canvas_draw_rectangle(&canvas, 432, 200, 5, 5, SR_RGBA(255, 0, 0, 255));
    const uint32_t quad[] = {
        244, 200, 
        300, 100,
        432, 200,
        300, 300,
    };
    sr_canvas_outline_poly(&canvas, quad, 4, SR_COLOR_BLACK);

    // Creates a non-owning canvas to draw on
    SR_Canvas view = sr_canvas_view(&canvas, 0, canvas.height/2, canvas.height/2, canvas.width/2);
    sr_canvas_fill(&view, SR_COLOR_GREEN);
    const uint32_t tri[] = {
        100, 100, 
        125, 50,
        150, 125,
    };
    sr_canvas_outline_poly(&view, tri, 3, SR_COLOR_BLACK);

    sr_canvas_save_as_ppm(&canvas, "out.ppm");


    sr_frame_free(frame);

    return 0;
}