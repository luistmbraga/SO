#include "headers/auxiliar.h"

#define MAXLINE 50

//Para organização e menos repetição de código podemos criar uma função que retorne a referência para o nome do artigo
int insertString(char* path, char* nome){
	int referencia = 1;
	int fdStrings = open(path, O_RDWR);
	char c;
	
	while(read(fdStrings, &c, 1) && c!=EOF){
		if(c == '\n') referencia++;
	}
	
	write(fdStrings, nome, strlen(nome));
	write(fdStrings,"\n", 1);
	
	close(fdStrings);

	return referencia;
}

// retorna o id ou código do artigo
int insertArtigo(char* path, char* nome, char* preco){
	int status;
	int linha = 1;
	if(fork() == 0){
		if(fork() == 0){
			char* pathStocks = malloc(strlen(path) + strlen("/STOCKS") + 1);
			strcpy(pathStocks, path);
			strcat(pathStocks, "/STOCKS");
			int fdStocks = open(pathStocks, O_RDWR);
			char c;
			lseek(fdStocks, 0, SEEK_END);
			write(fdStocks, "0                                     \n", 39);

			close(fdStocks);
			_exit(1);
		}
		else{
				// filho insere o novo nome no ficheiro STRINGS
				char* pathStrings = malloc(strlen(path) + strlen("/STRINGS") + 1);	
				strcpy(pathStrings, path);
				strcat(pathStrings, "/STRINGS");
		
				int referencia = insertString(pathStrings, nome);

				wait(NULL);
				_exit(referencia);
		}
	}
	else{
		// pai insere o novo artigo no ficheiro ARTIGOS
		char* pathArtigos = malloc(strlen(path) + strlen("/ARTIGOS") + 1);
		strcpy(pathArtigos, path);
		strcat(pathArtigos, "/ARTIGOS");
		int fdArtigos = open(pathArtigos, O_RDWR);
		char c;
		// o wait estará que estar depois de descobrirmos a linha onde temos que inserir devido à referência do nome que irá receber do seu filho
		// procura a linha onde terá que inserir o novo artigo
		
		while(read(fdArtigos, &c, 1) && c!=EOF){
			if (c == '\n') linha++;
		}

		wait(&status);

		int ref = WEXITSTATUS(status);

		char novoartigo[100];
		sprintf(novoartigo, "%d", ref);
		strcat(novoartigo, " ");
		strcat(novoartigo, preco);
		//write(fdArtigos, novoartigo, strlen(novoartigo));
		int espacos = MAXLINE - strlen(novoartigo) - 1;
		for(int i = 0; i < espacos; i++){
		    strcat(novoartigo, " ");
		}

		strcat(novoartigo, "\n");

		write(fdArtigos, novoartigo, strlen(novoartigo));

		close(fdArtigos);
	}

	return linha;

}

void alteraNomeArtigo(char *path, int id, char* novonome){
	int status;
	int linha = 1;
	if(fork()==0){
	    // filho insere o novo nome no ficheiro STRINGS
		char* pathStrings = malloc(strlen(path) + strlen("/STRINGS") + 1);	
		strcpy(pathStrings, path);
		strcat(pathStrings, "/STRINGS");

		int referencia = insertString(pathStrings, novonome);

		_exit(referencia);
	}
	else{
	    // pai insere o novo artigo no ficheiro ARTIGOS
		char* pathArtigos = malloc(strlen(path) + strlen("/ARTIGOS") + 1);
		strcpy(pathArtigos, path);
		strcat(pathArtigos, "/ARTIGOS");
		int fdArtigos = open(pathArtigos, O_RDWR);
		//int fdArtigos2 = open(pathArtigos, O_RDONLY);
		// o wait estará que estar depois de descobrirmos a linha onde temos que inserir devido à referência do nome que irá receber do seu filho
		// procura a linha onde terá que alterar o nome do artigo
		char c;
		//char c2;
		  while(linha != id && read(fdArtigos, &c, 1) && /*(read(fdArtigos2, &c2, 1)) && c2 != EOF  && */c!=EOF){
			if(c == '\n') linha++;
		  }

		  wait(&status);

		  if(linha != id){
		     // caso nao exista
			 write(STDOUT_FILENO, "O artigo com esse código não existe!\n", 39);
			 return;
		  }
		  else{
			// caso exista
				char buffer[100];
				ssize_t size;
				
				if( (size = readln(fdArtigos, buffer, 100)) <= 0) {write(STDOUT_FILENO, "O artigo com esse código não existe!\n", 39); return;}
				
				int ref = WEXITSTATUS(status);
				char refchar[200];
				sprintf(refchar, "%d", ref);

				lseek(fdArtigos, -size-1, SEEK_CUR);
				
				size_t nstrings;
				strcat(refchar, " ");
				char** lista = strsplit(buffer, " ", &nstrings);
				strcat(refchar, lista[1]);
				//strcat(refchar, "\n");
				//lseek(fdArtigos2, size, SEEK_CUR);
				write(fdArtigos, refchar, strlen(refchar));
		  }

		close(fdArtigos);
	}

}

void alteraPrecoArtigo(char* path, int id, char* preco){
	//Aqui a string path recebida, já estará direcionada para o ficheiro ARTIGOS
	
	int linha = 1;
	int linhaAux = 1;

    char* pathArtigos = malloc(strlen(path) + strlen("/ARTIGOS") + 1);
	strcpy(pathArtigos, path);
	strcat(pathArtigos, "/ARTIGOS");
	int fdArtigos = open(pathArtigos, O_RDWR);


	char c;

	while(linha != id && read(fdArtigos, &c, 1) && c!=EOF){
		if(c == '\n') linha++;
	}

    if(linha != id){
		// caso nao exista
		write(STDOUT_FILENO, "O artigo com esse código não existe!\n", 39);
	}
	else{
		// caso exista
				char buffer[100];
				ssize_t size;
				if( (size = readln(fdArtigos, buffer, 100)) <= 0) {write(STDOUT_FILENO, "O artigo com esse código não existe!\n", 39); return;}
				
				char refchar[100];
			
				lseek(fdArtigos, -size-1, SEEK_CUR);
				
				size_t nstrings;
				char** lista = strsplit(buffer, " ", &nstrings);
				strcat(refchar, lista[0]);
				strcat(refchar, " ");
				strcat(refchar, preco);
				int espacos = strlen(lista[1]) - strlen(preco);
				for(int i = 0; i < espacos; i++){
					strcat(refchar, " ");
				}
				//lseek(fdArtigos2, size, SEEK_CUR);
				write(fdArtigos, refchar, strlen(refchar));
    }

	close(fdArtigos);
		
}

void showArtigo(char* path, int id){
	char *resultado = malloc(300);
	// para o ficheiro ARTIGOS
	char* pathArtigos = malloc(strlen(path) + strlen("/ARTIGOS") + 1);
	strcpy(pathArtigos, path);
	strcat(pathArtigos, "/ARTIGOS");
	int fdArtigos = open(pathArtigos, O_RDONLY);

	// para o ficheiro STRINGS
	char* pathStrings = malloc(strlen(path) + strlen("/STRINGS") + 1);	
	strcpy(pathStrings, path);
	strcat(pathStrings, "/STRINGS");
	int fdStrings = open(pathStrings, O_RDONLY);

	// percorre até encontrar o artigo com o respetivo id
	char c;
	int linha = 1;
	while(linha != id && read(fdArtigos, &c, 1) && c!=EOF){
		if (c == '\n') linha++;
	}

	

	if(linha != id){
		// caso não exista
		write(STDOUT_FILENO, "O artigo com esse código não existe!\n", 39);
	}
	else {
		//lseek(fdArtigos, -1, SEEK_CUR);
		char artigo[200];

		if (readln(fdArtigos, artigo , 200) <= 0) {write(STDOUT_FILENO, "O artigo com esse código não existe!\n", 39); return;}

		size_t nstrings;
		char** lista = strsplit(artigo, " ", &nstrings);
		int novalinha = atoi(lista[0]);
		linha = 1;

		while(linha != novalinha && read(fdStrings, &c, 1) && c!=EOF){
			if(c == '\n') linha++;
		}

		char nome[100];
		readln(fdStrings, nome, 100);

		strcpy(resultado, "Nome:  ");
		strcat(resultado, nome);
		strcat(resultado, "\nPreço:  ");
		strcat(resultado, lista[1]);
		strcat(resultado, "\n");

		write(STDOUT_FILENO, resultado, strlen(resultado));
	}
	
	close(fdStrings);
	close(fdArtigos);

}


int main(int argc, char* argv[]){
	char byte[200];
	ssize_t size;	
	size_t nstrings;

	while((size = readln(STDIN_FILENO, byte, 200)) > 0){

		char** lista = strsplit(byte, " ", &nstrings);
		char retorno[200];

		if(!strcmp(lista[0], "i")){
  		 	if(nstrings > 2 && isDigit(lista[nstrings-1])){
  		 		char* s = strdup(myconcat(lista+1, nstrings-3));
  		 		
  		 		int id = insertArtigo("../files", s, lista[nstrings-1]);

  		 		sprintf(retorno, "%d", id);
  		 		strcat(retorno,"\n");
  		 		write(STDOUT_FILENO, retorno, strlen(retorno));

  		 		free(s);
  		 	}
  		 	else{
  		 		write(STDOUT_FILENO,"Argumentos insuficientes ou o preço não é um número!\n", 57);
  		 	}
		}
		// altera o nome do artigo
		else if(!strcmp(lista[0], "n")){
			if(nstrings > 2 && isDigit(lista[1])){
				char* s = strdup(myconcat(lista+2, nstrings-3));
				alteraNomeArtigo("../files", atoi(lista[1]), s);
				free(s);
			}
			else{ write(STDOUT_FILENO, "Argumentos insuficientes ou o código não é um digito!\n", 57); }
		}
		// altera o preço do artigo
	    else if(!strcmp(lista[0], "p")){
	    	if(nstrings == 3){
	    		if(isDigit(lista[1])){
	    			if(isDigit(lista[2])){
	    				
	    				alteraPrecoArtigo("../files", atoi(lista[1]), lista[2]);
	    			}
	    			else{
	    				write(STDOUT_FILENO, "O novo preço do artigo não é um digito!\n", 43);
	    			}
	    		}
	    		else{ write(STDOUT_FILENO, "O código do artigo não é um digito!\n", 39);}
	    	}
	    	else {write(STDOUT_FILENO, "Argumentos inválidos!\n", 23);}
		}

		//mostra o nome e o preço do artigo
		else if(!strcmp(lista[0], "s") && nstrings == 2){
			if(isDigit(lista[1])){
				 showArtigo("../files", atoi(lista[1]));
			}
			else {write(STDOUT_FILENO, "O código do artigo não é um digito!\n", 39);}
		}
	    else{
			write(STDOUT_FILENO,"Não executou nenhum comando válido!\n", 38);
		}
			 
	}

	return 0;
}