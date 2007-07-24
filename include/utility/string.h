// EPOS-- String Utility Declarations

#ifndef __string_h
#define __string_h

int memcmp(const void * s1, const void * s2, unsigned int n);
void * memcpy(void * d, const void * s, unsigned int n);
void * memset(void * d, int c, unsigned int n);
int strcmp(const char * s1, const char * s2);
int strncmp(const char * s1, const char * s2, unsigned int n);
char * strcpy(char * d, const char * s);
char * strncpy(char * d, const char * s, unsigned int n);
unsigned int strlen(const char * s);

/*
inline int memcmp(void * d, const void * s, unsigned int n) {
    return __builtin_memcmp(d, s, n);
}
inline void * memcpy(void * d, const void * s, unsigned int n) {
    return __builtin_memcpy(d, s, n);
}
inline void * memset(void * d, int c, unsigned int n) {
    return __builtin_memset(d, c, n);
}
inline int strcmp(char * d, const char * s) {
    return __builtin_strcmp(d, s);
}
inline int strncmp(char * d, const char * s, unsigned int n) {
    return __builtin_memcmp(d, s, n);
}
inline char * strcpy(char * d, const char * s) {
    return __builtin_strcpy(d, s);
}
inline char * strncpy(char * d, const char * s, unsigned int n) {
    return (char *)__builtin_memcpy(d, s, n);
}
inline unsigned int strlen(const char * s) {
    return __builtin_strlen(s);
}
*/

#endif
