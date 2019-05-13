#include "headers/auxiliar.h"

int quantas(char* path){
	int fd = open(path, O_RDONLY);
	char c;
	int linhas = 0;

	while(read(fd, &c, 1) && c!= EOF){
		if(c == '\n') linhas++;
	}

	close(fd);

	return linhas;
}

void criar_espaco(char* path, int linhascriadas, int novalinhas){

	int linha = linhascriadas;
	int fdAg = open(path, O_RDWR);
	Venda v;
	v = malloc(sizeof(struct venda));
	int size = sizeof(struct venda);
	lseek(fdAg, size*(linhascriadas), SEEK_SET);
	while(linha < novalinhas){
		v->id = linha+1;
		v->quantidade = v->valor = 0;
		write(fdAg, v, size);
		linha++;
	}
	free(v);
	close(fdAg);
}

int quantasA(char* path){
	int fd = open(path, O_RDONLY);
	int nr = 0;
	Venda v = malloc(sizeof(struct venda));
	while(read(fd, v, sizeof(struct venda))>0) nr++;
	return nr;
}


void agregaArtigo(char* path, int idArtigo, int quantidade, int valor){
	int fdAg = open(path, O_RDWR);
	lseek(fdAg, sizeof(struct venda) * (idArtigo - 1), SEEK_SET);
	Venda v;
	v = malloc(sizeof(struct venda));
	read(fdAg, v, sizeof(struct venda));
	v->quantidade += quantidade;
	v->valor += valor;
	lseek(fdAg, -sizeof(struct venda), SEEK_CUR);
	write(fdAg, v, sizeof(struct venda));
	free(v);
	close(fdAg);

}




char* agregar(char* path){
	char historico[100];
	strcpy(historico, path);
	strcat(historico, "/Historico_Ag");
	int linha_inicial = 0;
	int fdHist;
	//criar nome do ficheiro
	char* agregacao_aux = malloc(150);
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char nomeFich[100];
	sprintf(nomeFich,"%d-%d-%dT%d:%d:%d", tm.tm_year + 1900, tm.tm_mon +1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	strcpy(agregacao_aux, path);
	strcat(agregacao_aux, "/");
	strcat(agregacao_aux, nomeFich);
	//Verificar que já existe um ficheiro histórico, ou seja, que já foram realizadas agregações
	if((fdHist = open(historico, O_RDWR)) == -1){
		fdHist = open(historico, O_RDWR | O_CREAT);
		if(fork() == 0) {execlp("touch", "touch", agregacao_aux, NULL); _exit(0);}
		wait(NULL);
	}
	else{
		// caso exista, lê a linha final da ultima agregacao
		char aux[100];
		size_t size, sizeaux;
		while ( ((size = readln(fdHist, aux, 100)) > 0)) sizeaux = size;
		lseek(fdHist, -sizeaux-2, SEEK_CUR);
		size_t lsj = readln(fdHist, aux, 100);
		size_t n;
		char** auxl = strsplit(aux, ",", &n);
		linha_inicial = atoi(auxl[1]);
		char filesource[100];
		strcpy(filesource, path);
		strcat(filesource, "/");
		strcat(filesource, auxl[0]);
		if(fork()==0){execlp("cp", "cp", filesource, agregacao_aux, NULL); _exit(0);}
		wait(NULL);
	}
	char vendas[100];
	strcpy(vendas, path);
	strcat(vendas, "/VENDAS");
	int fdVendas = open(vendas, O_RDONLY);
	write(fdHist, nomeFich, strlen(nomeFich));
	write(fdHist, ",", 1);
	char* linhaslidas = malloc(100);
	sprintf(linhaslidas, "%d\n", quantas(vendas));
	write(fdHist, linhaslidas, strlen(linhaslidas));
	write(fdHist, "\n", 1);
	close(fdHist);
	char venda[150];
	int linha = 0;
	while(linha != linha_inicial && readln(fdVendas, venda, 150) > 0) linha++;
	linha = 0;
	int linhaQ = quantasA(agregacao_aux);

	while(readln(fdVendas, venda, 150) > 0){
		 ssize_t  nstrings;
		 char** lista = strsplit(venda, ",", &nstrings);
		 int idArtigo = atoi(lista[0]);
		 if(idArtigo >= linhaQ){
		 	criar_espaco(agregacao_aux, linhaQ, idArtigo);
		 	//linha++;
		 	linhaQ = idArtigo;
		 }
		 agregaArtigo(agregacao_aux, idArtigo, atoi(lista[1]), atoi(lista[2]));
		 	
	}

	close(fdVendas);

    return agregacao_aux;

}


int main(int argc, char* argv[]){
	char c[100];
	strcpy(c, agregar("../files"));
	strcat(c, "\n");
	
	int fd = open("ResultadoAgregar", O_WRONLY);
	write(fd, c, strlen(c));
	close(fd);

	return 0;
}