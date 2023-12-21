#include "bf.h"
#include "city.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#define N_HASHES 5

// Defines what members/fields the BloomFilter structure has.
// Salts holds many individual salts, each acting as a key to a vector (for the
// Hash). n_keys tracks the number of keys inputed to the structure. n_hits
// tracks the number of probes that return true. n_misses track the number of
// probes that return false. n_bits_examined tracks that the total number of
// bits examined (1-5 in each probe). filter is a BitVector that is associated
// with the BloomFilter
typedef struct BloomFilter BloomFilter;

struct BloomFilter {
  uint64_t salts[N_HASHES];
  uint32_t n_keys;
  uint32_t n_hits;
  uint32_t n_misses;
  uint32_t n_bits_examined;
  BitVector *filter;
};

// A static list that holds the default values for the 5 salts
static uint64_t default_salts[] = {0x5adf08ae86d36f21, 0x419d292ea2ffd49e,
                                   0x50d8bb08de3818df, 0x272347aea4045dd5,
                                   0x7c8e16f768811a21};

// The constructor for BloomFilter. Creates a new BloomFilter and returns a
// pointer to it if the memory was allocated succesfully. Else, return NULL
// Takes an uint32_t argument size  and set the length of the filter member to
// it
BloomFilter *bf_create(uint32_t size) {
  // Allocates memory for the new BloomFilter
  BloomFilter *bf = (BloomFilter *)malloc(sizeof(BloomFilter));
  // If the memory was allocated, set the members of it
  if (bf) {
    bf->n_keys = bf->n_hits = 0;
    bf->n_misses = bf->n_bits_examined = 0;
    for (int i = 0; i < N_HASHES; i++) {
      bf->salts[i] = default_salts[i];
    }
    // Try to create the BitVector filter
    bf->filter = bv_create(size);
    if (bf->filter == NULL) {
      free(bf);
      bf = NULL;
    }
  }
  // Return the new BloomFilter
  return bf;
}

// The destructor for a BloomFilter.
// Frees the BitVector member, frees the pointer to BloomFilter, and set it to
// NULL
void bf_delete(BloomFilter **bf) {
  if (*bf) {
    bv_delete(&((*bf)->filter));
    free(*bf);
    *bf = NULL;
  }
}

// Returns the size of the BloomFilter.
uint32_t bf_size(BloomFilter *bf) {
  uint32_t l = bv_length(bf->filter); // The size is the length of its BitVector
  return l;
}

// Inserts the argument oldspeak into the BloomFilter. Sets the right indecies
// in the filter member to 1.
void bf_insert(BloomFilter *bf, char *oldspeak) {
  // Hashes oldspeak with each of the salts
  for (uint64_t i = 0; i < N_HASHES; i += 1) {
    uint64_t h = hash(bf->salts[i], oldspeak) % bf_size(bf);
    bv_set_bit(bf->filter, h);
  }
  bf->n_keys += 1; // inputted a new key
}

// Probes the BloomFilter for a given oldspeak word
bool bf_probe(BloomFilter *bf, char *oldspeak) {
  // Hashes oldspeak with each of the salts
  for (uint64_t i = 0; i < N_HASHES; i += 1) {
    uint64_t h = hash(bf->salts[i], oldspeak) % bf_size(bf);
    uint8_t b = bv_get_bit(bf->filter, h);
    bf->n_bits_examined +=
        1; // Increase the number of bits examined since we look at another bit
    if (b == 0) { // If the bit is 0, no need to check the rest of the salts, so
                  // return false
      bf->n_misses += 1;
      return false;
    }
  }
  // If it reached this point, then all salts are set, and return true
  bf->n_hits += 1;
  return true;
}

// Return the number of set bits in the BloomFilter
uint32_t bf_count(BloomFilter *bf) {
  uint32_t count = 0;
  // Goes through the filter BitVector, and counts the number of bits that are
  // equal to 1
  for (uint64_t i = 0; i < bf_size(bf); i += 1) {
    if (bv_get_bit(bf->filter, i) == 1) {
      count += 1;
    }
  }
  return count;
}

// Prints the BloomFilter. Prints all members of the structure (keys, hits,
// misses, bits examined, filter, and salts)
void bf_print(BloomFilter *bf) {
  bv_print(bf->filter); // Prints the BitVector member
  printf("n_keys: %d, n_hits: %d, n_misses: %d, n_bits_examined: %d\nsalts: ",
         bf->n_keys, bf->n_hits, bf->n_misses, bf->n_bits_examined);
  for (uint64_t i = 0; i < N_HASHES; i += 1) {
    printf("%lu", bf->salts[i]);
  }
  printf("\n");
}

// Sets the arguments to the original value of the statistics
void bf_stats(BloomFilter *bf, uint32_t *nk, uint32_t *nh, uint32_t *nm,
              uint32_t *ne) {
  *nk = bf->n_keys;
  *nh = bf->n_hits;
  *nm = bf->n_misses;
  *ne = bf->n_bits_examined;
}




