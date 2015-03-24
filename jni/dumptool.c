#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <sys/ptrace.h>
#include <sys/wait.h>

#include "util.h"

int main(int argc, char **argv) {

    FILE *out;
    int pid;
    size_t start_address, end_address;
    size_t bufferlength;

    if (!(argc > 5)) {
        printf("%s <pid> <start_address> <end_address> -s -[a|u] <string to search>\n", argv[0]); 
        printf("Search for occurrences of string.\nIf -a option, search using plain ASCII\n");
        printf("If -u option, convert ASCII string to UTF-16 (for Java strings)\n\n");
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
        char *findstring = NULL;
        int findlength;

        if (strcmp(argv[5], "-u") == 0) {
            findlength = ascii_to_utf16(argv[6], &findstring);
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
        
        int j;

        search_memory(buffer, bufferlength, findstring, findlength);

        free(findstring);

    }

    free(buffer);
}







