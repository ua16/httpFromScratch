#ifndef FUNC_H
#define FUNC_H

typedef enum { TEXT, IMAGE } ContentType;

#define NUMFILETYPES 8
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

extern const char *contentTypes[2];
extern const char *fileTypes[NUMFILETYPES];
extern const char *fileExts[NUMFILETYPES];


char *getFileExtension(char *strStart,
                       int maxLen); // Get the file extension of a string
FileType getFileType(char *strStart, int maxLen); // A wrapper for the above
ContentType getContentType(FileType fileType);

#endif // !FUNC_H
