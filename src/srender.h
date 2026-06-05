#ifndef SRENDER_C
#define SRENDER_C

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define SLIB_MATRIX_TYPE float
#include <slib/matrix.h>
#include <slib/arena.h>

#ifdef SR_ASSERTS
#include <assert.h>
#endif // SR_ASSERTS

// User defined macros

#ifdef SR_DEV_MODE
    #define SR_ASSERTS
    #define SLIB_ASSERT_MATRIX_MULT
#endif // SR_DEV_MODE

#ifndef SRENDERDEF
    #define SRENDERDEF
#endif // SRENDERDEF

// Miscellaneous

#define SR_UNUSED(var) (void)var

typedef uint8_t SR_Bool;
#define SR_TRUE  1
#define SR_FALSE 0

// Canvas

typedef uint32_t SR_Color;

#define SR_COLOR_RED        0xFF0000FF
#define SR_COLOR_GREEN      0xFF00FF00
#define SR_COLOR_BLUE       0xFFFF0000

#define SR_COLOR_YELLOW     0xFF00FFFF
#define SR_COLOR_CYAN       0xFFFFFF00
#define SR_COLOR_MAGENTA    0xFFFF00FF

#define SR_COLOR_BLACK      0xFF000000
#define SR_COLOR_WHITE      0xFFFFFFFF

#define SR_GET_R(hex) (((hex) & 0x000000FF) >>  0)
#define SR_GET_G(hex) (((hex) & 0x0000FF00) >>  8)
#define SR_GET_B(hex) (((hex) & 0x00FF0000) >> 16)
#define SR_GET_A(hex) (((hex) & 0xFF000000) >> 24)
#define SR_RGBA(r, g, b, a) (((((r)&0xFF)<<0) | (((g)&0xFF)<<8)) | (((b)&0xFF)<<16) | (((a)&0xFF)<<24))

#define SR_AT_POS(canvas, x, y) ((canvas)->frame[(y)*((canvas)->stride) + (x)])

#define SR_CANVAS_PUT(canvas, x, y, color) {                                   \
    typeof(canvas->frame[0])* _dst = &SR_AT_POS((canvas), (x), (uint32_t)(y)); \
    *_dst = sr_color_blend((color), *_dst);                                    \
}

#define SR_SWAP(a, b) {    \
    typeof(a) _temp = (b); \
    (b) = (a);             \
    (a) = _temp;           \
}

typedef struct {
    uint32_t* frame;
    uint32_t width, height;
    uint32_t stride;
} SR_Canvas;

extern slib_arena sr_global_arena;

__attribute__((constructor))
static void sr_init_global_arena(void) {
    slib_arena_init(&sr_global_arena);
}

// Function declarations
static inline uint32_t SR_MAX_U32(const uint32_t a, const uint32_t b) { return a > b ? a : b; }
static inline uint32_t SR_MIN_U32(const uint32_t a, const uint32_t b) { return a < b ? a : b; }

// Canvas
SRENDERDEF void sr_canvas_init(SR_Canvas* canvas, uint32_t* frame, const uint32_t height, const uint32_t width, const uint32_t stride);
SRENDERDEF SR_Canvas sr_canvas_view(const SR_Canvas* canvas, const uint32_t x, const uint32_t y, const uint32_t height, const uint32_t width);

SRENDERDEF uint32_t* sr_frame_alloc(const int32_t height, const uint32_t width);
SRENDERDEF void sr_frame_free(uint32_t* frame);

SRENDERDEF uint32_t sr_color_blend(const uint32_t src, const uint32_t dst);
SRENDERDEF void sr_canvas_fill(SR_Canvas* canvas, const uint32_t color);
SRENDERDEF void sr_canvas_draw_rectangle(SR_Canvas* canvas, const uint32_t x, const uint32_t y, const uint32_t w, const uint32_t h, const uint32_t color);
SRENDERDEF void sr_canvas_draw_triangle(SR_Canvas* canvas, const uint32_t x0, const uint32_t y0, const uint32_t x1, const uint32_t y1, const uint32_t x2, const uint32_t y2, const uint32_t color);
SRENDERDEF void sr_canvas_outline_poly(SR_Canvas* canvas, const uint32_t* vertices, const uint32_t count, const uint32_t color);
SRENDERDEF void sr_canvas_draw_line(SR_Canvas* canvas, const uint32_t x0, const uint32_t y0, const uint32_t x1, const uint32_t y1, const uint32_t color);

// Linear algebra
SRENDERDEF void sr_matrix_alloc(slib_arena* arena, slib_mat_float* mat, uint32_t rows, uint32_t cols);

// Arena
SRENDERDEF void sr_global_arena_reset(void);

// Saving
SRENDERDEF SR_Bool sr_canvas_save_as_ppm(const SR_Canvas* canvas, const char* path);

#endif // SRENDER_C