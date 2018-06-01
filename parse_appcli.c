#include "appcli.h"

/*
    Parse_ccli  -> particiona o comand introduzido na CCLI em argumentos para ser mais facil manipular
*/

int parse_appcli(char *buf, char **args){

    int counter = 0;
    while('\0' != *buf){
        /* strip whitespace. Usa um NULL para indicar que o argumento anterior e. o ultimo */
        while (isspace((unsigned char)*buf)) *buf++ = '\0';

        *args++ = buf;		/* salvaguarda argumento */
        counter++;
        /* salta sobre o argumento */
        while( (*buf != '\0') && (!isspace( (unsigned char)*buf ) ) )	buf++;
    }
    *args = (char *) NULL;	/* o ultimo argumento e. NULL */
    return counter;
}

int cmd (int numargs, char **args){
    if (strcmp(args[0], "help") == 0 ){
		fprintf(stdout, "\nHow to use %sc-bot%s\n\n",GREEN_BOLD,RESET);
        fprintf(stdout, "%strack {ID}              %s\n",CYAN,RESET);
        fprintf(stdout, "Adds cryptocurrency with specified ID to bot's tracklist.\n\n"RESET);
        fprintf(stdout, "%stop 			    %s\n",CYAN,RESET);
        fprintf(stdout, "Displays Top 100 cryptocurrencies based on Market Capitalization.\n\n");
        fprintf(stdout, "%sinfo				%s\n",CYAN,RESET);
        fprintf(stdout, "Displays Top 100 cryptocurrencies based on Market Capitalization.\n\n");
        return 1;
	}
    if (strcmp(args[0], "track") == 0 ){
		fprintf(stdout, "%s{%s ~ c-bot}%s>ololafafaoll\n",CYAN,"timestring", RESET);
		return 1;
	}
    if (strcmp(args[0], "top") == 0 ){
		fprintf(stdout, "%s{%s ~ c-bot}%s>ololafafaoll\n",CYAN,"timestring", RESET);
		
		return 1;
	}
    if (strcmp(args[0], "info") == 0 ){
		fprintf(stdout, "%s{%s ~ c-bot}%s>oloafaflol\n",CYAN,"timestring", RESET);
		return 1;
	}
    return 0;
}