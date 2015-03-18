#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <sys/ptrace.h>
#include <sys/wait.h>

int ascii_to_utf16(char *input, char *output);

int main(int argc, char **argv) {

    FILE *out;
    int pid;
    size_t start_address, end_address;
    size_t bufferlength;

    if (!(argc > 5)) {
        printf("%s <pid> <start_address> <end_address> -s -[a|u] <string to search>\n", argv[0]); 
        printf("Search for occurrences of string.\nIf -a option, search using plain ASCII\n");
        printf("If -u option, convert ASCII string to UTF-16 (for Java strings)\n");
        printf("%s <pid> <start_address> <end_address> -d <file>\n", argv[0]);
        printf("dumps memory region to file\n");
        printf("Start/end addresses are in hex, without preceding 0x notation\n");
        exit(1);
    }

    // Extract PID from command line
    pid = atoi(argv[1]);

    // start/end addresses
    sscanf(argv[2], "%x", (unsigned int *)&start_address);
    sscanf(argv[3], "%x", (unsigned int *)&end_address);

    bufferlength = end_address - start_address;
    
    // Attach to process
    ptrace(PTRACE_ATTACH, pid, NULL, NULL);
    wait(NULL);
    
    // Open mem file
    char filename[64];
    sprintf(filename, "/proc/%d/mem", pid);

    char *buffer = malloc(bufferlength);
    int mem;
    mem = open(filename, O_RDONLY);

    pread(mem, buffer, bufferlength, start_address);

    // Detach from process
    ptrace(PTRACE_CONT, pid, NULL, NULL);
    ptrace(PTRACE_DETACH, pid, NULL, NULL); 

    // Determine what we're doing
    if (strcmp(argv[4], "-d") == 0) {
        printf("Dumping %zu bytes to file %s\n", bufferlength, argv[5]);
        // Open binary file
        out = fopen(argv[5], "wb");

        if (out == NULL) {
            printf("Can't open file for writing.\n");
            free(buffer);
            exit(1);
        }

        fwrite(buffer, sizeof(char), bufferlength, out);
        fflush(out);
        fclose(out);
    }
    else if (strcmp(argv[4], "-s") == 0) {
        char *findstring;
        int findlength;

        if (strcmp(argv[5], "-u") == 0) {
            findstring = malloc(strlen(argv[6]) * 2);
            findlength = ascii_to_utf16(argv[6], findstring);
        }
        else if (strcmp(argv[5], "-a") == 0) {
            findstring = malloc(strlen(argv[6]));
            findlength = strlen(argv[6]);
            memcpy(findstring, argv[6], findlength);
        } 
        else {
            printf("Need a -a or -u option!\n");
            free(buffer);
            exit(1);
        }


        // Pointer to current position in buffer
        char *searchstart = buffer; 

        // Length of remaining buffer
        size_t searchlength = bufferlength;

        while(1) {
            char *found = memmem(searchstart, searchlength, findstring, findlength);

            // If nothing was found, exit the loop
            if (found == NULL) {
                break;
            }

            printf("Found string at location: %p\n", found);

            // Adjust pointer to the found location plus length of search string
            // This is where the next buffer search will start
            searchstart = found + findlength;

            // Get the offset into the original search buffer 
            size_t foundoffset = found - buffer;
            
            // Adjust number of bytes to search 
            if((bufferlength - foundoffset - findlength) <= 0) {
                break;
            }
            searchlength = bufferlength - foundoffset - findlength;

        }
        free(findstring);

    }

    free(buffer);
}


int ascii_to_utf16(char *input, char *output) {

    int length = strlen(input);

    int i;
    int outputidx = 0;

    for(i = 0; i < length; i++) {
        output[outputidx] = 0x00;
        output[outputidx + 1] = input[i];
        outputidx += 2;
    }

    return outputidx;
}





