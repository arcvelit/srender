#define SR_STRIP_PREFIX
#define SR_IMPLEMENTATION

#include "srender.h"

#define FRAME_WIDTH  500
#define FRAME_HEIGHT 500

int main(void) {
    
    Canvas canvas = {0};
    uint32_t* const frame = frame_alloc(FRAME_WIDTH, FRAME_HEIGHT);
    canvas_init(&canvas, frame, FRAME_WIDTH, FRAME_HEIGHT, FRAME_WIDTH);

    canvas_fill(&canvas, COLOR_BLACK);

    float _a[2][2] = {0};
    Mat A = matrix_make((float*)&_a, 2, 2);
    for (size_t i = 0; i < A.rows*A.cols; i++) A.data[i] = i+1;
    printf("A =\n%.2f  %.2f\n%.2f  %.2f\n", A.data[0], A.data[1], A.data[2], A.data[3]);

    float _b[2][1] = {0};
    Vec B = matrix_make((float*)&_b, 2, 1);
    for (size_t i = 0; i < B.rows*B.cols; i++) B.data[i] = i+1;
    printf("B =\n%.2f\n%.2f\n", B.data[0], B.data[1]);


    float _r[2][1] = {0};
    Mat result = matrix_make((float*)_r, 2, 1);
    matrix_mult(&result, &A, &B);
    printf("AxB =\n%.2f\n%.2f\n", result.data[0], result.data[1]);

    /* Possible to allocate on the global arena like this
     *  Mat result = {0};
     *  matrix_alloc(&result, 2, 1); 
     *  matrix_mult(&result, &A, &B);
     *  ...
     *  // You can reset the arena cursor 
     *  sr_matrix_arena_reset();
     *  // careful, this erases all previous allocs too, see memory management demos
     *
     */

    canvas_save_as_ppm(&canvas, "out.ppm");

    frame_free(frame);

    return 0;
}