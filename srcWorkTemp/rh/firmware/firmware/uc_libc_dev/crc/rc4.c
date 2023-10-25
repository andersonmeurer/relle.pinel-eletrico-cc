#include "rc4.h"

void rc4_prepare_key(u8 *key_data_ptr, n16 key_data_len, rc4_key *key) {
  	u8 t;
  	u8 index1;
  	u8 index2;
  	u8* state;
  	n16 counter;

  	state = &key->state[0];
  	for(counter = 0; counter < 256; counter++) state[counter] = counter;
  	key->x = 0;
  	key->y = 0;
  	index1 = 0;
  	index2 = 0;
  	for(counter = 0; counter < 256; counter++)  {
    	index2 = (key_data_ptr[index1] + state[counter] + index2) % 256;
    	swap_byte(&state[counter], &state[index2]);
    	index1 = (index1 + 1) % key_data_len;
  	}
}

void rc4(u8 *buffer_ptr, n16 buffer_len, rc4_key *key) {
  	u8 t;
  	u8 x;
  	u8 y;
  	u8* state;
  	u8 xorIndex;
  	n16 counter;

  	x = key->x;
  	y = key->y;
  	state = &key->state[0];
  	for(counter = 0; counter < buffer_len; counter++) {
    	x = (x + 1) % 256;
    	y = (state[x] + y) % 256;
    	swap_byte(&state[x], &state[y]);
    	xorIndex = (state[x] + state[y]) % 256;
    	buffer_ptr[counter] ^= state[xorIndex];
  	}
  	key->x = x;
  	key->y = y;
}

//Exemplo de uso - sempre chamar prepare_key para preparar a chave e depois o rc4.
/*
#define buf_size 1024
n16 main(n16 argc, char* argv[])
{
  
  
  char seed[256];
  char data[512];
  char buf[buf_size];
  char digit[5];
  n16 hex, rd,i;
  n16 n;
  rc4_key key;

  if (argc < 2)
  {
    fprintf(stderr,"%s key <in >out\r\n",argv[0]);
    exit(1);
  }
  strcpy(data,argv[1]);
  n = strlen(data);
  if (n&1)
  {
    strcat(data,"0");
    n++;
  }
  n/=2;
  strcpy(digit,"AA");
  digit[4]='\0';
  for (i=0;i<n;i++)
  {
    digit[2] = data[i*2];
    digit[3] = data[i*2+1];
    sscanf(digit,"%x",&hex);
    seed[i] = hex;
  }

  prepare_key(seed,n,&key);
  rd = fread(buf,1,buf_size,stdin);
  while (rd>0)
  {
    rc4(buf,rd,&key);
    fwrite(buf,1,rd,stdout);
    rd = fread(buf,1,buf_size,stdin);
  }
}
*/
