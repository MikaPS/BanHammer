***How to run the script (banhammer.c)***<br>
The script reads in user input and prints out a corresponding message with the list of problematic words. To compile the script, type in the command line “make” or “make banhammer”. Afterward, you can run the program by writing “echo [text] | ./banhammer” or “cat [filename] | ./banhammer” followed by command line options. It will print a message, a list of problematic words (badspeak), and a list of bad words with their new translation (a pair of oldspeak and newspeak). 

***Command Line Options***<br>
Need to call the script following these options: -t (set the size of the HashTable). -f (set the size of the BloomFilter), -m (enables the move-to-front feature on LinkedList), -s (enables statistics message), -h (prints help usage message). You can mix and match the command options. For example, you are allowed to call -t -f to set both the sizes of the hash table and bloom filter. Inputting other options will lead to an error message.

***Files***<br>
DESIGN.pdf - shows my general idea and pseudo-code for my code. It has both my initial design and the final one.

WRITEUP.pdf - contains my reflection of the assignment (what I learned) and citations of websites I used while completing the assignment. As well as my answers for the pre-lab questions that were provided on the assignment pdf.

README.md - has descriptions on how to run the script, files in the directory, and citations.

Makefile - a script used to compile my sorting file and clean the files after running. You can compile the files by writing “make {name of function}”. "make format" will format all c files. "make clean" will erase all compiler-generated files except the executables. "make spotless" will delete all compiler generated files. 

banhammer.c - contains the main(). Gets user input from the command line and prints data based on that. Explained in more detail in the command line options section.

messages.h - a header file that has contains the three error messages that are printed based on the user input.

cityhash.h -  a header file that has the declaration of all the functions used in cityhash.c and specifies its interface.

cityhash.c - implements a hash function using CityHash.

ht.h -  a header file that has the declaration of all the functions used in ht.c and specifies the interface for hash table ADT.

ht.c - implements the hash table, which will hold the values of Oldspeak and Newspeak pairs.

ll.h - a header file that has the declaration of all the functions used in ll.c and specifies the interface for linked list ADT.

ll.c - implements the double linked list, which is made up of nodes.

node.h - a header file that has the declaration of all the functions used in node.c and specifies the interface for node ADT.

node.c - implements a node, which will hold a value, the address of the previous node, and the address of the next node.

bf.h - a header file that has the declaration of all the functions used in bf.c and specifies the interface for bloom filter ADT.

bf.c - implements a bloom filter, which checks if a value is definitely not in the filter, or probably in the filter.

bv.h - a header file that has the declaration of all the functions used in bv.c and specifies the interface for the bit vector ADT.

bv.c - implements a bit vector, which has bits of either 0 or 1.

parser.h  - a header file that has the declaration of all the functions used in parser.c and specifies the interface for the parser ADT.

parser.c - implements a parser moudle, which could provide the next word from a given file.

***Citations:***<br>
1)) Understand error message - https://stackoverflow.com/questions/27636306/valgrind-address-is-0-bytes-after-a-block-of-size-8-allocd 

2)) Use Valgrind to find uninitialized vars - https://stackoverflow.com/questions/2612447/pinpointing-conditional-jump-or-move-depends-on-uninitialized-values-valgrin 

3)) Understand fgets() usage-  https://www.ibm.com/docs/en/i/7.4?topic=functions-fgets-read-string 

4)) covert char to an int (didn’t use at the end) - https://www.tutorialspoint.com/how-do-i-convert-a-char-to-an-int-in-c-and-cplusplus 

5)) More Valgrind debugging commands - https://stackoverflow.com/questions/5134891/how-do-i-use-valgrind-to-find-memory-leaks 


6)) Understanding diff - https://www.geeksforgeeks.org/diff-command-linux-examples/ 

