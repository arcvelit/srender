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
// The following code is heavily inspired by his work, as it can be seen
// not just with the STRIP_PREFIX and such tricks, but with the overall 
// structure. Though I did incorporate my own tricks, my C developement
// style mostly comes from watching him. So thanks zozin <3.

// This is an attempt at making a very simple software renderer and using 
// my knowledge from my Computer Graphics course at Concordia University 
// (COMP371) under the supervision of Mr Popa. 

// PS: I am initially developing this while traveling Japan!


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

// Utils 

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

typedef struct {
    uint32_t* frame;
    uint32_t width, height;
    uint32_t stride;
} SR_Canvas;

// Function declarations
SRENDERDEF void sr_canvas_init(SR_Canvas* const canvas, uint32_t* const frame, const uint32_t height, const uint32_t width, const uint32_t stride);

SRENDERDEF uint32_t* sr_frame_alloc(const int32_t height, const uint32_t width);
SRENDERDEF void sr_frame_free(uint32_t* const frame);

SRENDERDEF uint32_t sr_color_blend(const uint32_t src, const uint32_t dst);
SRENDERDEF void sr_canvas_fill_uniform(SR_Canvas* const canvas, const uint32_t color);
SRENDERDEF void sr_canvas_fill_rect(SR_Canvas* const canvas, const uint32_t x, const uint32_t y, const uint32_t w, const uint32_t h, const uint32_t color);

SRENDERDEF SR_Bool sr_canvas_save_as_ppm(const SR_Canvas* const canvas, const char* const path);

// Implementation

#ifdef SR_IMPLEMENTATION

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

SRENDERDEF uint32_t* sr_frame_alloc(const int32_t width, const uint32_t height) {
    return (uint32_t*)malloc(sizeof(uint32_t)*height*width);
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

SRENDERDEF void sr_canvas_fill_uniform(SR_Canvas* const canvas, const uint32_t color) {
    for (uint32_t y = 0; y < canvas->height; y++) {
        for (uint32_t x = 0; x < canvas->width; x++) {
            uint32_t* const dst = &SR_AT_POS(canvas, x, y);
            *dst = sr_color_blend(color, *dst);
        }
    }
}

SRENDERDEF void sr_canvas_fill_rect(SR_Canvas* const canvas, const uint32_t x, const uint32_t y, const uint32_t w, const uint32_t h, const uint32_t color) {
    for (uint32_t j = 0; j < h; j++) {
        for (uint32_t i = 0; i < w; i++) {
            if (x+i < canvas->width && y+j < canvas->height) {
                uint32_t* const dst = &SR_AT_POS(canvas, x+i, y+j);
                *dst = sr_color_blend(color, *dst);
            }
        }
    }
}

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
    #define canvas_init sr_canvas_init
    #define color_blend sr_color_blend
    #define canvas_fill_uniform sr_canvas_fill_uniform
    #define canvas_fill_rect sr_canvas_fill_rect
    #define canvas_save_as_ppm sr_canvas_save_as_ppm
    #define frame_alloc sr_frame_alloc
    #define frame_free sr_frame_free
    #define Frame SR_Frame
    #define Bool SR_Bool
    #define TRUE SR_TRUE
    #define FALSE SR_FALSE
#endif // SR_STRIP_PREFIX

#endif // SRENDER_C