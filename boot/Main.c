#include "stdint.h"

void main(void){
  uint32_t* dummpyAddr = (uint32_t*)(1024*1024*100);
  *dummpyAddr = sizeof(long);
}
