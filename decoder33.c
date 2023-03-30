/////////////////////////
// decode 0x33
/////////////////////////
#include<stdint.h>

void decoder33(uint64_t *in_data){
  union Temp{
    uint8_t dat[8];
    uint64_t dat_64;
  }*temp;
  int i;

  temp = (Temp *)in_data;
  for(i=0;i<8;i++){
    temp->dat[i]-=(uint8_t)0x33U;
  }
}