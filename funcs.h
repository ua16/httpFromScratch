#ifndef FUNC_H
#define FUNC_H


typedef ContentType { TEXT, IMAGE,  }

char * getFileExtension(char * strStart, int maxLen); // Get the file extension of a string
char * getFileType(char * strStart, int maxLen); // A wrapper for the above

#endif // !FUNC_H
