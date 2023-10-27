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
    size_t Xoff;
    size_t Yoff;
public:
    DisplayInfo(Pixel* pixels, size_t width, size_t height, size_t pitch, size_t Xoff=0, size_t Yoff=0)
            : pixels(pixels), width(width), height(height), pitch(pitch), Xoff(Xoff), Yoff(Yoff) {}
    inline void setAt(size_t X, size_t Y, Pixel pixel) {
//        Pixel* orgpixels=pixels;
        if(!pixels) return;
        pixels[X+Xoff+((Yoff+Y)*pitch/sizeof(Pixel))]=pixel;
    }
    inline bool isInvalid() const { return pixels==0;}
    inline DisplayInfo slice(size_t Xoff, size_t Yoff, size_t width, size_t height) {
        return DisplayInfo(pixels, width, height, pitch, Xoff, Yoff);
    }
};

