#pragma once
#include <cstddef>
#include <cstdint>
#include "terminal.h"
namespace stdString {
        size_t itostr(char* result, size_t maxsize, int a);
        size_t itostr(char* result, size_t maxsize, int64_t a); 
        void   itohex(char* result, int64_t maxsize, int32_t a);
        void   itohex(char* result, int64_t maxsize, int64_t a);
};
