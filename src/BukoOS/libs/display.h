#pragma once
#include <cstddef>
#include <cstdint>
struct Pixel {
        uint8_t b;   // blue
        uint8_t g;   // green
        uint8_t r;
        uint8_t a;
        Pixel(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) {}
};
class DisplayInfo {
public:
    Pixel* pixels;
    size_t width;
    size_t height;
    size_t pitch;
public:
    DisplayInfo(Pixel* pixels, size_t width, size_t height, size_t pitch)
            : pixels(pixels), width(width), height(height), pitch(pitch) {}
    void setAt(size_t X, size_t Y, Pixel pixel) {
//        Pixel* orgpixels=pixels;
        pixels[X+(Y*pitch/sizeof(Pixel))]=pixel;
    }
};

