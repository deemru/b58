#include <stdint.h>
#include <stddef.h>
#include "b58.h"

static const char e58map[] = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

void e58( const uint8_t * bin, size_t binlen, char ** b58, size_t * b58len )
{
    size_t i, z, s, h, l, j;
    char * p;

    for( i = 0; i < binlen; i++ )
        if( bin[i] != 0 )
            break;

    z = i;
    p = *b58;
    s = *b58len;
    h = s - 1;
    l = h;

    for( ; i < binlen; i++ )
    {
        uint32_t c = bin[i];

        for( j = h;; j-- )
        {
            if( j == l )
                if( c )
                    l--;
                else
                    break;
            else
                c += (uint32_t)p[j] * 256;

            p[j] = c % 58;
            c /= 58;
        }
    }

    for( i = 0; i < z; i++ )
        p[( l + 1 - z ) + i] = '1';

    for( i = l + 1; i <= h; i++ )
        p[i] = e58map[((uint8_t *)p)[i]];

    *b58 = p + ( l + 1 - z );
    *b58len = s - ( l + 1 - z );
}

static const uint8_t d58map[] = {
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0xFF, 0x11, 0x12, 0x13, 0x14, 0x15, 0xFF,
    0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0xFF, 0x2C, 0x2D, 0x2E,
    0x2F, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};

#if defined( WORDS_BIGENDIAN ) || !defined( NO_BSWAP32 )

#if !defined( WORDS_BIGENDIAN ) && !defined( BSWAP32 )
#ifdef _WIN32
#include <intrin.h>
#define BSWAP32( v ) _byteswap_ulong( v )
#else
#define BSWAP32( v ) __builtin_bswap32( v )
#endif
#endif

void d58( const char * b58, size_t b58len, uint8_t ** bin, size_t * binlen )
{
    size_t i, z, s, h, l, j;
    uint8_t * p;

    for( i = 0; i < b58len; i++ )
        if( b58[i] != '1' )
            break;

    z = i;
    p = *bin;
    s = *binlen;
    h = s / 4 - 1;
    l = h;

    for( ; i < b58len; i++ )
    {
        uint64_t c = d58map[((uint8_t *)b58)[i]];

        for( j = h;; j-- )
        {
            if( j == l )
                if( c )
                    l--;
                else
                    break;
            else
                c += (uint64_t)((uint32_t *)p)[j] * 58;

            ((uint32_t *)p)[j] = (uint32_t)c;
            c >>= 32;
        }
    }

#ifndef WORDS_BIGENDIAN
    for( i = ( l + 1 ); i <= h; i++ )
        ((uint32_t *)p)[i] = BSWAP32( ((uint32_t *)p)[i] );
#endif

    for( i = ( l + 1 ) * 4; i <= h * 4 + 3; i++ )
        if( p[i] )
            break;

    for( j = ( i - z ); j < ( l + 1 ) * 4; j++ )
        p[j] = 0;

    *bin = p + ( i - z );
    *binlen = s - ( i - z );
}

#else

void d58( const char * b58, size_t b58len, uint8_t ** bin, size_t * binlen )
{
    size_t i, z, s, h, l, j;
    uint8_t * p;

    for( i = 0; i < b58len; i++ )
        if( b58[i] != '1' )
            break;

    z = i;
    p = *bin;
    s = *binlen;
    h = s - 1;
    l = h;

    for( ; i < b58len; i++ )
    {
        uint32_t c = d58map[((uint8_t *)b58)[i]];

        for( j = h;; j-- )
        {
            if( j == l )
                if( c )
                    l--;
                else
                    break;
            else
                c += (uint32_t)p[j] * 58;

            p[j] = c % 256;
            c /= 256;
        }
    }

    for( i = 0; i < z; i++ )
        p[( l + 1 - z ) + i] = 0;

    *bin = p + ( l + 1 - z );
    *binlen = s - ( l + 1 - z );
}

#endif

char c58( const char * b58, size_t b58len )
{
    size_t i;

    for( i = 0; i < b58len; i++ )
        if( b58[i] < '1' || d58map[((uint8_t *)b58)[i]] == d58map[0] )
            return 0;

    return 1;
}
