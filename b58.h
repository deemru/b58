#ifdef __cplusplus
extern "C" {
#endif

    void e58( const uint8_t * bin, size_t binlen, char ** b58, size_t * b58len );
    void d58( const char * b58, size_t b58len, uint8_t ** bin, size_t * binlen );
    char c58( const char * b58, size_t b58len );

#ifdef __cplusplus
}
#endif
