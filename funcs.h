#ifndef FUNC_H
#define FUNC_H

typedef enum { TEXT, IMAGE } ContentType;

#define NUMFILETYPES 10
typedef enum {
    FILE_DEFAULT,
    FILE_HTML,
    FILE_JS,
    FILE_CSS,
    FILE_PNG,
    FILE_BMP,
    FILE_GIF,
    FILE_JPG
} FileType;

const char *fileTypes[NUMFILETYPES] = {
    [FILE_DEFAULT] = "plain", [FILE_HTML] = "html", [FILE_JS] = "javascript",
    [FILE_CSS] = "css",       [FILE_PNG] = "png",   [FILE_BMP] = "bmp",
    [FILE_GIF] = "gif",       [FILE_JPG] = "jpg",
};

char *getFileExtension(char *strStart,
                       int maxLen); // Get the file extension of a string
FileType *getFileType(char *strStart, int maxLen); // A wrapper for the above

#endif // !FUNC_H
