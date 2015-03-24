#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util.h"

/**
 * Translate an ASCII string to UTF-16 by prepending 0x00 to each byte
 *
 * input - Input string
 * output - Char pointer to receive the string. Will be malloced to 2x the length of the input string
 *
 * Returns - Length of returned string
 */
int ascii_to_utf16(char *input, char **output) {

    int length = strlen(input);

    int i;
    int outputidx = 0;

    char *new = malloc(length * 2);
    if (new == NULL) {
        return -1;
    }

    for(i = 0; i < length; i++) {
        new[outputidx] = 0x00;
        new[outputidx + 1] = input[i];
        outputidx += 2;
    }

    *output = new;
    return outputidx;
}


/**
 * Search memory for a given string
 * Prints the pointer location to stdout if any strings are found
 *
 * haystack - Pointer to buffer to search
 * haystacklength - Length of search buffer
 * needle - What to search for
 * needlelength - How long needle is
 */
void search_memory(char *haystack, size_t haystacklength, char *needle, size_t needlelength) {
    //
    // Pointer to current position in buffer
    void *searchstart = haystack; 

    // Length of remaining buffer
    size_t searchlength = haystacklength;

    while(1) {
        char *found = memmem(searchstart, searchlength, needle, needlelength);

        // If nothing was found, exit the loop
        if (found == NULL) {
            break;
        }

        printf("Found string at location: %p\n", found);

        // Adjust pointer to the found location plus length of search string
        // This is where the next buffer search will start
        searchstart = found + needlelength;

        // Get the offset into the original search buffer 
        size_t foundoffset = found - haystack;
        
        // Adjust number of bytes to search 
        if((haystacklength - foundoffset - needlelength) <= 0) {
            break;
        }
        searchlength = haystacklength - foundoffset - needlelength;

    }

}
