#include "funcs.h"
#include <stdio.h>
#include <string.h>

const char *contentTypes[2] = {
    [TEXT] = "text", [IMAGE] = "image"
};
const char *fileTypes[NUMFILETYPES] = {
    [FILE_DEFAULT] = "plain", [FILE_HTML] = "html", [FILE_JS] = "javascript",
    [FILE_CSS] = "css",       [FILE_PNG] = "png",   [FILE_BMP] = "bmp",
    [FILE_GIF] = "gif",       [FILE_JPG] = "jpg",
};

const char *fileExts[NUMFILETYPES] = {
    [FILE_DEFAULT] = "plain", [FILE_HTML] = ".html", [FILE_JS] = ".js",
    [FILE_CSS] = ".css",       [FILE_PNG] = ".png",   [FILE_BMP] = ".bmp",
    [FILE_GIF] = ".gif",       [FILE_JPG] = ".jpg",
};

char *getFileExtension(char *strStart, int maxLen)
{
    // Maybe refactor this one day
    int pos;
    int isValid = 0;

    for (pos = 0; pos <= maxLen; pos++) {
        if (*(strStart + pos) == '\0') {
            isValid = 1;
            break;
        }
    }

    if (isValid) {
        isValid = 0;
        // Walkback till we find a .
        while (pos > 0) {
            if (*(strStart + pos) == '.') {
                isValid = 1;
                break;
            }
            pos--;
        }
    }

    return (isValid == 0) ? NULL : (strStart + pos);
}

FileType getFileType(char *strStart, int maxLen)
{
    FileType fileType = FILE_DEFAULT;
    char *fileExt = getFileExtension(strStart, maxLen);

    for (int i = 1; i < NUMFILETYPES; i++) {
        if (strcmp(fileExt, fileExts[i]) == 0) {
            fileType = i;
            break;
        }
    }

    return fileType;
}

ContentType getContentType(FileType fileType) {
    // This is 
    int i = fileType - 3;

    return (i < 0) ? TEXT : IMAGE;
    
}
