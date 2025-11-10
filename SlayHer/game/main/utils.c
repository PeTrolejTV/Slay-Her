#include "utils.h"
#include <stdio.h>
#include <unistd.h>
#include <conio.h>
#include <ctype.h>
#include <stdarg.h>

void formatLogMessage(char *buffer, size_t bufferSize, const char *format, ...) {
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, bufferSize, format, args);
    va_end(args);
}

void logToFile(const char *message) {
    FILE *logFile = fopen("log.txt", "a");
    if (logFile) {
        fprintf(logFile, "%s\n", message);
        fclose(logFile);
    } else {
        fprintf(stderr, "Failed to open log file.\n");
    }
}

void typeWriterEffect(const char *text) {
    int skipTypewriter = 0;

    for (int i = 0; text[i] != '\0'; i++) {
        if (skipTypewriter) {
            printf("%s", text + i);
            break;
        }

        putchar(text[i]);
        fflush(stdout);
        usleep(100000);

        if (_kbhit()) {
            const unsigned char ch = _getch();
            if (ch == '\r') {
                skipTypewriter = 1;
            }
        }
    }
    fflush(stdout);
    printf("\n");
}

void typeWriterEffectLine(const char *text) {
    printf("%s\n", text);
    fflush(stdout);
}

int strCaseInsensitiveCompare(const char *str1, const char *str2) {
    int result = 0;
    while (*str1 && *str2) {
        if (tolower((unsigned char)*str1) != tolower((unsigned char)*str2)) {
            result = 1;
            break;
        }
        str1++;
        str2++;
    }
    if (result == 0 && (*str1 != '\0' || *str2 != '\0')) {
        result = 1;
    }

    char logMessage[256];
    snprintf(logMessage, sizeof(logMessage), "Comparing '%s' and '%s' (case-insensitive). Result: %s", str1, str2, result == 0 ? "Match" : "No Match");
    logToFile(logMessage);

    return result;
}