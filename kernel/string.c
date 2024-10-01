#include "string.h"

void* memcpy(void* dest, const void* src, size_t n) {
    char* d = dest;
    const char* s = src;
    while (n--) {
        *d++ = *s++;
    }
    return dest;
}

void* memset(void* s, int c, size_t n) {
    unsigned char* p = s;
    while (n--) {
        *p++ = (unsigned char)c;
    }
    return s;
}

char* strncpy(char* dest, const char* src, size_t n) {
    size_t i;
    for (i = 0; i < n && src[i] != '\0'; i++) {
        dest[i] = src[i];
    }
    for (; i < n; i++) {
        dest[i] = '\0';
    }
    return dest;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

size_t strlen(const char* s) {
    size_t len = 0;
    while (s[len]) {
        len++;
    }
    return len;
}

char* strchr(const char* s, int c) {
    while (*s != (char)c) {
        if (!*s++) {
            return 0;
        }
    }
    return (char*)s;
}

char* strtok(char* str, const char* delim) {
    static char* last;
    if (str) last = str;
    if (!last) return 0;
    
    char* tok = last;
    while (*last && !strchr(delim, *last)) last++;
    if (*last) {
        *last = '\0';
        last++;
    } else {
        last = 0;
    }
    return tok;
}

int snprintf(char* str, size_t size, const char* format, ...) {
    // This is a very basic implementation that only supports %s and %d
    // You may want to expand this for more format specifiers
    va_list args;
    va_start(args, format);
    
    size_t written = 0;
    while (*format != '\0' && written < size - 1) {
        if (*format == '%') {
            format++;
            if (*format == 's') {
                char* s = va_arg(args, char*);
                while (*s != '\0' && written < size - 1) {
                    str[written++] = *s++;
                }
            } else if (*format == 'd') {
                int d = va_arg(args, int);
                char num[20];
                int_to_string(d, num);
                char* s = num;
                while (*s != '\0' && written < size - 1) {
                    str[written++] = *s++;
                }
            }
        } else {
            str[written++] = *format;
        }
        format++;
    }
    str[written] = '\0';
    
    va_end(args);
    return written;
}

void int_to_string(int value, char* str) {
    if (value == 0) {
        str[0] = '0';
        str[1] = '\0';
        return;
    }
    
    int i = 0;
    int is_negative = 0;
    if (value < 0) {
        is_negative = 1;
        value = -value;
    }
    
    while (value != 0) {
        str[i++] = (value % 10) + '0';
        value /= 10;
    }
    
    if (is_negative) {
        str[i++] = '-';
    }
    str[i] = '\0';
    
    // Reverse the string
    int start = 0;
    int end = i - 1;
    while (start < end) {
        char temp = str[start];
        str[start] = str[end];
        str[end] = temp;
        start++;
        end--;
    }
}
