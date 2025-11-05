// MIT License
// 
// Copyright (c) 2025 Louka Fortin-Sirianni
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.


// Enormous regards to Alexey Kutepov <reximkut@gmail.com>
// for the amazing Olive.c project that made me want to start this one.

#ifndef SRENDER_C
#define SRENDER_C

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

// User defined macros

#ifdef SR_DEV_MODE
    #define SR_STRIP_PREFIX
    #define SR_IMPLEMENTATION
#endif // SR_DEV_MODE

#ifndef SRENDERDEF
    #define SRENDERDEF
#endif // SRENDERDEF

// Miscellaneous

#define UNUSED(var) (void)var

typedef uint8_t SR_Bool;
#define SR_TRUE  1
#define SR_FALSE 0

// Canvas

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

#define SR_CANVAS_PUT(canvas, x, y, color) {                                          \
    typeof(canvas->frame[0])* const _dst = &SR_AT_POS((canvas), (x), (uint32_t)(y));  \
    *_dst = sr_color_blend((color), *_dst);                                           \
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

// Function declarations
SRENDERDEF uint32_t SR_MAX_U32(const uint32_t a, const uint32_t b);
SRENDERDEF uint32_t SR_MIN_U32(const uint32_t a, const uint32_t b);

SRENDERDEF void sr_canvas_init(SR_Canvas* const canvas, uint32_t* const frame, const uint32_t height, const uint32_t width, const uint32_t stride);
SRENDERDEF SR_Canvas sr_canvas_view(const SR_Canvas* const canvas, const uint32_t x, const uint32_t y, const uint32_t height, const uint32_t width);

SRENDERDEF uint32_t* sr_frame_alloc(const int32_t height, const uint32_t width);
SRENDERDEF void sr_frame_free(uint32_t* const frame);

SRENDERDEF uint32_t sr_color_blend(const uint32_t src, const uint32_t dst);
SRENDERDEF void sr_canvas_fill(SR_Canvas* const canvas, const uint32_t color);
SRENDERDEF void sr_canvas_draw_rectangle(SR_Canvas* const canvas, const uint32_t x, const uint32_t y, const uint32_t w, const uint32_t h, const uint32_t color);
SRENDERDEF void sr_canvas_draw_triangle(SR_Canvas* const canvas, const uint32_t x0, const uint32_t y0, const uint32_t x1, const uint32_t y1, const uint32_t x2, const uint32_t y2, const uint32_t color);
SRENDERDEF void sr_canvas_outline_poly(SR_Canvas* const canvas, const uint32_t* vertices, const uint32_t count, const uint32_t color);
SRENDERDEF void sr_canvas_draw_line(SR_Canvas* const canvas, const uint32_t x0, const uint32_t y0, const uint32_t x1, const uint32_t y1, const uint32_t color);

SRENDERDEF SR_Bool sr_canvas_save_as_ppm(const SR_Canvas* const canvas, const char* const path);

// Implementation

#ifdef SR_IMPLEMENTATION

SRENDERDEF uint32_t SR_MAX_U32(const uint32_t a, const uint32_t b) { return a > b ? a : b; }
SRENDERDEF uint32_t SR_MIN_U32(const uint32_t a, const uint32_t b) { return a < b ? a : b; }

SRENDERDEF void sr_canvas_init(
    SR_Canvas* const canvas, 
    uint32_t* const frame, 
    const uint32_t width, 
    const uint32_t height, 
    const uint32_t stride
) {
    canvas->frame  = frame;
    canvas->height = height;
    canvas->width  = width;
    canvas->stride = stride;
}

SRENDERDEF SR_Canvas sr_canvas_view(
    const SR_Canvas* const canvas, 
    const uint32_t x, 
    const uint32_t y, 
    const uint32_t height, 
    const uint32_t width
) {
    SR_Canvas view = {0};
    uint32_t* const sub_frame = canvas->frame + x + y * canvas->stride;
    sr_canvas_init(&view, sub_frame, width, height, canvas->width);
    return view;
}

SRENDERDEF uint32_t* sr_frame_alloc(const int32_t width, const uint32_t height) {
    return (uint32_t*)malloc(sizeof(uint32_t) * height * width);
}

SRENDERDEF void sr_frame_free(uint32_t* const frame) {
    free(frame);
}

// Drawing

SRENDERDEF uint32_t sr_color_blend(const uint32_t src, const uint32_t dst) {
    const uint8_t src_r = SR_GET_R(src);
    const uint8_t src_g = SR_GET_G(src);
    const uint8_t src_b = SR_GET_B(src);
    const uint8_t src_a = SR_GET_A(src);

    const uint8_t dst_r = SR_GET_R(dst);
    const uint8_t dst_g = SR_GET_G(dst);
    const uint8_t dst_b = SR_GET_B(dst);
    const uint8_t dst_a = SR_GET_A(dst);

    const uint8_t out_a = src_a + (dst_a * (255 - src_a)) / 255;   
    if (out_a == 0x00) {
        return 0x00000000;
    }

    const uint8_t out_r = (src_r * src_a + (dst_r * dst_a * (255 - src_a)) / 255) / out_a;
    const uint8_t out_g = (src_g * src_a + (dst_g * dst_a * (255 - src_a)) / 255) / out_a;
    const uint8_t out_b = (src_b * src_a + (dst_b * dst_a * (255 - src_a)) / 255) / out_a;   

    return SR_RGBA(out_r, out_g, out_b, out_a);
}

SRENDERDEF void sr_canvas_fill(SR_Canvas* const canvas, const uint32_t color) {
    for (uint32_t y = 0; y < canvas->height; y++) {
        for (uint32_t x = 0; x < canvas->width; x++) {
            uint32_t* const dst = &SR_AT_POS(canvas, x, y);
            *dst = sr_color_blend(color, *dst);
        }
    }
}

SRENDERDEF void sr_canvas_draw_rectangle(
    SR_Canvas* const canvas, 
    const uint32_t x, 
    const uint32_t y, 
    const uint32_t w, 
    const uint32_t h, 
    const uint32_t color
) {
    const uint32_t max_w = (x + w > canvas->width ) ? (canvas->width - x) : w;
    const uint32_t max_h = (y + h > canvas->height) ? (canvas->height - y) : h;
    for (uint32_t j = 0; j < max_h; j++) {
        for (uint32_t i = 0; i < max_w; i++) {
            uint32_t* const dst = &SR_AT_POS(canvas, x+i, y+j);
            *dst = sr_color_blend(color, *dst);
        }
    }
}

SRENDERDEF void sr_canvas_draw_triangle(
    SR_Canvas* const canvas, 
    const uint32_t x0, const uint32_t y0, 
    const uint32_t x1, const uint32_t y1, 
    const uint32_t x2, const uint32_t y2, 
    const uint32_t color
) {
    sr_canvas_draw_line(canvas, x0, y0, x1, y1, color);
    sr_canvas_draw_line(canvas, x1, y1, x2, y2, color);
    sr_canvas_draw_line(canvas, x2, y2, x0, y0, color);
}

SRENDERDEF void sr_canvas_outline_poly(
    SR_Canvas* const canvas, 
    const uint32_t* vertices, 
    const uint32_t count, 
    const uint32_t color
) {
    const uint32_t* const init = vertices;
    for (uint32_t p = 0; p < count - 1; p++) {
        sr_canvas_draw_line(canvas, *vertices, *(vertices+1), *(vertices+2), *(vertices+3), color);
        vertices += 2;
    }
    sr_canvas_draw_line(canvas, *vertices, *(vertices+1), *(init+0), *(init+1), color);
}

static inline void _sr_canvas_draw_line_bresenham_h(
    SR_Canvas* const canvas, 
    uint32_t x0, 
    uint32_t y0, 
    uint32_t x1, 
    uint32_t y1, 
    const uint32_t color
) {
    // Left to right convention
    if (x0 > x1) {
        SR_SWAP(x0, x1);
        SR_SWAP(y0, y1);
    }

    const uint32_t dx = x1 - x0;
    int32_t        dy = y1 - y0;

    // Miror octants
    const int32_t dir = dy > 0 ? 1 : -1;
    dy *= dir;

    int32_t D = 2*dy - dx;
    for (; x0 <= x1; x0++) {
        SR_CANVAS_PUT(canvas, x0, y0, color);
        if (D > 0) {
            y0 += dir;
            D  -= 2*dx;
        }
        D += 2*dy;
    }
}

static inline void _sr_canvas_draw_line_bresenham_v(
    SR_Canvas* const canvas, 
    uint32_t x0, 
    uint32_t y0, 
    uint32_t x1, 
    uint32_t y1, 
    const uint32_t color
) {
    // Bottom to top convention
    if (y0 > y1) {
        SR_SWAP(x0, x1);
        SR_SWAP(y0, y1);
    }

    int32_t        dx = x1 - x0;
    const uint32_t dy = y1 - y0;
    
    // Miror octants
    const int32_t dir = dx > 0 ? 1 : -1;
    dx *= dir;

    int32_t D = 2*dx - dy;
    for (; y0 <= y1; y0++) {
        SR_CANVAS_PUT(canvas, x0, y0, color);
        if (D > 0) {
            x0 += dir;
            D  -= 2*dy;
        }
        D += 2*dx;
    }
}

SRENDERDEF void sr_canvas_draw_line(
    SR_Canvas* const canvas, 
    const uint32_t x0, 
    const uint32_t y0, 
    const uint32_t x1, 
    const uint32_t y1, 
    const uint32_t color
) {
    const uint32_t dx = (x1 > x0) ? (x1 - x0) : (x0 - x1);
    const uint32_t dy = (y1 > y0) ? (y1 - y0) : (y0 - y1);

    // Branch to variants of Bresenham algorithm on the octant category
    if (dx > dy) {
        _sr_canvas_draw_line_bresenham_h(canvas, x0, y0, x1, y1, color);
    } else {
        _sr_canvas_draw_line_bresenham_v(canvas, x0, y0, x1, y1, color);
    }
}


// TODO: Fill rectangle and add points stuff and matrices and whatnot

// Saving 

SRENDERDEF SR_Bool sr_canvas_save_as_ppm(const SR_Canvas* const canvas, const char* const path) {
    FILE* f = fopen(path, "wb");
    if (!f) {
        return SR_FALSE;
    }

    fprintf(f, "P6\n%d %d\n255\n", canvas->width, canvas->height);
    for (uint32_t y = 0; y < canvas->height; y++) {
        for (uint32_t x = 0; x < canvas->width; x++) {
            const uint32_t col = canvas->frame[y*canvas->stride + x];
            fputc(SR_GET_R(col), f);
            fputc(SR_GET_G(col), f);
            fputc(SR_GET_B(col), f);
        }
    }

    fclose(f);
    return 0;
}
#endif // SR_IMPLEMENTATION

// Prefixes

#ifdef SR_STRIP_PREFIX
    #define Color uint32_t
    #define COLOR_RED   SR_COLOR_RED
    #define COLOR_GREEN SR_COLOR_GREEN
    #define COLOR_BLUE  SR_COLOR_BLUE
    #define COLOR_YELLOW  SR_COLOR_YELLOW
    #define COLOR_CYAN    SR_COLOR_CYAN
    #define COLOR_MAGENTA SR_COLOR_MAGENTA
    #define COLOR_BLACK SR_COLOR_BLACK
    #define COLOR_WHITE SR_COLOR_WHITE
    #define Canvas SR_Canvas
    #define GET_R SR_GET_R
    #define GET_G SR_GET_G
    #define GET_B SR_GET_B
    #define GET_A SR_GET_A
    #define RGBA SR_RGBA
    #define AT_POS SR_AT_POS
    #define CANVAS_PUT SR_CANVAS_PUT
    #define MIN_U32 SR_MIN_U32
    #define MAX_U32 SR_MAX_U32
    #define canvas_init sr_canvas_init
    #define canvas_view sr_canvas_view
    #define color_blend sr_color_blend
    #define canvas_fill sr_canvas_fill
    #define canvas_draw_rectangle sr_canvas_draw_rectangle
    #define canvas_draw_triangle sr_canvas_draw_triangle
    #define canvas_outline_poly sr_canvas_outline_poly
    #define canvas_draw_line sr_canvas_draw_line
    #define canvas_save_as_ppm sr_canvas_save_as_ppm
    #define frame_alloc sr_frame_alloc
    #define frame_free sr_frame_free
    #define Frame SR_Frame
    #define Bool SR_Bool
    #define TRUE SR_TRUE
    #define FALSE SR_FALSE
#endif // SR_STRIP_PREFIX

#endif // SRENDER_C