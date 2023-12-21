#include "ll.h"
#include "node.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint64_t seeks; // Number of seeks performed.
uint64_t links; // Number of links traversed.

// Returns the length of a list.
// The argument is a pointer to the array of characters.
uint64_t my_strlens(char *s) {
  if (s) { // If the string exists
    char c = s[0];
    uint64_t count = 0;
    for (uint64_t i = 0; c != '\0';
         i += 1) { // Go throgh the list, stopping at \0
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

// Returns 'true' if the two string arguments are the same, and 'false' if they
// are different.
bool my_strcmp(char *s, char *s1) {
  if (s != NULL & s1 != NULL) {
    uint64_t l = my_strlens(s);
    uint64_t l1 = my_strlens(s1);
    if (l != l1) {
      return false;
    } else {
      for (uint64_t i = 0; i < l; i += 1) {
        if (s[i] != s1[i]) {
          return false;
        }
      }
      return true;
    }
  }
  return false;
}

// Defines what members/fields the LinkedList structure has.
// The length is the size or the number of nodes.
// Head and Tail are sentinel nodes that signal the beginning and end of the
// linked list mtf enables the move-to-front feature
typedef struct LinkedList LinkedList;

struct LinkedList {
  uint32_t length;
  Node *head; // Head sentinel node.
  Node *tail; // Tail sentinel node.
  bool mtf;
};

// The constructor for the LinkedList. Creates a new LinkedList and returns a
// pointer to it if the memory was allocated succesfully. Else, return NULL.
// Takes a bool mtf that sets the mtf member to it.
LinkedList *ll_create(bool mtf) {
  LinkedList *ll = (LinkedList *)malloc(sizeof(LinkedList));
  // If the memory was allocated, set the members of the LinkedList
  if (ll) {
    ll->head = node_create(NULL, NULL); // Create nodes and set it to NULL
    ll->tail = node_create(NULL, NULL);
    ll->length = 0;            // length starts with 0
    ll->tail->prev = ll->head; // format: head points at tail
    ll->head->next = ll->tail;
    ll->mtf = mtf;
  }
  // Return the new LinkedList
  return ll;
}

// The destructor for a LinkedList.
// Frees all nodes and set them to NULL.
void ll_delete(LinkedList **ll) {
  if (*ll) {
    // Travels the list, each time deleting the head and setting the next node
    // as the new head
    while ((*ll)->head != NULL) {
      Node *next = NULL;
      next = ((*ll)->head)->next;
      node_delete(&(*ll)->head);
      (*ll)->head = next;
    }
    free(*ll);
    *ll = NULL;
  }
}

// Returns the length of the Linked List
uint32_t ll_length(LinkedList *ll) { return ll->length; }

// Searches for a node that has a specific oldspeak word.
// If it founds, return a pointer to it. Else, return NULL
Node *ll_lookup(LinkedList *ll, char *oldspeak) {
  // If the link is not empty
  seeks += 1;
  if (ll->head->next != ll->tail) {
    Node *temp = ll->head;
    // Go through all nodes in the list
    // Stop when the next node is the tail
    while (temp->next != ll->tail) {
      links += 1;
      // connects temp
      Node *n = temp;
      temp = temp->next;
      temp->prev = n;
      // Checks for oldspeak
      if (my_strcmp(temp->oldspeak, oldspeak) == true) {
        if (ll->mtf) {
          // makes sure all nodes are connected
          temp->prev->next = temp->next;
          temp->next->prev = temp->prev;
          // move the node to the front
          temp->next = ll->head->next;
          temp->prev = ll->head;
          ll->head->next = temp;
        }
        return temp;
      }
    }
  }
  return NULL;
}

// Inserts a new node into the list that has the given oldspeak and newspeak
void ll_insert(LinkedList *ll, char *oldspeak, char *newspeak) {
  if (ll_lookup(ll, oldspeak) == NULL) { // oldspeak isn't already in the list
    Node *n = node_create(oldspeak, newspeak); // Creating a new node
    // Puts node at the beginning of the list
    n->next = ll->head->next;
    n->prev = ll->head;
    // Changes head to point at the new node
    ll->head->next = n;
    ll->length += 1;
  }
}

// Prints all nodes of the LinkedList
void ll_print(LinkedList *ll) {
  // If the LinkedList exists
  if (ll) {
    // Travels through all nodes
    if (ll->head->next != ll->tail) {
      Node *temp = ll->head->next;
      node_print(temp);
      while ((temp = temp->next) != ll->tail) {
        node_print(temp);
      }
    }
  }
}

// Sets the stats (seeks and links)
void ll_stats(uint32_t *n_seeks, uint32_t *n_links) {
  *n_seeks = seeks;
  *n_links = links;
}
