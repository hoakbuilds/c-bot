#include "ccli.h"

int main(int argc, char **argv){

    char buffer[MAXLEN] /*input*/, *args[64] /*partitioned input*/;
    int outbound=0, inbound=0, protocol=0, protocolrt=0, len = 0, numargs=0, auth=0;
	
    if( (  outbound = open(FIFOWRITE , O_WRONLY)  < 0 ) 
    || ( ( inbound = open(FIFOREAD, O_RDONLY) ) < 0)
    || ( ( protocol = open(FIFOREAD, O_WRONLY) ) < 0)
    || ( ( protocolrt = open(FIFOREAD, O_RDONLY) ) < 0) ){
        fprintf(stderr,"%s[CONNECTION:ERROR]%s Could not estabilish connection to server.\n",RED,RESET);
        sleep(2);
        fprintf(stderr,"%s[CONNECTION:ERROR]%s Could not estabilish connection to server..\n",RED,RESET);
        sleep(2);
        fprintf(stderr,"%s[CONNECTION:ERROR]%s Could not estabilish connection to server...\n",RED,RESET);
        sleep(2);
        return main(argc, argv);
    }else{

        fprintf(stdout,"\n    		>> Welcome to the Console Application <<    \n");
        fprintf(stdout,"%s[CONSOLE]%s> Server is online, waiting for a command. Type 'help' for commands.\n",CYAN, RESET);

        if(  ( strcmp(args[1], "-t") ==0 ) && args[2] != NULL && args[3] != NULL ){
            if(strlen(args[2])>25 || strlen(args[3]) > 256){
                 fprintf(stdout,"%s[CONSOLE]%s> Username or password sent via arguments are too long. [USR:25] [PWD:256]\n", CYAN, RESET);
            }else{
                
            }

        }else if( args[1] != NULL && args[2] != NULL){
            if(strlen(args[1])>25 || strlen(args[2]) > 256){
                fprintf(stdout,"%s[CONSOLE]%s> Username or password sent via arguments are too long. [USR:25] [PWD:256]\n", CYAN, RESET);
            }else{

            }
            
        }

        while(1){

            fprintf(stdout,"%s[CONSOLE]%s>", CYAN, RESET);

            if ( fgets(buffer,1023,stdin) == NULL){
                continue;
	        }

            if( (len = strlen(buffer)) == 1) continue;

            if( buffer[len-1] == '\n' ) buffer[len-1] = '\0';

            numargs = parse_ccli(buffer, args);

            if ( !cmd(numargs, args, auth) ){
                fprintf(stdout,"%s[CONSOLE]%s> That is not an available command. Type 'help' for a list of available commands.\n", CYAN, RESET);
                fprintf(stdout,"%s[CONSOLE]%s>", CYAN, RESET);
            }

        }

    }

    return 0;
    
}