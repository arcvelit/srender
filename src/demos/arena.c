#define SR_STRIP_PREFIX
#define SR_IMPLEMENTATION

#include "srender.h"

#define FRAME_WIDTH  500
#define FRAME_HEIGHT 500

int main(void) {

    Arena mat_arena = {0};
    arena_init(&mat_arena, 64);
    
    float _a[2][2] = {0};
    Mat A = matrix_make((float*)&_a, 2, 2);
    for (size_t i = 0; i < A.rows*A.cols; i++) A.data[i] = i+1;
    printf("A =\n%.2f  %.2f\n%.2f  %.2f\n", A.data[0], A.data[1], A.data[2], A.data[3]);

    Mat B = {0}; 
    matrix_alloc(&mat_arena, &B, 2, 2);
    // There also exists a global arena called : SR_Global_Arena
    // matrix_alloc(&SR_Global_Arena, &B, 2, 2);
    matrix_copy(&B, &A);
    
    printf("B =\n%.2f  %.2f\n%.2f  %.2f\n", B.data[0], B.data[1], B.data[2], B.data[3]);

    arena_deinit(&mat_arena);
    return 0;
}