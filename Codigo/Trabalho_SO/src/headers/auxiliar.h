#ifndef __AUXILIAR_H__
#define __AUXILIAR_H__

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <time.h>

typedef struct venda{
	int id;
	int quantidade;
	int valor;
}*Venda;

ssize_t readln (int fildes, void *buf, size_t nbyte);

char **strsplit(const char* str, const char* delim, size_t* numtokens);

int isDigit(char* number);

char* myconcat(char** lista, ssize_t n);


#endif
