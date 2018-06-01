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
