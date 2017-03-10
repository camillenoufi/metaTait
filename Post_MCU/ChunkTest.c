/*
 * ChunkTest.c
 *
 *  Created on: Mar 9, 2017
 *      Author: camillenoufi
 */


uint16_t chunkDMA[1584];
uint16_t chunkTMP[3168];

void test() {
    //build packet
   int i;
   for (i=0;i<1584;i++)
   {
       chunkDMA[i] = 0x22FF;

   }

   int j;
   int k = 0;

   for(j=0;j<1584;j++) {
       chunkTMP[k] = chunkDMA[i] & 0x0011;      //mask out lower 8-bits
       chunkTMP[k+1] = chunkDMA[i] & 0x1100;    //mask out upper 8-bits
       k = k+2;
   }


}
