
#include "headers/auxiliar.h"


ssize_t readln (int fildes, void *buf, size_t nbyte){
	ssize_t nbytes = 0;
	int n;
	char c;
	char *buffer = (char *)buf;

	while(nbytes < nbyte && (n = read(fildes, &c, 1)) >0 && c!='\n')
		buffer[nbytes++] = c;

	(nbytes < nbyte) ? (buffer[nbytes] = '\0') : (buffer[nbytes - 1] = '\0');

	return nbytes;
}


char **strsplit(const char* str, const char* delim, size_t* numtokens) {
    // copy the original string so that we don't overwrite parts of it
    // (don't do this if you don't need to keep the old line,
    // as this is less efficient)
    char *s = strdup(str);
    // these three variables are part of a very common idiom to
    // implement a dynamically-growing array
    size_t tokens_alloc = 1;
    size_t tokens_used = 0;
    char **tokens = calloc(tokens_alloc, sizeof(char*));
    char *token, *strtok_ctx;
    for (token = strtok_r(s, delim, &strtok_ctx);
            token != NULL;
            token = strtok_r(NULL, delim, &strtok_ctx)) {
        // check if we need to allocate more space for tokens
        if (tokens_used == tokens_alloc) {
            tokens_alloc *= 2;
            tokens = realloc(tokens, tokens_alloc * sizeof(char*));
        }
        tokens[tokens_used++] = strdup(token);
    }
    // cleanup
    if (tokens_used == 0) {
        free(tokens);
        tokens = NULL;
    } else {
        tokens = realloc(tokens, tokens_used * sizeof(char*));
    }
    *numtokens = tokens_used;
    free(s);
    return tokens;
}

int isDigit(char* number){
    int s = strlen(number);
    for(int i = 0; i<s; i++){
       if(!isdigit(number[i])) return 0;
    }

    return 1;
}

char* myconcat(char** lista, ssize_t n){
    int totalsize = 0;
    ssize_t i = 0;
    
    for(i = 0; i <= n; i++){
        totalsize += strlen(lista[i]);
    }

    char* s = malloc(totalsize + 1 + (i-1)*1) ;
    s = strcpy(s, lista[0]); 

    for(i = 1; i <= n; i++){
        strcat(s, " ");
        strcat(s, lista[i]);
    }

    return s;
}