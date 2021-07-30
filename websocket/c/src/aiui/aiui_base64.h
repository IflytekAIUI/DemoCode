#ifndef BASE64__H
#define BASE64__H

/**
 * encode an array of bytes using Base64 (RFC 3548)
 *
 * @param source the source buffer
 * @param sourcelen the length of the source buffer
 * @param target the target buffer
 * @param targetlen the length of the target buffer
 * @return 1 on success, 0 otherwise
 */  
int aiui_base64_encode(unsigned char *source, size_t sourcelen, char *target, size_t targetlen);

/**
 * decode base64 encoded data
 *
 * @param source the encoded data (zero terminated)
 * @param target pointer to the target buffer
 * @param targetlen length of the target buffer
 * @return length of converted data on success, -1 otherwise
 */  
size_t aiui_base64_decode(char *source, unsigned char *target, size_t targetlen);

#endif