#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/wait.h>

int ascii_to_utf16(char *input, char *output);

int main(int argc, char **argv) {

    FILE *out;
    int pid;
    long start_address, end_address;
    long bytes;

    if (!(argc > 5)) {
        printf("%s <pid> <start_address> <end_address> -s <string to search>\n", argv[0]); 
        printf("searches for occurrences of string (converts to utf-16).\n\n");
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

    bytes = end_address - start_address;
    
    // Attach to process
    ptrace(PTRACE_ATTACH, pid, NULL, NULL);
    wait(NULL);
    
    // Open mem file
    char filename[64];
    sprintf(filename, "/proc/%d/mem", pid);

    char *buffer = malloc(bytes);
    int mem;
    mem = open(filename, O_RDONLY);

    pread(mem, buffer, bytes, start_address);

    // Detach from process
    ptrace(PTRACE_CONT, pid, NULL, NULL);
    ptrace(PTRACE_DETACH, pid, NULL, NULL); 

    // Determine what we're doing
    if (strcmp(argv[4], "-d") == 0) {
        printf("Dumping %ld bytes to file %s\n", bytes, argv[5]);
        // Open binary file
        out = fopen(argv[5], "wb");

        if (out == NULL) {
            printf("Can't open file for writing.\n");
            free(buffer);
            exit(1);
        }

        fwrite(buffer, sizeof(char), bytes, out);
        fflush(out);
        fclose(out);
    }
    else if (strcmp(argv[4], "-s") == 0) {
        printf("Searching for string: %s\n", argv[5]);

        char *searchstring = malloc(strlen(argv[5]) * 2);
        int length = ascii_to_utf16(argv[5], searchstring);

        int i;
        for (i = 0; i < length; i++) {
            printf("%x ", searchstring[i]);
        }
        free(searchstring);

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





