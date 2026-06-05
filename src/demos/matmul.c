#define SR_DEV_MODE
#include "srender.h"

#define FRAME_WIDTH  500
#define FRAME_HEIGHT 500

int main(void) {

    SR_Canvas canvas = {0};
    uint32_t* frame = sr_frame_alloc(FRAME_WIDTH, FRAME_HEIGHT);
    sr_canvas_init(&canvas, frame, FRAME_WIDTH, FRAME_HEIGHT, FRAME_WIDTH);

    sr_canvas_fill(&canvas, SR_COLOR_RED);

    float _a[2][2] = {0};
    slib_mat_float A = slib_mat_float_make((float*)&_a, 2, 2);
    for (size_t i = 0; i < A.rows*A.cols; i++) A.data[i] = i+1;
    printf("A =\n%.2f  %.2f\n%.2f  %.2f\n", A.data[0], A.data[1], A.data[2], A.data[3]);

    slib_mat_float B = {0}; 
    sr_matrix_alloc(&sr_global_arena, &B, 2, 2);
    printf("hello, world\n");
    fflush(stdout);
    slib_mat_float_copy(&B, &A);


    printf("B =\n%.2f  %.2f\n%.2f  %.2f\n", B.data[0], B.data[1], B.data[2], B.data[3]);

    sr_canvas_save_as_ppm(&canvas, "out.ppm");

    sr_frame_free(frame);

    return 0;
}