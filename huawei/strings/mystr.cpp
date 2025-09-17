#include "mystr.hpp"
#include "stdio.h"
#include "error_handling/my_assert.hpp"

int mc_fgetc(FILE *__restrict stream)
{
    assert(stream != NULL);
    int c;
    return fread(&c, 1, 1, stream) ? c : EOF;
}

int mc_getchar()
{
    return mc_fgetc(stdin);
}

int mc_fputc(int ch, FILE *__restrict stream)
{
    (void)(ch); // UNUSED
    assert(stream != NULL);
    return 1;
}

int mc_putchar(int ch)
{
    (void)(ch); // UNUSED
    return 1;
}

char* mc_fgets( char* __restrict str, int count, FILE* __restrict stream )
{
    assert(stream != NULL);
    assert(str != NULL);

    int i = 0;
    int ch = 0;

    while (i < count)
    {
        ch = mc_fgetc(stream);
        if (ch == EOF) break;

        str[i] = (char)ch;
        if (ch == '\n' || ch == '\0')
        {
            if (i < count - 1 && ch == '\n')
            {
                str[i + 1] = '\0';
            }
            break;
        }
        ++i;
    }

    return i ? str : NULL;
}

int mc_puts(const char* str)
{
    assert(str != NULL);

    int n = (int) fwrite(str, mc_strlen(str), 1, stdout);
    fwrite("\n", 1, 1, stdout);
    return n + 1;
}

char* mc_strchr( const char* str, int ch )
{
    assert(str != NULL);
    
    while(*str++ && *str != ch) {} 
    return *--str ? (char*) (size_t) ++str : NULL;
}

size_t mc_strlen( const char* str )
{
    assert(str != NULL);

    char* end = mc_strchr(str, '\0');
    return end ? (size_t) (end - str) : 0;
}

int strcmp ( const char * str1, const char * str2 )
{
    int status = 0;
    while(*str1 == *str2 && *str1)
    {
        status = *str1++ - *str2++;
    }
    return status;
}

char* mc_strcpy( char* dest, const char* src )
{
    assert(dest != NULL);
    assert(src != NULL);

    char* d = dest; 
    while ((*dest++ = *src++)){} 
    return d;
}

char* mc_strncpy( char* dest, const char* src, size_t count )
{
    assert(dest != NULL);
    assert(src != NULL);

    char* d = dest; 
    for ( size_t i = 0; i < count && (*dest++ = *src++); ++i){} 
    return d;
}

char* mc_strcat( char* dest, const char* src )
{
    char* d = dest; 
    dest += mc_strlen(dest); 
    while ((*dest++ = *src++)){}
    return d;
}

char* mc_strncat( char* dest, const char* src, size_t count )
{
    assert(dest != NULL);
    assert(src != NULL);

    char* d = dest; 
    dest += mc_strlen(dest); 

    mc_strncpy(dest, src, count);

    return d;
}


char * mc_strdup( const char *str1 )
{
    assert(str1 != NULL);

    size_t str_len = mc_strlen(str1);

    char* str_copy = (char*) calloc(str_len, sizeof (char));

    return mc_strcpy(str_copy, str1);
}

size_t mc_getline(char **__restrict lineptr, size_t *__restrict n, FILE *__restrict stream)
{
    assert(lineptr != NULL);
    assert(n != NULL);
    assert(stream != NULL);

    if (*lineptr == NULL)
    {
        *lineptr = (char*) calloc(++*n, sizeof(char));
    } 

    if (*lineptr == NULL)
    {
        return 0;
    }

    int c = 0; 
    size_t len = 0;  

    do
    {
        c = mc_fgetc(stream);

        if (c == EOF) 
        {
            break;
        }

        if (len == *n) 
        {
            *lineptr = (char*) realloc(*lineptr, *n = *n * 2);
        }

        (*lineptr)[len++] = (char)c;
    } 
    while (c != '\n');

    if (len == 0)
    {
        *lineptr = NULL;
        return 0;
    }
    return len;
}

char * mc_strtok ( char * str, const char * delimiters ) // NOT CORRECT
{
    assert(str != NULL);
    assert(delimiters != NULL);

    char buf[256] = {};

    while ((buf[(unsigned char)*delimiters] = *delimiters)) 
    {
        delimiters++;
    }

    while (*str) 
    {
        if (buf[(int)(*str++)]) 
        {
            return str - 1;
        }
    }

    return NULL;
}

const char* mc_strstr( const char* haystack, const char* needle ) // NOT CORRECT
{
    assert(haystack != NULL);
    assert(needle != NULL);

    size_t needle_len = mc_strlen(needle), haystack_len = mc_strlen(haystack), i = 0; 

    if (haystack_len < needle_len)
    {
        return NULL;
    }

    long long max_coef = HASH_POLY_BASE, hash = 0, haystack_hash = 0;

    for (i = 1; i < needle_len; i++)
    {
        max_coef = (max_coef * HASH_POLY_BASE) % HASH_MODULE;
    }

    i = 0;
    hash = createPolyHash(needle, needle_len);
    haystack_hash = createPolyHash(haystack, needle_len);

    while (haystack[i + needle_len] && hash != haystack_hash)
    {
        haystack_hash = updatePolyHash(haystack[i++ + needle_len], haystack_hash, max_coef);
    }
    
    return &(haystack[i - 1]);
}

long long createPolyHash(const char* str, size_t len)
{
    assert(str != NULL);

    long long k = HASH_POLY_BASE, hash = 0;

    for (size_t i = 0; i < len; ++i)
    {
        hash = (hash + k * (unsigned char)str[i]) % HASH_MODULE;
        k = (k * HASH_POLY_BASE) % HASH_MODULE;
    }

    return hash;
}

long long updatePolyHash(const char new_c, long long old_hash, long long max_coef)
{
    long long hash = ((old_hash + (unsigned char) new_c - max_coef) * HASH_POLY_BASE - HASH_POLY_BASE) % HASH_MODULE;
    return hash >= 0 ? hash : hash + HASH_MODULE;
}