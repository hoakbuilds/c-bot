#include "ccli.h"

/*
    Parse_ccli  -> particiona o comand introduzido na CCLI em argumentos para ser mais facil manipular
*/

int parse_ccli(char *buf, char **args){

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
        fprintf(stdout,"%s[CONSOLE COMMANDS]%s> Current Console/Client is not authenticated.\n", RED, RESET);
        fprintf(stdout,"%s[CONSOLE COMMANDS]%s> login [USER] [PASSWORD]\n", MAGENTA, RESET);
        //fprintf(stdout,"%s[CONSOLE COMMANDS]%s> \n", MAGENTA, RESET);
        return 1;
    }
    if (strcmp(args[0], "login") == 0 ){
        if(strcmp(args[1], "-t") == 0){
            fprintf(stdout,"%s[CONSOLE COMMANDS]%s> Trying to authenticate user %s for a timed session.\n", CYAN, RESET, args[2]);
            //fprintf(stdout,"%s[CONSOLE COMMANDS]%s> \n", MAGENTA, RESET);
            return 1;
        }
        fprintf(stdout,"%s[CONSOLE COMMANDS]%s> Trying to authenticate user %s.\n", CYAN, RESET, args[1]);
        //fprintf(stdout,"%s[CONSOLE COMMANDS]%s> \n", MAGENTA, RESET);
        return 1;
    }



    return 0;
}