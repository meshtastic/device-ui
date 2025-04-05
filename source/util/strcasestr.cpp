#ifdef ARCH_RP2040

#include <ctype.h>

char *strcasestr(const char *haystack, const char *needle) {
    if (!haystack || !needle) {
        return NULL;
    }

    // Get the lengths of the haystack and needle
    size_t needle_len = 0;
    while (needle[needle_len] != '\0') {
        needle_len++;
    }

    if (needle_len == 0) {
        return (char *)haystack; // Empty needle matches the start of haystack
    }

    for (; *haystack; haystack++) {
        // Check if the current substring matches the needle (case-insensitive)
        size_t i;
        for (i = 0; i < needle_len; i++) {
            if (tolower((unsigned char)haystack[i]) != tolower((unsigned char)needle[i])) {
                break;
            }
        }

        if (i == needle_len) {
            return (char *)haystack; // Match found
        }
    }

    return nullptr; // No match found
}

#endif