#define SR_STRIP_PREFIX
#define SR_IMPLEMENTATION

#include "srender.h"

#define FRAME_WIDTH  500
#define FRAME_HEIGHT 500

int main(void) {
    
    Canvas canvas = {0};
    uint32_t* const frame = frame_alloc(FRAME_WIDTH, FRAME_HEIGHT);
    canvas_init(&canvas, frame, FRAME_WIDTH, FRAME_HEIGHT, FRAME_WIDTH);

    canvas_fill(&canvas, COLOR_BLUE);
    canvas_draw_rectangle(&canvas, 432, 200, 5, 5, RGBA(255, 0, 0, 255));
    const uint32_t quad[] = {
        244, 200, 
        300, 100,
        432, 200,
        300, 300,
    };
    canvas_outline_poly(&canvas, quad, 4, COLOR_BLACK);

    // Creates a non-owning canvas to draw on
    Canvas view = canvas_view(&canvas, 0, canvas.height/2, canvas.height/2, canvas.width/2);
    canvas_fill(&view, COLOR_GREEN);
    const uint32_t tri[] = {
        100, 100, 
        125, 50,
        150, 125,
    };
    canvas_outline_poly(&view, tri, 3, COLOR_BLACK);

    canvas_save_as_ppm(&canvas, "out.ppm");


    frame_free(frame);

    return 0;
}