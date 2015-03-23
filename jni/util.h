#ifndef UTIL_H
#define UTIL_H

int ascii_to_utf16(char *input, char *output);
void search_memory(void *haystack, size_t haystacklength, void *needle, size_t needlelength);

#endif

