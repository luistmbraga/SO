#include "headers/auxiliar.h"


// quando chamamos esta função, tem-se que ter a certeza que o artigo existe (retorna o preço do artigo pretendido)
int verPrecoArtigo(char* path, int id){
	int fdArtigos = open(path, O_RDONLY);
	int linha = 1;
	char buffer[200];
	char c;

	// percorrer até chegar ao id correto
	while(linha != id && readln(fdArtigos, buffer, 200)) linha++;

	// ir buscar a linha relativa à informação que queremos do artigo pedido
	readln(fdArtigos, buffer, 200);
	size_t numStrings;
	char** lista = strsplit(buffer, " ", &numStrings);

	return atoi(lista[1]);
}

// regista a venda efetuada no ficheiro VENDAS
void regista_Venda(char* path, int id, int stock){
	int status;

	if(fork() == 0){
		// o filho tratará de ir buscar o preço do artigo
		char* pathArtigo = malloc(strlen(path) + strlen("/ARTIGOS") + 1);
		strcpy(pathArtigo, path);
		strcat(pathArtigo, "/ARTIGOS");

		int preco = verPrecoArtigo(pathArtigo, id);

		_exit(preco);
	}
	else{
		// o pai tratará de ir para o fim do ficheiro e esperar pelo preço do artigo
		char* pathVendas = malloc(strlen(path) + strlen("/VENDAS") + 1);
		strcpy(pathVendas, path);
		strcat(pathVendas, "/VENDAS");
		int fdVendas = open(pathVendas, O_RDWR);

		lseek(fdVendas, 0, SEEK_END);
		wait(&status);


		int montantetotal = WEXITSTATUS(status)*(-stock);
		char precochar[30];
		sprintf(precochar, "%d", montantetotal);
		char stockchar[30];
		sprintf(stockchar, "%d", -stock);
		char idchar[120];
		sprintf(idchar, "%d", id);
		strcat(idchar, ",");
		strcat(idchar, stockchar);
		strcat(idchar, ",");
		strcat(idchar, precochar);
		strcat(idchar, "\n");

		write(fdVendas, idchar, strlen(idchar));

		close(fdVendas);

	}
}

// funcao responsavel por mostrar o preço e o stock disponível de um dado artigo
char* mostra_SP(char* path, int id){
	int status;

	char* pathStocks = malloc(strlen(path) + strlen("/STOCKS") + 1);
	strcpy(pathStocks, path);
	strcat(pathStocks, "/STOCKS");
	int fdStocks = open(pathStocks, O_RDWR);
	int linha = 1;
	char buffer[100];
	char* resultado;


	while(linha != id && readln(fdStocks, buffer, 200)){
		linha++;
	}

	if(linha != id) {return NULL;}
	ssize_t size;
	if((size = readln(fdStocks, buffer, 200))<=0) return NULL;


	char* pathArtigo = malloc(strlen(path) + strlen("/ARTIGOS") + 1);
	strcpy(pathArtigo, path);
	strcat(pathArtigo, "/ARTIGOS");

	int preco = verPrecoArtigo(pathArtigo, id);
	char precochar[30];
	sprintf(precochar, "%d", preco);

	resultado = malloc(strlen("Stock:  ") + strlen(buffer) + 1 + strlen("Preço:  ") + strlen(precochar) + 2);
	strcpy(resultado, "Stock:  ");
	strcat(resultado, buffer);
	strcat(resultado, "\n");
	strcat(resultado, "Preço:  ");
	strcat(resultado, precochar);

	return resultado;
}


// função que dado um caminho para o ficheiro, um código de artigo e um dado stock a adicionar ou remover, retorna o stock atual, após a operação
int atualiza_Stock(char* path, int id, int stock){
	char* pathStocks = malloc(strlen(path) + strlen("/STOCKS") + 1);
	strcpy(pathStocks, path);
	strcat(pathStocks, "/STOCKS");

	int fdStocks = open(pathStocks, O_RDWR);
	
	int linha = 1;
	int resultado;
	char aux[200];

	while(linha!=id && readln(fdStocks, aux, 200)){
		linha++;
	}

	if(linha != id){
		// caso não exista, retorna -2
		close(fdStocks);
		return -2;
	}
	else{
		ssize_t size;
		if((size = readln(fdStocks, aux, 200)) <= 0) return -2;

		// caso exista
		int stockatual = atoi(aux);

		// retorna -1 caso o stock seja insuficiente (quando se realizam vendas)
		resultado = stockatual + stock;
		if(resultado < 0) {close(fdStocks); return -1;}

		// significa que se trata de uma venda de stock
		if(stock < 0) regista_Venda(path, id, stock);

		char charstock[40];
		sprintf(charstock, "%d", resultado);

		lseek(fdStocks, -size-1, SEEK_CUR);
		
		int espaco = strlen(aux) - strlen(charstock);

		write(fdStocks, charstock, strlen(charstock));

	    for(int j = 0; j < espaco; j++){
			write(fdStocks, " ", 1);
		}

		close(fdStocks);

		return resultado;
	}
}



int main(int argc, char* argv[]){
	if (fork() == 0){
		int fifo = mkfifo("Agregar", 0600);
		while(1){
			int fdAgregar = open("Agregar", 0600);
			char* aux = malloc(100);
			if(readln(fdAgregar, aux, 100)){

		
				// chamar a função que agrega
				if(fork() == 0){
					execv("ag", NULL);
					_exit(0);
				}
				wait(NULL);
			}
			close(fdAgregar);
		}
		_exit(0);
	}
	else{
		int fifo = mkfifo("inputCliente", 0600);
		ssize_t size;
		while(1){
			int fdCliente = open("inputCliente", O_RDONLY);
			char input[200];
			if((size = readln(fdCliente, input, 200))>0){

				if(fork() == 0){
				char aux[200];
				strcpy(aux, input);
				size_t numStrings;
				char** lista = strsplit(aux, " ", &numStrings);
				char fichOutput[100] = "C";
				strcat(fichOutput, lista[0]);
				int fdOutput = open(fichOutput, O_WRONLY); 
				if(numStrings == 2){
					char* resultado = mostra_SP("../files", atoi(lista[1]));
					if(resultado == NULL){
						write(fdOutput, "Não existe esse artigo!", 24);
					}
					else{
						write(fdOutput, resultado, strlen(resultado) + 1);	
					}
				}
				else if(numStrings == 3){
					int resultado = atualiza_Stock("../files", atoi(lista[1]), atoi(lista[2]));
					if(resultado >= 0){
						char auxres[100];
						sprintf(auxres, "%d", resultado);
						write(fdOutput, auxres, strlen(auxres) + 1);
					}
					else{
						if(resultado == -2){
						write(fdOutput, "Não existe esse artigo!", 24);
						}
						else{
						write(fdOutput, "Não há stock suficiente!", 25);
						}
					}
				}
				close(fdOutput);
				_exit(0);
			}
			wait(NULL);
		}
			close(fdCliente);
		}
	}
 
	
	return 0;
}