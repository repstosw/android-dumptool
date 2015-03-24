#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>

#include "util.h"

#define OMAP3_MEM_START     0x80000000
#define OMAP3_MEM_END       0x8FFFFFFF

int main(int argc, char **argv) {
    
    int fd;
    void *map_base;
    size_t mapped_size;
    size_t page_size;

    if (argc < 2) {
        printf("Usage: dumpmem <searchstring> [-u]\n");
        printf("if -u option, convert search string to UTF-16 before searching\n");
        exit(1);
    }


    fd = open("/dev/mem", O_RDONLY | O_SYNC);

    if (fd < 0) {
        printf("Can't open /dev/mem\n");
        exit(1);
    }

    mapped_size = (OMAP3_MEM_END - OMAP3_MEM_START) + 1;

    map_base = mmap(NULL,
            mapped_size,
            PROT_READ,
            MAP_SHARED,
            fd,
            OMAP3_MEM_START);

    if (map_base == NULL) {
        printf("Couldn't perform mapping");
        close(fd);
        exit(1);
    }
    close(fd);

    printf("Searching starting at address: %p\n", map_base);

    if (argc >= 2) { 
        if (strcmp(argv[2], "-u") == 0) {
            printf("Searching for pattern: \n");
            char *needle;
            size_t needlelength = ascii_to_utf16(argv[1], &needle);
            print_buffer(needle, needlelength);
            search_memory(map_base, mapped_size, needle, needlelength);
        }
        else {
            printf("Couldn't understand \"%s\" argument. Fix it.\n", argv[2]);
        }
    }
    else {
        printf("Searching for pattern: %s\n", argv[1]);
        search_memory(map_base, mapped_size, argv[1], strlen(argv[1])); 
    }
}
