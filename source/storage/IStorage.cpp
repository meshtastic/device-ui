#include "storage/IStorage.h"
#include <cstring>

bool IFile::readLine(char *buf, uint32_t len)
{
    if (!buf || len == 0)
        return false;
    uint32_t n = 0;
    bool any = false;
    char c;
    while (n + 1 < len && read(&c, 1) == 1) {
        any = true;
        if (c == '\n')
            break;
        buf[n++] = c;
    }
    buf[n] = '\0';
    return any;
}

bool IFile::print(const char *text)
{
    uint32_t len = text ? strlen(text) : 0;
    return len == 0 || write(text, len) == (int32_t)len;
}
