#ifndef STRING_H
#define STRING_H

#include <stddef.h>
#include <stdarg.h>

void* memcpy(void* dest, const void* src, size_t n);
void* memset(void* s, int c, size_t n);
char* strncpy(char* dest, const char* src, size_t n);
int strcmp(const char* s1, const char* s2);
size_t strlen(const char* s);
char* strchr(const char* s, int c);
char* strtok(char* str, const char* delim);
int snprintf(char* str, size_t size, const char* format, ...);
void int_to_string(int value, char* str);

#endif // STRING_H
