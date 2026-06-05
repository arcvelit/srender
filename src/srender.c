#define SLIB_IMPLEMENTATION
#include "srender.h"

slib_arena sr_global_arena = {0};

SRENDERDEF void sr_canvas_init(
    SR_Canvas* canvas, 
    uint32_t* frame, 
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
    const SR_Canvas* canvas, 
    const uint32_t x, 
    const uint32_t y, 
    const uint32_t height, 
    const uint32_t width
) {
    SR_Canvas view = {0};
    uint32_t* sub_frame = canvas->frame + x + y * canvas->stride;
    sr_canvas_init(&view, sub_frame, width, height, canvas->width);
    return view;
}

SRENDERDEF uint32_t* sr_frame_alloc(const int32_t width, const uint32_t height) {
    return (uint32_t*)malloc(sizeof(uint32_t) * height * width);
}

SRENDERDEF void sr_frame_free(uint32_t* frame) {
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

SRENDERDEF void sr_canvas_fill(SR_Canvas* canvas, const uint32_t color) {
    for (uint32_t y = 0; y < canvas->height; y++) {
        for (uint32_t x = 0; x < canvas->width; x++) {
            uint32_t* dst = &SR_AT_POS(canvas, x, y);
            *dst = sr_color_blend(color, *dst);
        }
    }
}

SRENDERDEF void sr_canvas_draw_rectangle(
    SR_Canvas* canvas, 
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
            uint32_t* dst = &SR_AT_POS(canvas, x+i, y+j);
            *dst = sr_color_blend(color, *dst);
        }
    }
}

SRENDERDEF void sr_canvas_draw_triangle(
    SR_Canvas* canvas, 
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
    SR_Canvas* canvas, 
    const uint32_t* vertices, 
    const uint32_t count, 
    const uint32_t color
) {
    // Takes array of contiguous points { x0, y0, x1, y1, etc... }
    const uint32_t* init = vertices;
    for (uint32_t p = 0; p < count - 1; p++) {
        sr_canvas_draw_line(canvas, *vertices, *(vertices+1), *(vertices+2), *(vertices+3), color);
        vertices += 2;
    }
    sr_canvas_draw_line(canvas, *vertices, *(vertices+1), *(init+0), *(init+1), color);
}

static inline void sr__internal_canvas_draw_line_bresenham_h(
    SR_Canvas* canvas, 
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

static inline void sr__internal_canvas_draw_line_bresenham_v(
    SR_Canvas* canvas, 
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
    SR_Canvas* canvas, 
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
        sr__internal_canvas_draw_line_bresenham_h(canvas, x0, y0, x1, y1, color);
    } else {
        sr__internal_canvas_draw_line_bresenham_v(canvas, x0, y0, x1, y1, color);
    }
}

SRENDERDEF void sr_matrix_alloc(slib_arena* arena, slib_mat_float* mat, const uint32_t rows, const uint32_t cols) {
    const size_t required  = rows * cols * sizeof(float);
    mat->data = slib_arena_alloc_tail(arena, required);
    mat->cols = cols;
    mat->rows = rows;
}

SRENDERDEF void sr_global_arena_reset() {
    slib_arena_reset(&sr_global_arena);
}

// Saving 

SRENDERDEF SR_Bool sr_canvas_save_as_ppm(const SR_Canvas* canvas, const char* path) {
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
