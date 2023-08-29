#include "string.h"
#include "memory.h"
size_t stdString::itostr(char* result, size_t maxsize, int a) {
   if(a==0) {result[0]='0'; return 1;}
   bool isNegative=a<0;
   if(isNegative) {
        a = -a;
   }
   int n = a;
   size_t digits = isNegative;
   while (n > 0) {
        digits++;
        n /= 10;
   }
   if (digits > maxsize) 
        digits=maxsize;
   maxsize=digits;
   
   while (a > 0 && digits > 0) {
        result[digits-1] = a%10+'0';
        a /= 10;
        digits--;
   }
   
   if(isNegative) {
        result[digits-1] = '-';
   } 
   
   return maxsize;
}
size_t stdString::itostr(char* result, size_t maxsize, int64_t a) {
   if(a==0) {result[0]='0'; return 1;}
   bool isNegative=a<0;
   if(isNegative) {
        a = -a;
   }
   int64_t n = a;
   size_t digits = isNegative;
   while (n > 0) {
        digits++;
        n /= 10;
   }
   if (digits > maxsize) 
        digits=maxsize;
   maxsize=digits;
   
   while (a > 0 && digits > 0) {
        result[digits-1] = a%10+'0';
        a /= 10;
        digits--;
   }
   
   if(isNegative) {
        result[digits-1] = '-';
   } 
   return maxsize;
}  
char HexDigits[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
#include "terminal.h"
void stdString::itohex(char* result, int64_t maxsize, int a) {
  #if 1
  int64_t digits = 0;
  auto n = a;
  do {
    digits++;
    n/=16;
  } while(n>0);
  if (digits > maxsize) digits=maxsize;
  while (digits > 0) {
    result[digits-1]    = HexDigits[a&0xF];
    if(digits>1) result[digits-2]  = HexDigits[(a&0xF0)>>4];
    digits-=2;
    a>>=8;
  } 
  #else
    if (result == NULL || maxsize == 0) {
        return;
    }

    size_t numDigits = 0;
    int temp = a;
    do {
        temp /= 16;
        numDigits++;
    } while (temp != 0);

    if (numDigits > maxsize - 1) {
        numDigits = maxsize - 1;
    }

    size_t index = numDigits;
    do {
        index--;
        int remainder = a % 16;
        result[index] = (remainder < 10) ? ('0' + remainder) : ('a' + remainder - 10);
        a /= 16;
    } while (a != 0 && index>0);
   #endif
}


void stdString::itohex(char* result, int64_t maxsize, int64_t a) {
#if 1
  int64_t digits = 0;
  auto n = a;
  do {
    digits++;
    n/=16;
  } while(n>0);
  if (digits > maxsize) digits=maxsize;
  while (digits > 0) {
    result[digits-1]    = HexDigits[a&0xF];
    if(digits>1) result[digits-2]  = HexDigits[(a&0xF0)>>4];
    digits-=2;
    a>>=8;
  }
#else
    if (result == NULL || maxsize == 0) {
        return;
    }

    size_t numDigits = 0;
    int64_t temp = a;
    do {
        temp /= 16;
        numDigits++;
    } while (temp != 0);

    if (numDigits > maxsize - 1) {
        numDigits = maxsize - 1;
    }

    size_t index = numDigits;
    do {
        index--;
        int64_t remainder = a % 16;
        result[index] = (remainder < 10) ? ('0' + remainder) : ('a' + remainder - 10);
        a /= 16;
    } while (a != 0 && index>0);
#endif
}
