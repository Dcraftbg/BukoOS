#pragma once
#include<cstdint>
#include <stddef.h>
void* memcpy(const void* dst, const void* src, size_t size);
void* memset(const void* dst, int value, size_t size);
uint8_t memcmp(const void* s1, const void* s2, size_t size); 
void* memmove(void* dst, const void* src, size_t size);
