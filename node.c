#include "node.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Defines what members/fields the Node structure has
typedef struct Node Node;

// My implementation of the strlen function from the string library
// Takes an argument string s and returns its length
uint64_t my_strlen(char *s) {
  if (s) {
    char c = s[0];
    uint64_t count = 0;
    for (uint64_t i = 0; c != '\0'; i += 1) {
      c = s[i];
      if (c == '\0') {
        break;
      }
      count += 1;
    }
    return count;
  }
  return 0;
}

// My implementation of the strdup function from the string library
char *my_strdup(char *s) {
  if (s) {
    // Allocates memory for the duplication
    char *dup = (char *)malloc(sizeof(char) * (my_strlen(s) + 1));
    char c = ' ';
    int check = 0;
    // Duplicates each character in the string argument
    for (uint64_t i = 0; check == 0; i += 1) {
      c = s[i];
      dup[i] = c;
      if (c == '\0') {
        check = 1;
      }
    }
    // Returns the dup
    return dup;
  }
  return NULL;
}

// The constructor for a Node. Creates a new Node and returns a pointer to it if
// the memory was allocated succesfully. Else, return NULL Oldspeak and
// newspeak. are strings that represent words Next holds the address of the next
// node, while prev holds the previous node.
Node *node_create(char *oldspeak, char *newspeak) {
  // Allocates memory for the new Node
  Node *n = (Node *)malloc(sizeof(Node));
  // If the memory was allocated, set the members of it
  if (n) {
    n->oldspeak = my_strdup(oldspeak);
    n->newspeak = my_strdup(newspeak);
    n->next = NULL;
    n->prev = NULL;
    return n;
  }
  return NULL;
}

// The destructor for a Node. Frees the pointer to the Node, and set it to NULL
void node_delete(Node **n) {
  if (*n) {
    // free badspeak and newspeak
    free((*n)->oldspeak);
    free((*n)->newspeak);
    //    free(dup);
    free(*n);
    *n = NULL;
  }
}

// Prints all characteristics of the Node based on specifications given on the
// assignment PDF
void node_print(Node *n) {
  if (n) {
    // If there's only an oldspeak
    if (n->newspeak == NULL) {
      printf("%s\n", n->oldspeak);
    } // If there are both the oldspeak and newspeak
    else if ((n->oldspeak != NULL) && (n->newspeak != NULL)) {
      printf("%s -> %s\n", n->oldspeak, n->newspeak);
    }
  }
}
