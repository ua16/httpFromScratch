#include "funcs.h"

#define NULL ((void *)0)

char *getFileExtension(char *strStart, int maxLen)
{
    // Maybe refactor this one day
    int pos;
    int isValid = 0;

    for (pos = 0; pos < maxLen; pos++) {
        if (*(strStart + pos) == '\0') {
            isValid = 1;
            break;
        }
    }

    if (isValid) {
        isValid = 0;
        // Walkback till we find a .
        while (pos != 0) {
            if (*(strStart + pos) == '.') {
                isValid = 1;
                break;
            }
        }
    }

    return (isValid == 0) ? NULL : (strStart + pos);
}
