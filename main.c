
#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "ws2812.pio.h"

#define IS_RGBW false
#define NUM_PIXELS 1
#define WS2812_PIN 16

static inline void put_pixel(uint32_t pixel_grb) {
    pio_sm_put_blocking(pio0, 0, pixel_grb << 8u);
}

#include <math.h>
typedef struct RGBColor {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} RGBColor;

RGBColor hsv2rgb(float h, float s, float v) {
	float r, g, b;
	
	int i = floor(h * 6);
	float f = h * 6 - i;
	float p = v * (1 - s);
	float q = v * (1 - f * s);
	float t = v * (1 - (1 - f) * s);
	
	switch (i % 6) {
		case 0: r = v, g = t, b = p; break;
		case 1: r = q, g = v, b = p; break;
		case 2: r = p, g = v, b = t; break;
		case 3: r = p, g = q, b = v; break;
		case 4: r = t, g = p, b = v; break;
		case 5: r = v, g = p, b = q; break;
	}
	
	RGBColor color;
	color.r = r * 255;
	color.g = g * 255;
	color.b = b * 255;
	
	return color;
}

static inline uint32_t urgb_u32(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t) (r) << 8) |
            ((uint32_t) (g) << 16) |
            (uint32_t) (b);
}

int main() {
    stdio_init_all();
    printf("WS2812 Test, using pin %d", WS2812_PIN);

    // todo get free sm
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &ws2812_program);
    ws2812_program_init(pio, 0, offset, WS2812_PIN, 800000, IS_RGBW);

	float h = 0;
    while (1) {
		RGBColor color = hsv2rgb(h, 0.99, 0.3);
		put_pixel(urgb_u32(color.r, color.g, color.b)); sleep_ms(500);
		put_pixel(0); sleep_ms(500);
		h += 0.1;
		if(h >= 1) {
			h = 0;
		}
    }
}
