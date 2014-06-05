/* 
 * gcc -Os -O2 -g -o aligned -DTEST_ALIGNED bench_alignment.c
 * gcc -Os -O2 -g  -o unaligned  bench_alignment.c
 *
 * tile-gcc -Os -O2  -g  -o tile_aligned -DTILE -DTEST_ALIGNED bench_alignment.c -ltmc
 * tile-gcc -Os -O2 -g  -o tile_unaligned -DTILE bench_alignment.c -ltmc
 *
 * Tile:
 * Aligned Benchmark test for 100000 iters and 32768 arr_size
 * result = 576742016, time = 2
 * # ./tile_unaligned 4321
 * Unaligned Benchmark test for 100000 iters and 32768 arr_size
 * result = 2072483280, time = 18
 * */



#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <time.h>
#include <assert.h>

#ifdef TILE
#include <tmc/alloc.h>
#endif

//#define TEST_ALIGNED 1
#define REPEAT_COUNT 100000
#define ARRAY_SIZE 32768
uint16_t *indexes;
uint32_t *data1, *data2, *data3, *data4;

uint32_t accumulate_non_aligned(uint8_t *data1, uint8_t *data2, uint8_t *data3, uint8_t *data4, uint16_t *indexes, uint32_t size) {
  int  i, j;
  uint32_t result = 0;
  for (i=0; i<REPEAT_COUNT; i++) {
    for(j = 0; j<REPEAT_COUNT; j++){
      result += *((volatile uint32_t *)(data1+(uint32_t)indexes[i]));
      result += *((volatile uint32_t *)(data2+(uint32_t)indexes[i]));
      result += *((volatile uint32_t *)(data3+(uint32_t)indexes[i]));
      result += *((volatile uint32_t *)(data4+(uint32_t)indexes[i]));
    }
  }
  return result;
}

uint32_t accumulate_aligned(uint32_t *data1, uint32_t *data2, uint32_t *data3, uint32_t *data4, uint16_t *indexes, uint32_t size) {
  int  i, j;
  uint32_t result = 0;
  for (i=0; i<REPEAT_COUNT; i++) {
    for(j = 0; j<REPEAT_COUNT/100; j++){
      result += *((volatile uint32_t *)(data1+(uint32_t)indexes[i]));
      result += *((volatile uint32_t *)(data2+(uint32_t)indexes[i]));
      result += *((volatile uint32_t *)(data3+(uint32_t)indexes[i]));
      result += *((volatile uint32_t *)(data4+(uint32_t)indexes[i]));
    }
  }
  return result;
}

int main(int argc, char **argv) {
  int i;
  uint32_t result = 0;
  clock_t start, end;
  void *buf;
  unsigned size = (sizeof(uint16_t) * REPEAT_COUNT)+(sizeof(uint32_t) * ARRAY_SIZE * 4);
  
  if(argc != 2)
  {
    printf("Usage %s: <seed>\n", argv[0]);
    return 0;
  }
  int seed = atoi(argv[1]);
#ifdef TEST_ALIGNED
  printf("Aligned Benchmark test for %d iters and %d arr_size\n", REPEAT_COUNT, ARRAY_SIZE);
#else
  printf("Unaligned Benchmark test for %d iters and %d arr_size\n", REPEAT_COUNT, ARRAY_SIZE);
#endif  

#ifndef TILE
  buf = malloc(size);
#else
  tmc_alloc_t alloc = TMC_ALLOC_INIT;
  tmc_alloc_set_home(&alloc, TMC_ALLOC_HOME_HERE);
  tmc_alloc_set_huge(&alloc);
  
  buf = tmc_alloc_map(&alloc, size);
#endif
  assert(buf);
  indexes = buf;
  buf += sizeof(uint16_t) * REPEAT_COUNT;
  data1 = buf;
  buf += sizeof(uint32_t) * ARRAY_SIZE;
  data2 = buf;
  buf += sizeof(uint32_t) * ARRAY_SIZE;
  data3 = buf;
  buf += sizeof(uint32_t) * ARRAY_SIZE;
  data4 = buf;

  srand(seed);
  for (i=0;i<REPEAT_COUNT;i++) {
#ifdef TEST_ALIGNED
    indexes[i] = rand() % ARRAY_SIZE;
#else
    indexes[i] = rand() % ARRAY_SIZE;
#endif    
  }
  for (i=0;i<ARRAY_SIZE;i++) {
    data1[i] = rand();
    data2[i] = rand();
    data3[i] = rand();
    data4[i] = rand();
  }
  /* Run the test many times and measure time */
  start = clock();
  #ifdef TEST_ALIGNED
  result += accumulate_aligned(data1, data2, data3, data4, indexes, REPEAT_COUNT);
  #else
  result += accumulate_non_aligned((uint8_t *)data1, (uint8_t *)data2, (uint8_t *)data3, (uint8_t *)data4, indexes, REPEAT_COUNT);
  #endif
  end = clock();
  printf("result = %d, time = %d\n", result, ((end-start)/CLOCKS_PER_SEC));
}
