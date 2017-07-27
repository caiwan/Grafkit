#ifndef __MD5_H_
#define __MD5_H_

extern "C" void md5(const uint8_t *initial_msg, size_t initial_len, uint8_t *digest) ;
extern "C" void to_bytes(uint32_t val, uint8_t *bytes);
extern "C" uint32_t to_int32(const uint8_t *bytes);

#endif  //__MD5_H_