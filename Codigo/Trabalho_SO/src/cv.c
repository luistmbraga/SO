#include "headers/auxiliar.h"

int main(int argc, char* argv[]){
	char byte[200];
	ssize_t size;	
	size_t nstrings;
	//int fifo = mkfifo("inputCliente", 0666);
	
	char pidchar[50];
	char output[200];
	sprintf(pidchar, "%d", getpid());
	strcpy(output, "C");
	strcat(output, pidchar);
	int fifo = mkfifo(output, 0600);
	//int fdInput = open("inputCliente", O_WRONLY);
	//char idfifo[60];
	//sprintf(idfifo, "%d", fifo);
	//write(STDOUT_FILENO, idfifo, strlen(idfifo)+1);
	//int fdOutput = open(output, O_RDONLY);
	//int fdInput = open("inputCliente", O_WRONLY);
	while((size = readln(STDIN_FILENO, byte, 200)) > 0){

		char** lista = strsplit(byte, " ", &nstrings);
		//char retorno[200];
		char aux[200];
		sprintf(aux, "%ld", nstrings);
		//write(aux, 3)
		//write(STDOUT_FILENO, byte, strlen(byte)+1);
		
		if(nstrings <= 2 && nstrings > 0 && isDigit(lista[0])){
			if(nstrings == 2 && isDigit(lista[1])){
					
					char input[260];
					strcpy(input, pidchar);
					strcat(input, " ");
					strcat(input, byte);
					strcat(input, "\n");
					//write(STDOUT_FILENO, input, strlen(input)+1);
					int fdInput = open("inputCliente", O_WRONLY);
					write(fdInput, input, strlen(input)+1);
					close(fdInput);
					//write(STDOUT_FILENO, input, strlen(input)+1);
					char resultado[100];

					int fdOutput = open(output, O_RDONLY);
					readln(fdOutput, resultado, 100);
					close(fdOutput);
					strcat(resultado,"\n");
					write(STDOUT_FILENO, resultado, strlen(resultado));
					//close(fdInput);
			}
			else if(nstrings == 1){
				    //int fdInput = open("inputCliente", O_WRONLY);
					char input[260];
					strcpy(input, pidchar);
					strcat(input, " ");
					strcat(input, byte);
					strcat(input, "\n");
					int fdInput = open("inputCliente", O_WRONLY);
					write(fdInput, input, strlen(input)+1);
					close(fdInput);

					char resultado[100];
					int fdOutput = open(output, O_RDONLY);
					readln(fdOutput, resultado, 100);
					close(fdOutput);
					//readln(fdOutput, resultado, 200);
					strcat(resultado,"\n");
					write(STDOUT_FILENO, resultado, strlen(resultado));
					//close(fdInput);
			}
			else {write(STDOUT_FILENO, "Comando inválido!\n", 19);}
		} 
		else {write(STDOUT_FILENO, "Comando inválido!\n", 19);}
		//fsync(fdInput);
	}

	return 0;
}