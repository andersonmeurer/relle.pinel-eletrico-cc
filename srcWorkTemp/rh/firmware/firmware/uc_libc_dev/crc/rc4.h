#ifndef __RC4_H
#define __RC4_H

#include "_config_cpu_.h"

typedef struct rc4_key {      
   u8 state[256];       
   u8 x;        
   u8 y;
} rc4_key;

#define swap_byte(x,y) t = *(x); *(x) = *(y); *(y) = t

void rc4_prepare_key(u8 *key_data_ptr, n16 key_data_len, rc4_key *key);
void rc4(u8 *buffer_ptr, n16 buffer_len, rc4_key *key);

#endif
