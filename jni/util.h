#ifndef UTIL_H
#define UTIL_H

int ascii_to_utf16(char *input, char **output);
int search_memory(char *haystack, size_t haystacklength, char *needle, size_t needlelength);
void print_buffer(char *buffer, unsigned int length);

#endif

