#ifndef ADM_quota_h
#define ADM_quota_h

#include <stdio.h>
#include <unistd.h>
#include "config.h"

/* qfopen stands for quota-fopen() */
FILE *qfopen(const char *, const char *);

/* qfprintf stands for quota-fprintf() */
void qfprintf(FILE *, const char *, ...);
size_t qfwrite(const void *ptr, size_t size, size_t  nmemb, FILE *stream);
ssize_t qwrite(int fd, const void *buf, size_t numbytes);

/* qfclose stands for quota-fclose() */
int qfclose(FILE *);

#ifdef USE_LIBXML2
#include <libxml/tree.h>
int qxmlSaveFormatFile(const char *filename, xmlDocPtr cur, int format);
#endif

#endif
