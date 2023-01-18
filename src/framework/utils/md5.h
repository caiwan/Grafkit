#ifndef __MD5_H_
#define __MD5_H_

#if !defined(__cplusplus) || __cplusplus < 201103L
typedef unsigned int uint32_t ;
typedef unsigned char uint8_t ;
#endif //__cplusplus

#ifndef __cplusplus
extern "C" {
#endif //__cplusplus

	void to_bytes(uint32_t val, uint8_t *bytes);
	uint32_t to_int32(const uint8_t *bytes);
	void md5(const uint8_t *initial_msg, size_t initial_len, uint8_t *digest) ;

#ifndef __cplusplus
}
#endif //__cplusplus

#endif  //__MD5_H_
