#include "ht.h"
#include "city.h"
#include "ll.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// Defines what members/fields the HashTable has.
// Salt is acting as a key to a vector.
// Size is the number of LinkedList it has.
// n_keys tracks the number of keys inputed to the structure. n_hits
// tracks the number of lookups that return true. n_misses track the number of
// lookups that return false. n_bits_examined tracks that the total number of
// bits examined (using stats from LinkedList)
typedef struct HashTable HashTable;

struct HashTable {
  uint64_t salt;
  uint32_t size;
  uint32_t n_keys;
  uint32_t n_hits;
  uint32_t n_misses;
  uint32_t n_examined;
  bool mtf;
  LinkedList **lists;
};

// The constructor for the LinkedList. Creates a new LinkedList and returns a
// pointer to it if the memory was allocated succesfully. Else, return NULL
// Takes a bool mtf that sets the mtf member to it. Takes a size argument that
// sets the size of the HashTable.
HashTable *ht_create(uint32_t size, bool mtf) {
  HashTable *ht = (HashTable *)malloc(sizeof(HashTable));
  if (ht != NULL) {
    ht->mtf = mtf;
    ht->salt = 0x9846e4f157fe8840;
    ht->n_hits = ht->n_misses = ht->n_examined = 0;
    ht->n_keys = 0;
    ht->size = size;
    ht->lists = (LinkedList **)calloc(size, sizeof(LinkedList *));
    if (!ht->lists) {
      free(ht);
      ht = NULL;
    }
  }
  return ht;
}

// The destructor for a HashTable
// Frees all LinkedLists and set them to NULL, the HashTable object, and the
// lists member
void ht_delete(HashTable **ht) {
  if (*ht) { // If the pointer to the HashTable is not NULL
    uint32_t s = (**ht).size;
    for (uint32_t i = 0; i < s;
         i += 1) { // Go through all the LinkedLists and delete them
      ll_delete(&(*ht)->lists[i]);
    }
    // Delete the lists member
    free((*ht)->lists);
    (*ht)->lists = NULL;
    // Delete the HashTable object
    free(*ht);
    *ht = NULL;
  }
}

// Returns the size member of the HashTable
uint32_t ht_size(HashTable *ht) { return ht->size; }

Node *ht_lookup(HashTable *ht, char *oldspeak) {
  // LinkedList stats before running lookup
  uint32_t seeks = 0;
  uint32_t links = 0;
  // LinkedList stats at the end
  uint32_t u_seeks = 0;
  uint32_t u_links = 0;
  ll_stats(&seeks, &links);
  // find the index of the linked list
  uint64_t h = hash(ht->salt, oldspeak);
  h = h % (ht_size(ht));
  // if it exists
  if (ht->lists[h] != NULL) {
    // Find the index of the LinkedList that holds the oldspeak
    Node *n = ll_lookup(ht->lists[h], oldspeak); // Save the oldspeak's node
    ll_stats(&u_seeks,
             &u_links); // Call stats again and calculate the difference
    ht->n_examined += (u_links - links);
    if (n) { // If the node exists, then it's a hit. Else, it's a miss.
      ht->n_hits += 1; // +1 hits since we were able to find the node
      return n;
    } else {
      ht->n_misses += 1;
      return NULL;
    }
  } else {
    ll_stats(&u_seeks, &u_links);
    ht->n_examined += u_links - links;
    ht->n_misses += 1; // +1 misses since we weren't able to find the node
    return NULL;
  }
}

// Inserts a new oldspeak-newspeak pair into the HashTable.
// If the index of the LinkedList to insert doesn't exist, create the LinkedList
// and insert the values
void ht_insert(HashTable *ht, char *oldspeak, char *newspeak) {
  // find the index of the linked list to insert into
  uint64_t h = hash(ht->salt, oldspeak);
  h = h % (ht_size(ht));
  uint64_t s =
      ht->size; // Change the size to 64 bits to match with the hash index
  // Loop through all the LinkedLists until finding the one with the correct
  // index
  for (uint64_t i = 0; i < s; i += 1) {
    // if the linked list with the index exists
    if (i == h) {
      if (ht->lists[i]) { // Insert the oldspeak and new speak
        uint32_t l_before = ll_length(ht->lists[i]);
        ll_insert(ht->lists[i], oldspeak, newspeak);
        uint32_t l_after = ll_length(ht->lists[i]);
        if (l_before != l_after) { 
          	ht->n_keys += 1;
        }
        break;
      } else {
        // Create a new LinkedList, insert it to the lists memebr, and insert
        // the args
        ht->lists[h] = ll_create(ht->mtf);
        ll_insert(ht->lists[h], oldspeak, newspeak);
          	ht->n_keys += 1;
        break;
      }
    }
  }
  // Increase the number of keys because there's a new value
 // if (ll_lookup(ht->lists[h], oldspeak) == NULL) {
 // 	ht->n_keys += 1;
 // }
}

// Returns the number of non-NULL LinkedLists in the HashTable
uint32_t ht_count(HashTable *ht) {
  uint32_t count = 0;
  uint64_t s = ht->size;
  // Loop through all the lists, if it's not NULL, add 1
  for (uint64_t i = 0; i < s; i += 1) {
    if (ht->lists[i] != NULL) {
      count += 1;
    }
  }
  return count;
}

// Prints all characteristics of the HashTable.
// Including the array of LinkedLists and all members
void ht_print(HashTable *ht) {
  uint64_t s = ht->size;
  for (uint64_t i = 0; i < s; i += 1) {
    ll_print(ht->lists[i]);
  }
  printf("mtf (1=true, 0=false): %d, salt: %lu, size: %d, n_keys: %d, "
         "n_misses: %d, n_hits: %d, n_examined: %d\n",
         ht->mtf, ht->salt, ht->size, ht->n_keys, ht->n_misses, ht->n_hits,
         ht->n_examined);
}

// Sets the stats (keys, hits, misses, and examined)
void ht_stats(HashTable *ht, uint32_t *nk, uint32_t *nh, uint32_t *nm,
              uint32_t *ne) {
  *nk = ht->n_keys;
  *nh = ht->n_hits;
  *nm = ht->n_misses;
  *ne = ht->n_examined;
}
