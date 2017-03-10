#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

/*
 * ChunkTest.c
 *
 *  Created on: Mar 9, 2017
 *      Author: camillenoufi
 */

uint16_t size = 216;
uint16_t chunkDMA[216];
uint16_t chunkTMP[432];

int main() {
    //build packet
   int i;
   for (i=0;i<size;i++)
   {
       chunkDMA[i] = 0x22FF;

	printf("%04x",chunkDMA[i]);
   }

   int j;
   int k = 0;
   
   printf("\n");printf("\n");

  for(j=0;j<size;j++) {
       chunkTMP[k] = chunkDMA[j] & 0x00FF;      //mask out lower 8-bits
       chunkTMP[k+1] = chunkDMA[j] & 0xFF00;    //mask out upper 8-bits
       k = k+2;		
   }
   
   int p;
   for(p=0;p<size*2;p++) {
     printf("%04x", chunkTMP[p]);
     printf("\n");
   }

   return 0;
}
