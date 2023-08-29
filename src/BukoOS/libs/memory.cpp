#include "memory.h"

void* memcpy(const void* dst, const void* src, size_t size) {
    for(size_t i=0; i<size; ++i){
        ((char*)dst)[i]=((char*)src)[i];
    }
    return (void*)dst;
}
void* memset(const void* dst, int value, size_t size) {
    for(size_t i=0; i<size; ++i) {
        ((char*)dst)[i] = (char)value;
    }
    return (void*)dst;
}
uint8_t memcmp(const void* s1, const void* s2, size_t size) {
    for(size_t i=0; i < size; ++i) {
        if(((char*)s1)[i] < ((char*)s2)[i]) return -1;
        if(((char*)s1)[i] > ((char*)s2)[i]) return 1;
    }
    return 0;
}
void* memmove(void* dst, const void* src, size_t size) {
    for(size_t i=0; i < size; ++i) {
        ((char*)dst)[i]=((char*)src)[i];
    }
    return dst;
}
