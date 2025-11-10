#ifndef TYPEWRITER_H
#define TYPEWRITER_H

#include <stddef.h>

void typeWriterEffect(const char *text);
void typeWriterEffectLine(const char *text);
int strCaseInsensitiveCompare(const char *str1, const char *str2);
void logToFile(const char *message);
void formatLogMessage(char *buffer, size_t bufferSize, const char *format, ...);

#endif