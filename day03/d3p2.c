#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef BENCH
#include <time.h>
#endif

#define LINE_BUFFER_SIZE   15
#define NUMBER_BUFFER_SIZE 1000

int main() {
  int *numbers, *search, *bit1, *bit0;
  int numbersSize = NUMBER_BUFFER_SIZE;
  int numbersCount = 0, searchCount, countBit1, countBit0;
  int i, j, bitCount;

  int oxygenRating = 0, co2rating = 0;

#ifdef BENCH
  clock_t start = clock();
#endif

  numbers = (int *)malloc(sizeof(int) * numbersSize);

  char line[LINE_BUFFER_SIZE];
  while (fgets(line, LINE_BUFFER_SIZE, stdin)) {
    if (numbersCount >= numbersSize) {
      numbersSize += NUMBER_BUFFER_SIZE;
      numbers = (int *)realloc(numbers, sizeof(int) * numbersSize);
    }
    numbers[numbersCount++] = strtol(line, NULL, 2);
  }
  bitCount = strlen(line) - 1;

  search = (int *)malloc(sizeof(int) * numbersCount);
  bit1 = (int *)malloc(sizeof(int) * numbersCount);
  bit0 = (int *)malloc(sizeof(int) * numbersCount);

  searchCount = numbersCount;
  memcpy(search, numbers, sizeof(int) * searchCount);
  for (i = bitCount - 1; i >= 0; i--) {
    int mask = 1 << i;
    countBit1 = countBit0 = 0;
    for (j = 0; j < searchCount; j++) {
      if (search[j] & mask) {
        bit1[countBit1++] = search[j];
      } else {
        bit0[countBit0++] = search[j];
      }
    }
    if (countBit1 >= countBit0) {
      if (1 == countBit1) {
        oxygenRating = bit1[0];
        break;
      }
      searchCount = countBit1;
      memcpy(search, bit1, sizeof(int) * countBit1);
    } else {
      if (1 == countBit0) {
        oxygenRating = bit0[0];
        break;
      }
      searchCount = countBit0;
      memcpy(search, bit0, sizeof(int) * countBit0);
    }
  }
  free(search);

  searchCount = numbersCount;
  search = numbers;
  for (i = bitCount - 1; i >= 0; i--) {
    int mask = 1 << i;
    countBit1 = countBit0 = 0;
    for (j = 0; j < searchCount; j++) {
      if (search[j] & mask) {
        bit1[countBit1++] = search[j];
      } else {
        bit0[countBit0++] = search[j];
      }
    }
    if (countBit1 < countBit0) {
      if (1 == countBit1) {
        co2rating = bit1[0];
        break;
      }
      searchCount = countBit1;
      memcpy(search, bit1, sizeof(int) * countBit1);
    } else {
      if (1 == countBit0) {
        co2rating = bit0[0];
        break;
      }
      searchCount = countBit0;
      memcpy(search, bit0, sizeof(int) * countBit0);
    }
  }

  free(search);
  free(bit1);
  free(bit0);

#ifdef BENCH
  printf("Elapse: %f\n", ((double) clock() - start) / CLOCKS_PER_SEC);
#endif

  printf("Oxygen rating: %d\nCO2 rating: %d\nResult: %d\n", oxygenRating, co2rating, oxygenRating * co2rating);

  return 0;
}
