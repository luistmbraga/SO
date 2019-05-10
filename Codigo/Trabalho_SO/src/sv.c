
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
	//char idchar[200];
	//sprintf(idchar,"%d", id);
	//int fifo = mkfifo(pathStocks, 0666);
	int fdStocks = open(pathStocks, O_RDWR);
	//int fdStocksWrite = open(pathStocks, O_WRONLY)
	int linha = 1;
	int resultado;
	char aux[200];

	while(linha!=id && readln(fdStocks, aux, 200)){
		linha++;
	}

	if(linha != id){
		// caso não exista, retorna -2
		//close(fdStocksRead);
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

int quantasVendas(char* pathVendas){
	int fdVendas = open(pathVendas, O_RDONLY);
	char c;
	int linhas = 0;

	while(read(fdVendas, &c, 1) && c!= EOF){
		if(c == '\n') linhas++;
	}

	close(fdVendas);

	return linhas;
}


/*
void aux_agregar(char* path, int linhainicial, int linhafinal, int linhaM){


}

void agregacao_concorrente(char* path, int linhainicial){
	char vendas[100];
	strcpy(vendas, path);
	strcat(vendas, "/VENDAS");
	int total_linhas = quantasVendas(path);
	int i, j;
	//int linhas_matriz = (total_linhas-linhainicial)/400;
	int 
	for(int i = linha_inicial; i != total_linhas; ){
		j = i + 400;
		if(j > total_linhas) j = total_linhas;

	}


}
*/

char* agregar(char* path){
	char historico[100];
	strcpy(historico, path);
	strcat(historico, "/Historico_Ag");
	int linha_inicial = 1;
	int fdHist;
	//Verificar que já existe um ficheiro histórico, ou seja, que já foram realizadas agregações
	if((fdHist = open(historico, O_RDWR)) == -1){
		fdHist = open(historico, O_RDWR | O_CREAT);
	}
	else{
		// caso exista, lê a linha final da ultima agregacao
		char aux = malloc(100);
		while ( ((size = readln(fdHist, aux, 100)) > 0));
		lseek(fdHist, -size-1, SEEK_CUR);
		readln(fdHist, aux, 100);
		int fdUltimaAg = open(aux, O_RDONLY);
		readln(fdUltimaAg, aux, 100);
		linha_inicial = atoi(aux);
		free(aux);
	}
	int stock_artigos[100];
	char vendas[100];
	strcpy(vendas, path);
	strcat(vendas, "/VENDAS");
	int arraysize = fazAgregacao(vendas, stock_artigos, linha_inicial);
	while(readln(fdUltimaAg, vendas, 100) > 0){
		 ssize_t  nstrings;
		 char** lista = strsplit(vendas, ",", &nstrings);
		 int idArtigo = atoi(lista[0]);
		 if(idArtigo < arraysize){
		 	   
		 }

	}

}


int main(int argc, char* argv[]){
	int fifo = mkfifo("inputCliente", 0600);
	int pid_pai = getpid();
	//atualiza_Stock("../files", 3, 1000);
	//char input[200];
 

	//char* namefifo = "inputCliente";
	
		ssize_t size;
		while(1){
			//int sair = prctl(PR_SET_PDEATHSIG, SIGTERM);
			//if(getppid() != pid_pai) _exit(0);
			//printf("%d %d \n", getppid(), pid_pai);
			// receber input do cliente de vendas
			//printf("o caralho!\n");
			int fdCliente = open("inputCliente", O_RDONLY);
			char input[200];
			if((size = readln(fdCliente, input, 200))>0){

				if(fork() == 0){
				char aux[200];
				strcpy(aux, input);
				write(STDOUT_FILENO, aux, strlen(aux) + 1);
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
					//write(p_id[1], lista[0], strlen(lista[0]) + 1);
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
					//write(p_id[1], lista[0], strlen(lista[0]) + 1);
				}
				close(fdOutput);
				_exit(0);
			}
			wait(NULL);
		}
			close(fdCliente);
		}
		//close(fdCliente);
	/*	//_exit(0);
	}
	else{
		
		while(1){
			// enviar resultados para o respetivo cliente
			close(p_id[1]);
			close(presultados[1]);
			char output[200];
			char cliente[200];
			strcpy(cliente, "C");
			if(read(p_id[0], output, 200)>0){
				strcat(cliente, output);
				write(STDOUT_FILENO, cliente, strlen(cliente));
				/*int fdOutuput = open(cliente, O_WRONLY);
				if(read(presultados[0], output, 200)){
					write(fdOutuput, output, 200);
				}
				close(fdOutuput); 
			}
		}
		wait(NULL);
	} */
 
	
	return 0;
}