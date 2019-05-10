typedef struct venda{
	int quantidade;
	int valor;
} *Venda;

#define Venda[1000]
#define tamanho = 1000

char* agregar(char* path){
	char historico[100];
	strcpy(historico, path);
	strcat(historico, "/Historico_Ag");
	int linha_inicial = 1;
	int fdHist;
	int fdUltimaAg = -1;
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
	    fdUltimaAg = open(aux, O_RDONLY);
		readln(fdUltimaAg, aux, 100);
		linha_inicial = atoi(aux);
		free(aux);
	}
	int stock_artigos[100];
	char vendas[100];
	strcpy(vendas, path);
	strcat(vendas, "/VENDAS");
	int fdVendas = open(vendas, O_RDONLY);
	char v[100];
	int linha = 1;
	while(linha!=linha_inicial && readln(fdVendas, v, 100) > 0 ) linha++;
	while(readln(fdVendas, v, 100)){
		size_t nstrings;
		char** lista = strsplit(v, ",", &nstrings);
		if(nstrings == 3){
			 int idArtigo = atoi(lista[0]);
			 if(idArtigo > tamanho){
			 	int novo_tamanho = 2*idArtigo;
			 	
			 	for(int i = tamanho; i < )

			 }
		}

	}

	/*
	int arraysize = fazAgregacao(vendas, stock_artigos, linha_inicial);
	while(readln(fdUltimaAg, vendas, 100) > 0){
		 ssize_t  nstrings;
		 char** lista = strsplit(vendas, ",", &nstrings);
		 int idArtigo = atoi(lista[0]);
		 if(idArtigo < arraysize){
		 	   
		 }

	}
	*/

}
