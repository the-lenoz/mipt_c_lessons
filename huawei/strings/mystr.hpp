#ifndef MYSTR_DECLARED
#define MYSTR_DECLARED

#include <stdlib.h>
#include <stdio.h>

#define HASH_MODULE 998244353
#define HASH_POLY_BASE 257



int mc_fgetc(FILE *__restrict stream);
int mc_getchar();

char* mc_fgets( char* __restrict str, int count, FILE* __restrict stream );
int mc_fputc(int ch, FILE *__restrict stream);
int mc_putchar(int ch);

int mc_puts(const char* str);


char* mc_strchr( const char* str, int ch );
size_t mc_strlen( const char* str );

int strcmp ( const char * str1, const char * str2 );

char* mc_strcpy( char* dest, const char* src );
char* mc_strncpy( char* dest, const char* src, size_t count );
char* mc_strcat( char* dest, const char* src );
char* mc_strncat( char* dest, const char* src, size_t count );
char * mc_strdup( const char *str1 );
size_t mc_getline(char **__restrict lineptr, size_t *__restrict n, FILE *__restrict stream);
char * mc_strtok ( char * str, const char * delimiters );
const char* mc_strstr( const char* haystack, const char* needle );
long long createPolyHash(const char* str, size_t len);
long long updatePolyHash(const char new_c, long long old_hash, long long max_coef);

#endif // MYSTR_DECLARED