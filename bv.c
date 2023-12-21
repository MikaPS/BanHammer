#include "bv.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Defines what members/fields the BitVector structure has
// length is the size or the number of bits
// vector will hold the states for each individual bit in the structure
typedef struct BitVector BitVector;

struct BitVector {
  uint32_t length;
  uint64_t *vector;
};

// The constructor for the BitVector. Creates a new BitVector and returns a
// pointer to it if the memory was allocated succesfully. Else, return NULL
// Takes an uint32_t argument length and set the member length of the structure
// to it
BitVector *bv_create(uint32_t length) {
  // Allocates memory for the new BitVector
  BitVector *bv = (BitVector *)malloc(sizeof(BitVector));
  // If the memory was allocated, set the members of the BitVector
  if (bv != NULL) {
    bv->length = length;
    bv->vector = (uint64_t *)calloc(length, sizeof(uint64_t));
  }
  // Returns the BitVector
  return bv;
}

// The destructor for a BitVector. Frees the vector member, frees the pointer to
// the bit vector, and set it to NULL
void bv_delete(BitVector **bv) {
  if (*bv) {
    free((*bv)->vector);
    free(*bv);
    *bv = NULL;
  }
}

// Returns the length of the BitVector
uint32_t bv_length(BitVector *bv) { return bv->length; }

// Sets the ith bit in a BitVector. The argument i specifies which bit to set.
void bv_set_bit(BitVector *bv, uint32_t i) {
  if (bv != NULL) {
    uint64_t byte = i / 64; // Gets the location of the byte that ith is in
    uint64_t bit = i % 64;  // Gets the location of the bits that ith is in
    // Since we want to set the value, the mask is 1
    // Use OR (|) because if the current value is 0, it will switch to 1 (0|1=1,
    // 1|1=1) Use shifting to get the right bit inside of the byte
    bv->vector[byte] |= ((1UL) << (bit));
  }
}

// Clears the ith bit in a BitVector. The argument i specifies which bit to
// clear.
void bv_clr_bit(BitVector *bv, uint32_t i) {
  if (bv != NULL) {
    uint64_t byte = i / 64; // Gets the location of the byte that ith is in
    uint64_t bit = i % 64;  // Gets the location of the bits that ith is in
    // Use AND (&) because 0&1=0 and 0&0=0, so the chosen bit will become 0
    // Shift the bit 0, bit amount of places
    // Use shifting to get the right bit inside of the byte
    bv->vector[byte] &= (0UL << (bit));
  }
}

// Return the ith bit in a BitVector. The argument i specifies which bit to get
uint8_t bv_get_bit(BitVector *bv, uint32_t i) {
  if (bv != NULL) {
    uint64_t byte = i / 64; // Gets the location of the byte that ith is in
    uint64_t bit = i % 64;  // Gets the location of the bits that ith is in
    // Use shifting to get the right bit
    // Use &1UL because if the value is 1, it will just return 1. Else, it will
    // return 0
    return ((bv->vector[byte] & ((1UL) << (bit))) >> bit);
  }
  // If bv doesn't exist, return 0
  return 0;
}

// Prints all characteristics of the BitVector
void bv_print(BitVector *bv) {
  // Goes through the entire BitVector, and prints each bit
  for (uint32_t i = 0; i < bv_length(bv); i += 1) {
    uint8_t b = bv_get_bit(bv, i);
    printf("%d", b);
  }
  printf("\n");
}
