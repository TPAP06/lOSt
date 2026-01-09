// kernel/lib/string.h - Standard string functions

#ifndef STRING_H
#define STRING_H

#include <stdint.h>
#include <stddef.h>

// Get length of string
size_t strlen(const char *str);

// Compare two strings (returns 0 if equal)
int strcmp(const char *s1, const char *s2);

// Compare n characters of two strings
int strncmp(const char *s1, const char *s2, size_t n);

// Copy string
char *strcpy(char *dest, const char *src);

// Copy n characters
char *strncpy(char *dest, const char *src, size_t n);

// Concatenate strings
char *strcat(char *dest, const char *src);

// Find character in string
char *strchr(const char *str, int c);

// Fill memory with a constant byte
void *memset(void *ptr, int value, size_t num);

// Copy memory
void *memcpy(void *dest, const void *src, size_t n);

// Compare memory
int memcmp(const void *s1, const void *s2, size_t n);

// Convert integer to string
void itoa(int value, char *str, int base);

// Convert string to integer
int atoi(const char *str);

#endif // STRING_H