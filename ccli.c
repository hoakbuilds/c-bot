#include "ccli.h"

int main(int argc, char **argv){

    char buffer[MAXLEN] /*input*/, *args[64] /*partitioned input*/;
    int outbound=0, inbound=0, protocol=0, protocolrt=0, len = 0, numargs=0, auth=0;
    char password[256]={0}, username[25]={0};
	
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

        if(  argc > 3 ){    /* verifica se password e username estao bem colocados juntamente com os argumentos */
            if(strlen(argv[2])<=12 || strlen(argv[3]) <= 12){
                fprintf(stdout,"%s[CONSOLE]%s> Wrong init arguments. If trying to auth, use \"--password=\"pwd\"\" and the same for username. [USR:25] [PWD:256]\n", CYAN, RESET);
                return 0;   /* verifica se é uma timed auth (via -t arg) */
            }else if( ( ( strcmp(argv[1], "-t") == 0 ) && (strncmp(argv[2],"--password=",12) == 0) && (strncmp(argv[3],"--username=",12) == 0 ) )){
                if(strlen(argv[2])>37 || strlen(argv[3]) > 268){
                    fprintf(stdout,"%s[CONSOLE]%s> Username or password sent via arguments are too long. [USR:25] [PWD:256]\n", CYAN, RESET);
                    return 0;
                }else{
                    sprintf(username, "%s", argv[3]+12);
                    sprintf(password, "%s", argv[2]+12);
                }
            }else if ( ( ( strcmp(argv[1], "-t") == 0 ) && (strncmp(argv[3],"--password=",12) == 0) && (strncmp(argv[2],"--username=",12) == 0 ) ) ){
                if(strlen(argv[2])>37 || strlen(argv[3]) > 268){
                    fprintf(stdout,"%s[CONSOLE]%s> Username or password sent via arguments are too long. [USR:25] [PWD:256]\n", CYAN, RESET);
                    return 0;
                }else{
                    sprintf(username, "%s", argv[2]+12);
                    sprintf(password, "%s", argv[3]+12);
                }
            }
            /* verifica se é uma regular auth (non-timed) (inexistencia de -t arg) */
        }else 
            if( (argc == 3) ){
                if(strlen(argv[2])<=12 || strlen(argv[3]) <= 12){
                    fprintf(stdout,"%s[CONSOLE]%s> Wrong init arguments. If trying to auth, use \"--password=\"pwd\"\" and the same for username. [USR:25] [PWD:256]\n", CYAN, RESET);
                    return 0;   /* verifica se é uma timed auth (via -t arg) */
                }else if(  (strncmp(argv[1],"--password=",12) == 0) && (strncmp(argv[2],"--username=",12) == 0 )){
                    if(strlen(args[1])>37 || strlen(args[2]) > 268){
                        fprintf(stdout,"%s[CONSOLE]%s> Username or password sent via arguments are too long. [USR:25] [PWD:256]\n", CYAN, RESET);
                        return 0;
                    }else{
                        sprintf(username, "%s", argv[3]+12);
                        sprintf(password, "%s", argv[2]+12);

                    }

                }else if( (  (strncmp(argv[2],"--password=",12) == 0) && (strncmp(argv[1],"--username=",12) == 0 ) ) ){
                    if(strlen(args[1])>37 || strlen(args[2]) > 268){
                        fprintf(stdout,"%s[CONSOLE]%s> Username or password sent via arguments are too long. [USR:25] [PWD:256]\n", CYAN, RESET);
                        return 0;
                    }else{
                        sprintf(username, "%s", argv[2]+12);
                        sprintf(password, "%s", argv[3]+12);
                    }

                }
                
            
        }

        fprintf(stdout,"\n    		>> Welcome to the Console Application <<    \n");
        fprintf(stdout,"%s[CONSOLE]%s> Server is online, waiting for a command. Type 'help' for commands.\n",CYAN, RESET);

        

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