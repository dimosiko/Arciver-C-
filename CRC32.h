#ifndef CRC32
#define CRC32

#include <stddef.h>
#include <stdint.h>

uint_least32_t Crc32(const unsigned char *buf, unsigned long long len, uint_least32_t crc);

#endif