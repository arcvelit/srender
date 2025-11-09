#define SR_DEV_MODE
#include "srender.h"

#define FRAME_WIDTH  500
#define FRAME_HEIGHT 500

int main(void) {
    
    Canvas canvas = {0};
    uint32_t* frame = frame_alloc(FRAME_WIDTH, FRAME_HEIGHT);
    canvas_init(&canvas, frame, FRAME_WIDTH, FRAME_HEIGHT, FRAME_WIDTH);

    canvas_fill(&canvas, COLOR_BLACK);

    Vec vector = {0};
    vector_alloc(&vector, 3);
    for (size_t i = 0; i < 3; i++) vector.data[i] = i;

    Mat column = {0};
    matrix_alloc(&column, 3, 1);
    for (size_t i = 0; i < 3; i++) column.data[i] = i;

    Mat result = {0};
    matrix_mult(&result, &vector, &column);
    printf("%.2f\n", *result.data);

    canvas_save_as_ppm(&canvas, "out.ppm");

    frame_free(frame);

    return 0;
}