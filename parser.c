#include "parser.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#define MAX_PARSER_LINE_LENGTH 1000

// The char c is used to hold the last character that was in NextWord.
// This way, we could easily know when a line ends.
char c = '\n';

// My implementation of the strlen function from the string library.
// Takes a string argument and returns its size.
uint64_t mystrlens(char *s) {
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
bool mystrcmp(char *s, char *s1) {
  if (s != NULL & s1 != NULL) { // If both strings exist
    uint64_t l = mystrlens(s);
    uint64_t l1 = mystrlens(s1);
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

// My implementation of the strcpy function from the string library.
// Copy each char of string s to string s1.
void my_strcpy(char *s1, char *s) {
  if ((s != NULL) & (s1 != NULL)) {
    int check = 0;
    for (uint64_t i = 0; check == 0; i += 1) {
      *(s1 + i) = *(s + i);
      // printf("char is %c\n", *(s+i));
      if (*(s + i) == '\0') {
        check = 1;
      }
    }
  }
}

// Defines what members/fields the Parser structure has.
// The f is the file it parses.
// Current line is a string that holds the current line of the file.
// Line offset is used to check how much of the line was read already.
typedef struct Parser Parser;

struct Parser {
  FILE *f;
  char current_line[MAX_PARSER_LINE_LENGTH + 1];
  uint32_t line_offset;
};

// The constructor for the Parser. Creates a new Parser and returns a
// pointer to it if the memory was allocated succesfully. Else, return NULL
// Takes a file f and set it to the file f.
Parser *parser_create(FILE *f) {
  // Allocates memory for the new Parser
  Parser *p = (Parser *)malloc(sizeof(Parser));
  // If the memory was allocated, set the members of the Parser
  if (p != NULL) {
    p->f = f;
    p->line_offset = 0;
  }
  // Returns the Parser
  return p;
}

// The destructor for a Parser.
// Closes the file, frees the pointer to the Parser, and set it to NULL.
void parser_delete(Parser **p) {
  if (*p) {
      fclose((*p)->f);
      (*p)->f = NULL;
      free(*p);
      *p = NULL;
  }
}

// Finds the next word from a file
bool next_word(Parser *p, char *word) {
  char new_word[MAX_PARSER_LINE_LENGTH + 1] = "";
  // If the file is not stdin
  if (p->f != stdin) {
    // Read a line and save it in current line
    if (fgets(p->current_line, MAX_PARSER_LINE_LENGTH, p->f) != NULL) {
              p->current_line[MAX_PARSER_LINE_LENGTH] = '\n';
      char c = p->current_line[0];
      for (uint64_t i = 0; c != '\0'; i += 1) {
        c = p->current_line[i];
        p->line_offset += 1;
        if (isalnum(c) != 0 || c == '\'' || c == '-') {
          if (isalpha(c) != 0) { // if it's alphabetical
            c = tolower(c);      // change to lower case
          }
          new_word[i] = c;
        } else {
          // Copy the new word to word
          my_strcpy(word, new_word);
          fseek(p->f, p->line_offset, SEEK_SET);
          return true;
        }
      }
    }
    return false;

    // If it's stdin
  } else {
    // If this is the beginning of a line
    if ((!feof(p->f)) & (p->f != NULL)) {
      if ((c == '\n') & (c != EOF)) {
        // Get a new line using fgets
        if (fgets(p->current_line, MAX_PARSER_LINE_LENGTH, p->f) != NULL) {
          // To handle files with more characters than MAX_PARSER_LINE_LENGTH,
          // set the last character of the line to \n. This way, it will split
          // the long line to a couple of smaller lines
         // p->current_line[MAX_PARSER_LINE_LENGTH] = '\n';
           c = p->current_line[MAX_PARSER_LINE_LENGTH];
          if (isalnum(c) == 0 && c != '\'' &&  c != '-') {
          	p->current_line[MAX_PARSER_LINE_LENGTH] = '\n';
          }
          else {
                for (uint64_t i = MAX_PARSER_LINE_LENGTH; i >=0; i -= 1) {
          		c = p->current_line[i];
          		if (isalnum(c) == 0 && c != '\'' &&  c != '-') {
          		p->current_line[i] = '\n';
          		}
          		
          	}
          } 
          // Get a word from this line.
          // Word stops using either \0, or things other than - ' letters and
          // numbers
          p->line_offset = 0; // Reset the line offset each time
          for (uint64_t i = 0; i < MAX_PARSER_LINE_LENGTH + 1; i += 1) {
            c = p->current_line[i]; // Gets current character
            p->line_offset += 1;    // Number of read chars
            if (isalnum(c) != 0 || c == '\'' || c == '-') {
              if (isalpha(c) != 0) { // if it's alphabetical
                c = tolower(c);      // change to lower case
              }
              new_word[i] = c;
            } else {
              break;
            }
          }
          // change the line offset and return the new word
          my_strcpy(word, new_word);
          fseek(p->f, p->line_offset, SEEK_SET);
          return true;
        }

        // If it's not a new line, and there are more characters left
      } else {
        // Similar actions as seen above
        uint64_t target = p->line_offset;
        for (int i = p->line_offset; c != '\n'; i += 1) {
          c = p->current_line[i];
          p->line_offset += 1; // Number of read chars
          if (isalnum(c) != 0 || c == '\'' || c == '-' || c == '\0') {
            if (isalpha(c) != 0) { // if it's alphabetical
              c = tolower(c);      // change to lower case
            }
            new_word[i - target] = c;
          } else {
            break;
          }
        }
        // Change the line offset and return the new word
        my_strcpy(word, new_word);
        fseek(p->f, p->line_offset, SEEK_SET);
        return true;
      }
    }
    return false;
  }
}



/*   c = p->current_line[MAX_PARSER_LINE_LENGTH];
          if (isalnum(c) == 0 && c != '\'' &&  c != '-') {
          	p->current_line[MAX_PARSER_LINE_LENGTH] = '\n';
          }*/
        /*  else {
                for (uint64_t i = MAX_PARSER_LINE_LENGTH; i >=0; i -= 1) {
          		c = p->current_line[i];
          		if (isalnum(c) == 0 && c != '\'' &&  c != '-') {
          		p->current_line[i] = '\n';
          		}
          		
          	}
          }*/
