#include "bf.h"
#include "bv.h"
#include "ht.h"
#include "ll.h"
#include "messages.h"
#include "node.h"
#include "parser.h"
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_PARSER_LINE_LENGTH 1000

// My implementation of the strlen function from the string library.
// Takes a string argument and returns its size.
uint64_t my_strlength(char *s) {
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

// My implementation of the strcmp function from the string library.
// Compares the two argument strings; returns false if there is a different.
// Returns true if they are the same.
bool mystrcmps(char *s, char *s1) {
  if (s != NULL & s1 != NULL) { // If both strings exist
    uint64_t l = my_strlength(s);
    uint64_t l1 = my_strlength(s1);
    if (l != l1) { // If their lengths are different, they can't be the same
      return false;
    } else {
      for (uint64_t i = 0; i < l;
           i += 1) { // Check each character and compare the difference
        if (s[i] != s1[i]) {
          return false;
        }
      }
      return true;
    }
  }
  return false;
}
// int test(void);

// Function to print the help usage message
void print_error(void) {
  fprintf(stderr, "Usage: ./banhammer [options]\n");
  fprintf(stderr, "  ./banhammer will read in words from stdin, identify any "
                  "badspeak or old speak and output an\n");
  fprintf(stderr, "  appropriate punishment message. The badspeak and oldspeak "
                  "(with the newspeak translation)\n");
  fprintf(stderr, "  that caused the punishment will be printed after the "
                  "message. If statistics are enabled\n");
  fprintf(stderr, "  punishment messages are suppressed and only statistics "
                  "will be printed.\n");
  fprintf(
      stderr,
      "    -t <ht_size>: Hash table size set to <ht_size>. (default: 10000)\n");
  fprintf(
      stderr,
      "    -f <bf_size>: Bloom filter size set to <bf_size>. (default 2^19)\n");
  fprintf(stderr, "    -s          : Enables the printing of statistics.\n");
  fprintf(stderr, "    -m          : Enables move-to-front rule.\n");
  fprintf(stderr, "    -h          : Display program synopsis and usage.\n");
}

// int main(void) {  test(); return 0;}

// Main function of the program
// Takes user input, and prints a message with the badspeak and
// oldspeak-newspeak pairs
int main(int argc, char **argv) {
  int opt = 0; // used for getopt
  // set default numbers
  uint32_t ht_size = 10000;
  uint64_t bf_sizes = pow(2, 19);
  uint32_t mtf = 0;
  uint32_t stats = 0;
  LinkedList *thought_crime =
      ll_create(true); // Holds all the words for thought crime
  LinkedList *rightspeak =
      ll_create(true); // Holds all the words for rightspeak crime
  //int false_positive = 0;

  // gets user input and runs until processes all the commands
  while ((opt = getopt(argc, argv, "t:f:msh")) !=
         -1) { // list of valid commands
    // sets the size of the hash table
    if (opt == 't') {
      if (optarg[0] == '-') { 
         fprintf(stderr, "./banhammer: Invalid hash table size.\n");
         return 1;
      }
      ht_size = strtoul(optarg, NULL, 10);
      if (ht_size <= 0) {
      	fprintf(stderr, "./banhammer: Invalid hash table size.\n");
      	return 1;
      }
    }
    // sets the size of the bloom filter
    if (opt == 'f') {
    	if (optarg[0] == '-') { 
         fprintf(stderr, "./banhammer: Invalid bloom filter table size.\n");
         ll_delete(&rightspeak);
  	ll_delete(&thought_crime);
         return 1;
      }
      bf_sizes = strtoul(optarg, NULL, 10);
      if (bf_sizes <= 0) {
      	fprintf(stderr, "./banhammer: Invalid bloom filter table size.\n");
      	ll_delete(&rightspeak);
  	ll_delete(&thought_crime);
      	return 1;
      }
    }
    // enables the move-to-front feature
    if (opt == 'm') {
      mtf = 1;
    }
    // enables display of statistics
    if (opt == 's') {
      stats = 1;
    }
    // usage message
    if (opt == 'h') {
      print_error();
      return 0;
    }
    // if it's not in the above options, return an error number
    if (opt != 'h' && opt != 't' && opt != 'f' && opt != 'm' && opt != 's') {
      print_error();
      ll_delete(&rightspeak);
      ll_delete(&thought_crime);
      return 1;
    }
  }

  // Creates all the needed structures
  BloomFilter *bf = bf_create(bf_sizes);
  HashTable *ht = ht_create(ht_size, mtf);

  // Reads in from badspeak file and inserts them to the Bloom Filter & Hash
  // Table
  FILE *f = fopen("badspeak.txt", "r");
  if (f == NULL) {
  	printf("can't open file\n");
  	return 1;
  }
  Parser *p = parser_create(f);
  char word[MAX_PARSER_LINE_LENGTH + 1] = "";
  while (next_word(p, word)) {
    bf_insert(bf, word);
    ht_insert(ht, word, NULL);
  }

  // Reads in from newspeak file and inserts them to the Bloom Filter & Hash
  // Table accordingly
  FILE *new = fopen("newspeak.txt", "r");
  if (new == NULL) {
  	printf("can't open file\n");
  	return 1;
  }
  Parser *np = parser_create(new);
  char oldspeak[MAX_PARSER_LINE_LENGTH + 1] = "";
  char newspeak[MAX_PARSER_LINE_LENGTH + 1] = "";
  while (next_word(np, oldspeak) && next_word(np, newspeak)) {
    bf_insert(bf, oldspeak);
    ht_insert(ht, oldspeak, newspeak);
  }

  // Reads values from stdin
  FILE *std = stdin;
  Parser *ip = parser_create(std);
  while (!feof(std)) {
    if (next_word(ip, oldspeak)) {
     if (mystrcmps(oldspeak, " ") || mystrcmps(oldspeak, "\n") || mystrcmps(oldspeak, "")) {
          continue;
      }

      if (bf_probe(bf, oldspeak) ==
          true) { // Checks if the word is already in the Bloom Filter
           //   printf("%s\n", oldspeak);
        Node *n = ht_lookup(ht, oldspeak); // If it is, find the right node
                                           // associated with the oldspeak
        if (n) {
          if (n->newspeak ==
              NULL) { // If it's only oldspeak, then thought crime
            ll_insert(thought_crime, oldspeak, NULL);
          } else { // If both, then rightspeak crime
            ll_insert(rightspeak, oldspeak, n->newspeak);
          }
        }
      }
    }
  }

  // Prints the right messages based on the crimes
  if (stats == 0) {
    if ((ll_length(thought_crime) > 0) && (ll_length(rightspeak) > 0)) {
      printf("%s", mixspeak_message);
      ll_print(thought_crime);
      ll_print(rightspeak);
    } else if (ll_length(thought_crime) > 0) {
      printf("%s", badspeak_message);
      ll_print(thought_crime);
    } else if (ll_length(rightspeak) > 0) {
      printf("%s", goodspeak_message);
      ll_print(rightspeak);
    }
  }
  // Prints stats
  if (stats == 1) {
    uint32_t bnk = 0;
    uint32_t bnh = 0;
    uint32_t bnm = 0;
    uint32_t bne = 0;
    uint32_t hnk = 0;
    uint32_t hnh = 0;
    uint32_t hnm = 0;
    uint32_t hne = 0;
    bf_stats(bf, &bnk, &bnh, &bnm, &bne);
    ht_stats(ht, &hnk, &hnh, &hnm, &hne);
    fprintf(stdout, "ht keys: %u\nht hits: %u\nht misses: %u\nht probes: %u\n",
            hnk, hnh, hnm, hne);
    fprintf(stdout,
            "bf keys: %u\nbf hits: %u\nbf misses: %u\nbf bits examined: %u\n",
            bnk, bnh, bnm, bne); 
    double bepm;
    if (bnm == 0) {
      bepm = 0;
    } else {
      bepm = (bne - (5 * bnh)) / (double)bnm;
    }
    double fp;
    if (bnh == 0) {
      fp = 0;
    } else {
      fp = (double)hnm / bnh;
    }
    double bfl;
    if (bf_size(bf) == 0) {
      bfl = 0;
    } else {
      bfl = (double)((bf_count(bf))) / (bf_size(bf));
    }
    double asl;
    if (hnh + hnm == 0) {
      asl = 0;
    } else {
      asl = ((double)hne) / (hnh + hnm);
    }
    fprintf(
        stdout,
        "Bits examined per miss: %.6lf\nFalse positives: %.6lf\nAverage seek "
        "length: %.6lf\nBloom filter load: %.6lf\n",
        bepm, fp, asl, bfl); 

    // Used for bash:
       //fprintf(stdout, "%lu %.6lf\n", bf_sizes, fp); //false positive
       //fprintf(stdout, "%u %u\n", ht_size, hne); // hne = number of links
       //fprintf(stdout, "%lu %.6lf\n", bf_sizes, bepm);
  }

  // Delete all structures and frees memory
  ll_delete(&rightspeak);
  ll_delete(&thought_crime);
  bf_delete(&bf);
  ht_delete(&ht);
  parser_delete(&p);
  parser_delete(&np);
  parser_delete(&ip);

  return 0;
}



int test(void) {

  uint32_t l = 35300;
  BitVector *bv = bv_create(l);
  //  BloomFilter *bf = bf_create(l);
  /* printf("----------PARSER TESTS---------\n");
   FILE *f = fopen("test", "r");
   Parser *p = parser_create(f);
   char *word = "";
   char *w = next_word(p, word);
   printf("word is %s", w);
   w = next_word(p, word);
   printf("word is %s", w);

  printf("---------HASH TESTS---------\n");
  HashTable *ht = ht_create(100, true);
  //  ht_print(ht);
  ht_insert(ht, "hey", "bye");
  printf("printing first element of hash: \n");
  ht_print(ht);
  ht_insert(ht, "1", "10");
  ht_insert(ht, "2", "20");
  ht_print(ht);
  // Node *one =
  printf("look hey: ");
  Node *hey = ht_lookup(ht, "hey");
  node_print(hey);
  printf("look 1: ");
  node_print(ht_lookup(ht, "1"));
  printf(" look 5: ");
  node_print(ht_lookup(ht, "5"));
  printf("count is %u", ht_count(ht));
  ht_delete(&ht);

    printf("-------LIST TESTS--------\n");
    LinkedList *ll = ll_create(true);
    // ll_print(ll);
    ll_insert(ll, "1", "10");
    /bool my_strcmp(char *s, char *s1);
    //printf("test str cmp: %d\n", my_strcmp("hey", "bye"));

    printf("printing the first node, 1\n");
    ll_print(ll);
    ll_insert(ll, "2", "20");
    ll_insert(ll, "3", "30");
    ll_insert(ll, "4", "40");
    printf("printing all four nodes, 4->3->2->1\n");
    ll_print(ll);
    printf("calling lookup...\n");
    Node *nl = ll_lookup(ll, "2");
    ll_lookup(ll, "2");
    printf("2 needs to be in the front\n");
    ll_print(ll);
    ll_insert(ll, "1", "20");
    //    ll_print(ll);
    printf("length is %u\n", ll_length(ll));
    ll_delete(&ll);
        //    printf("%u\n", ll_length(ll));

  printf("---------NODE TESTS------");
  uint64_t my_strlen(char *s);
  char *my_strdup(char *s);
  char *word = "hey";
  printf("%s\n", word);
  uint64_t l = my_strlen("123456789");
  printf("length of hey is %lu\n", l);
  char *my_dup = my_strdup(word);
  printf("my dup: %s", my_dup);
  Node *n = node_create(word, "bye");
  node_print(n);
  Node *n1 = node_create("1", NULL);
  node_print(n1);
  LinkedList *ll = ll_create(true);
  ll_insert(ll, "1", NULL);
  ll_print(ll);
  //    Node *n = node_create(c, "bye");
  //  node_print(n);
  //      c = "other";

            uint64_t l = my_strlen("vhdnsljy6t");
            printf("length of hey is %lu\n", l);
            char * word = "hey";
            char * my_dup = my_strdup(word);
            char * dup = strdup(word);
            char *test = word;
            printf("my dup is %p theirs is %p test is %p address of word %p\n",
      (void
      *)&my_dup,(void *)&dup, (void *)&test, (void *)&word);
        //    word = "mika";
            printf("my dup is %s theirs is %s test is %s word is %s\n",
      my_dup,dup,test,word);

          //  Node *no = node_create();
          //  node_print(no);


            printf("------BF TESTS-----\n");
              bf_print(bf);
              printf("size: %u\n", bf_size(bf));
              bf_insert(bf, "hry");
              bf_print(bf);
              printf("hry: %d\n bye:%d\n", bf_probe(bf, "hry"), bf_probe(bf,
      "bye")); printf("count is %d\n", bf_count(bf)); bf_delete(&bf);
              //  bf_print(bf);
*/
  printf("------BV TESTS-----\n");
  // bv_print(bv);
  uint32_t t = 3;
  bv_set_bit(bv, t);
  bv_set_bit(bv, 32300);
  bv_set_bit(bv, 12390);
  bv_set_bit(bv, 12);
  printf("bit is %d\n", bv_get_bit(bv, 12));
  printf("bit is %d\n", bv_get_bit(bv, 32300));
  printf("bit is %d\n", bv_get_bit(bv, 12390));
  //  bv_print(bv);
  bv_clr_bit(bv, 25);
  //   bv_print(bv);
  printf("bit is %d\n", bv_get_bit(bv, 14));

  return 0;
}
