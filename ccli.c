#include "ccli.h"

int outbound=0, inbound=0, protocol=0, protocolrt=0;

int main(int argc, char **argv){

   
    int auth=0, pid=0;
    char password[256]={0}, username[25]={0}, authtoken[256]={0};
	
    if( (  outbound = open(FIFOWRITE , O_WRONLY)  < 0 ) 
    || ( ( inbound = open(FIFOREAD, O_RDONLY) ) < 0)
    || ( ( protocol = open(FIFOAUTH, O_WRONLY) ) < 0)
    || ( ( protocolrt = open(FIFOAUTHRT, O_RDONLY) ) < 0) ){
        fprintf(stderr,"%s[CONNECTION:ERROR]%s Could not estabilish connection to server.\n",RED,RESET);
        sleep(2);
        fprintf(stderr,"%s[CONNECTION:ERROR]%s Could not estabilish connection to server..\n",RED,RESET);
        sleep(2);
        fprintf(stderr,"%s[CONNECTION:ERROR]%s Could not estabilish connection to server...\n",RED,RESET);
        sleep(2);
        return main(argc, argv);
    }else{
        if(argc < 2){
            fprintf(stdout,"%s[CONSOLE]%s> Wrong init arguments. Try \"--help\"\n", CYAN, RESET);
            return 0; 
        }else if( (argc == 2 )  ){
            if( (strncmp(argv[1], "--help",7 ) == 0 ) ){
                fprintf(stdout,"%s[CONSOLE]%s> -t [TIME] --auth [AUTH_TOKEN] (Without the [])\n        Auths user based on authentication token, authentication hardtimed at 5 minutes if \"-t\" is used and [TIME] is omitted.\n        [AUTH_TOKEN->MAX_LENGTH=256]\n", CYAN, RESET);
                fprintf(stdout,"%s[CONSOLE]%s> --reg --username [USER] --password [PASSWD] (Without the [])\n        Registers [username] and retrieves auth token for future authentications.\n        [USERNAME->MAX_LENGTH=25] [PASSWD->MAX_LENGTH=256]\n", CYAN, RESET);
                return 0;
            }else{
                fprintf(stdout,"%s[CONSOLE]%s> Wrong init arguments. Try \"--help\"\n", CYAN, RESET);
                return 0; 
            }
        }else if( (argc == 3) ){
                /* verifica se é uma regular auth (non-timed) (inexistencia de -t arg) */
            if( (strncmp(argv[1], "--token",8) == 0 ) ){

                sprintf(authtoken,
                    "%s",argv[2]);
                printf("%s",authtoken);

                auth=getAuthenticated(AUTH_TOKEN,NULL,NULL,authtoken);

                if( auth == AUTH_SUCCESS ){
                    if( (pid = fork())==0){
                        ccli(pid, authtoken, 0);
                    }else{
                        wait(&pid);
                    }
                }else{
                    fprintf(stdout,"%s[CONSOLE]%s> Authentication token is invalid.\n", CYAN, RESET);
                    return 0; 
                }

                return 0;  
            }else {
                fprintf(stdout,"%s[CONSOLE]%s> Wrong init arguments. Try \"--help\"\n", CYAN, RESET);
                return 0; 
            }
             
        }else if(  argc == 4 ){
             
            if( ( strcmp(argv[1], "-t") == 0 ) && (strncmp(argv[2],"--token",8) == 0) ){
                    /* timed auth without time specified */
                sprintf(authtoken,
                    "%s",argv[3]);
                printf("%s",authtoken);
                /*tries to auth*/
                auth=getAuthenticated(AUTH_TOKEN,NULL,NULL,authtoken);

                if( auth == AUTH_SUCCESS ){
                    if( (pid = fork())==0){ /*son initiates client*/
                        ccli(pid, authtoken, 1);
                    }else{
                         /* while parent initiates a thread to hard time the auth session */
                        pthread_t th;
                        timed_auth_t *t_auth = (timed_auth_t*)malloc(sizeof(timed_auth_t));
                        t_auth->tempo = 300;
                        pthread_create( &th, NULL, &timedauthwrapper, (void*)t_auth );
                        pthread_join(th, NULL);
                        kill( pid, SIGQUIT);
                        return 0;

                    }
                }else{

                    fprintf(stdout,"%s[CONSOLE]%s> Authentication token is invalid.\n", CYAN, RESET);
                    return 0; 

                }
            
            }else{
                fprintf(stdout,"%s[CONSOLE]%s> Wrong init arguments. Use \"--help\" to see a list arguments.\n", CYAN, RESET);
                return 0;
            }

        }else if ( argc == 5 ){
                /* timed auth with time specified */
            if( ( strcmp(argv[1], "-t") == 0 ) && (strcmp(argv[3],"--token") == 0) ){

                sprintf(authtoken,
                    "%s",argv[4]);
                printf("%s",authtoken);
                /*tries to auth*/
                auth=getAuthenticated(AUTH_TOKEN,NULL,NULL,authtoken);

                if( auth == AUTH_SUCCESS ){
                    if( (pid = fork())==0){ /*son initiates client*/
                        ccli(pid, authtoken, 1);
                    }else{ /* while parent initiates a thread to hard time the auth session */
                        pthread_t th;
                        timed_auth_t *t_auth = (timed_auth_t*)malloc(sizeof(timed_auth_t));
                        t_auth->tempo = atoi(argv[2]);
                        strcpy(t_auth->token, authtoken);
                        pthread_create( &th, NULL, &timedauthwrapper, (void*)t_auth );
                        pthread_join(th, NULL);
                        kill(pid, SIGQUIT);
                        return 0;
                    }
                }else{
                    fprintf(stdout,"%s[CONSOLE]%s> Authentication token is invalid.\n", CYAN, RESET);
                    return 0; 
                }

            }

        }else if( argc == 6 ){
               /* verifica se é um pedido de registo juntamente do servidor */
            if( ((strncmp(argv[1], "--register",7) == 0 ) && (strncmp(argv[2],"--username",12) == 0 ) && (strncmp(argv[4],"--password",12) == 0) )){

                if(strlen(argv[3])>37 || strlen(argv[3]) > 268){
                    fprintf(stdout,"%s[CONSOLE]%s> Username or password sent via arguments are too long. [USR:25] [PWD:256]\n", CYAN, RESET);
                    return 0;
                }else{
                    sprintf(username, "%s", argv[3]);
                    sprintf(password, "%s", argv[5]);

                    auth = getAuthenticated(USRPWD, username, password, authtoken);

                    if( auth == REGISTRATION_SUCCESS ){
                        fprintf(stdout, "%s[CONSOLE]%s> Registration successful.\n", CYAN, RESET);
                        fprintf(stdout, "%s[CONSOLE]%s> This is your authentication token, it is your means of authenticating yourself within the server, please save it.\n     %s\n",CYAN, RESET, authtoken );
                        return 0; 
                    }else {

                    }
                }
                
                
            }else {
                fprintf(stdout,"%s[CONSOLE]%s> Wrong init arguments. Use \"--help\" to see a list arguments.\n", CYAN, RESET);
                return 0;
            }

        }else{
            fprintf(stdout,"%s[CONSOLE]%s> Wrong init arguments. Use \"--help\" to see a list arguments.\n", CYAN, RESET);
            return 0;
        } 

    }

    return 0;
    
}

void ccli(int pid, char authtoken[256], int timed){

    if( timed == 0 ){
        kill(pid, SIGQUIT);
    }

    char buffer[MAXLEN] /*input*/, *args[64] /*partitioned input*/;
    int len = 0, numargs=0;

    fprintf(stdout,"\n    		>> Welcome to the Console Application <<    \n");
    fprintf(stdout,"%s[CONSOLE]%s> Server is online, waiting for a command. Type 'help' for commands.\n",CYAN, RESET);
    fprintf(stdout,"%s[CONSOLE]%s>", CYAN, RESET);
        

    while(1){

        if ( fgets(buffer,1023,stdin) == NULL){
            continue;
        }

        if( (len = strlen(buffer)) == 1) continue;

        if( buffer[len-1] == '\n' ) buffer[len-1] = '\0';

        numargs = parse_ccli(buffer, args);

        if ( !cmd(numargs, args ) ){
            fprintf(stdout,"%s[CONSOLE]%s> That is not an available command. Type 'help' for a list of available commands.\n", CYAN, RESET);
            fprintf(stdout,"%s[CONSOLE]%s>", CYAN, RESET);
        }

    }

    

}

int getAuthenticated( int auth_mode, char username[25], char password[256], char authtoken[256]){

    char message[512]={0};
    char buffer[1024]={0};
    int  w=0,r=0;

    switch(auth_mode){

        case(USRPWD) : {

            sprintf(message,
                "%s %s %s",
                    "register", username, password);

            if( (w = write(protocol, message, strlen(message))) < 0 ){
                fprintf(stdout,"%s[CONSOLE]%s> Error communicating with the server.\n",CYAN, RESET);
                return AUTH_ERROR;
            }else{
                while( r >= 0){
                    if ( (r = read(protocolrt, &buffer, 1024)) <= 0){
                        continue;
                    }else{
                        if ( strncmp(buffer, "registration_successful" , 26)  == 0 ){
                            sprintf(authtoken, "%s", buffer+27);                             
                        }else if ( strncmp(buffer, "registration_failed" , 14)  == 0 ){
                            if( strncmp(buffer+15, username, strlen(username)) == 0){
                                return REGISTRATION_FAILED;
                            }else continue;
                        }
                    }
                }
            }
            

        }

        case(AUTH_TOKEN) : {

            sprintf(message,
                "%s %s",
                    "login_auth_token", authtoken);

            if( (w = write(protocol, message, strlen(message))) < 0 ){
                fprintf(stdout,"%s[CONSOLE]%s> Error communicating with the server.\n",CYAN, RESET);
                return AUTH_ERROR;
            }else{
                while( r >= 0){
                    if ( (r = read(protocolrt, &buffer, 1024)) <= 0){
                        continue;
                    }else{
                        if ( strncmp(buffer, "authentication_successful" , 26)  == 0 ){
                            if( strncmp(buffer+8, authtoken, strlen(authtoken)) == 0){
                                return AUTH_SUCCESS;
                            }else continue;
                        }else if ( strncmp(buffer, "invalid_token" , 14)  == 0 ){
                            if( strncmp(buffer+8, authtoken, strlen(authtoken)) == 0){
                                return AUTH_FAILED;
                            }else continue;
                        }
                    }
                }
            }

        }

    }

    return AUTH_ERROR;

}

void timedauth_t (int tempo){
     
    while (tempo > 0) {
    sleep (1);
        tempo--;
    }
    fprintf (stderr, "\n%sI said it was hardtimed, Goodbye!%s\n",RED,RESET);
    
}
 
void * timedauthwrapper(void *args){

    timed_auth_t * time = (timed_auth_t*) args;
    timedauth_t(time->tempo);
    free(time);

    return NULL;
 
}