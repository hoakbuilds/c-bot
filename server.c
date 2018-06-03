#include "server.h"

/*	LOGGING SERVER	

	In this file you will find a distinction between the SERVER AGENT, SERVER RESPONSE TEAM and the
	SERVER AUTH AGENT.

	The SERVER AGENT process will be responsible for:
		- Managing the sockets 

	The SERVER RESPONSE TEAM process will be responsible for:
		- The socket management system for IPC to respond to requests from Application Clients (AC)
		as well as the Console Client (CC)
		- Do the necessary process(es) to be able to respond to those requests, be it:
			_» A request to show logs from AC X
			_» A
	
	The SERVER AUTH AGENT process will be responsible for:
		- The socket management system for IPC to notice a Request For Authentication (RFA) from AC's
		 and the CC
		- Do the necessary process(es) to be able to answer those RFA's, the former being:
			_» Registering a user, providing auth token, trashing user credentials,
			verifying authentication via auth token and serving successful login response to RFA's.
*/


/* Missing func to check if program has been added to system startup or not */


int main(int argc, char **argv){
	
	char buffer[MAXLEN]="", *args[64];
	int INBOUND=0, OUTBOUND=0, AUTH=0, pid=0, sidagent=0, numargs=0;
	int ppid = getpid(), err=0;
	int ERRLOG=0, SVLOGS=0, AGENTLOGS=0, AUTHRT=0, RTLOGS=0, USRDB=0;

	User *head = (User*)malloc(sizeof(User));

	if( (argc == 3) ){
                /* verifica se é uma regular auth (non-timed) (inexistencia de -t arg) */
            if( (strncmp(argv[1], "--startup",8) == 0 ) ){
                 
				if( (strncmp(argv[2], "enable",7) == 0 ) ){

				}else if( (strncmp(argv[2], "disable",7) == 0 ) ){
					err = startup(STARTUP_DISABLE);
					if(err==STARTUP_DISABLE_ERROR){
						fprintf(stdout,"[SERVER]> There was an error removing the Logging Server from startup execution.\n" );
               			return 0; 
					}else if ( err == STARTUP_DISABLE_SUCCESS){
						fprintf(stdout,"[SERVER]> Successfully removed the Logging Server from startup execution\n" );
                		return 0; 
					}
				}else{
					fprintf(stdout,"[SERVER]> Wrong init arguments. Try \"--help\"\n");
				}
             
                return 0;  
            }else {
                fprintf(stdout,"[SERVER]> Wrong init arguments. Try \"--help\"\n" );
                return 0; 
            }
			
			/* initiate the actual server */
        }else if( ( pid = fork() ) == 0){
			/* Check if fifos already exist */
		if( ((mknod(FIFOREAD, 0666, 0) < 0) && (errno == EEXIST)) 
		|| ((mknod(FIFOWRITE, 0666, 0) < 0) && (errno == EEXIST))
		|| ((mknod(FIFOAUTH, 0666, 0) < 0) && (errno == EEXIST))
		|| ((mknod(FIFOAUTHRT, 0666, 0) < 0) && (errno == EEXIST)) ){
			fprintf(stderr, "A Logging Server instance is already running!\n");
			return 0;
		}

		printf("		>> Welcome to the Logging Server (alpha release) << 		\n");
		printf("[SERVER]> Please wait while we set things up.\n");

		
		/* Set new session id */
		sidagent = setsid();
		/*  what ze fuck boi
		ERR_load_crypto_strings();
		OpenSSL_add_all_algorithms();
		OPENSSL_config(NULL); */

		if((sidagent == (pid_t) -1)){
			fprintf(stderr,"%s[FATAL ERROR]%s Setting new session for Server Agent.\n",RED,RESET);
			return 0;
		}

		fprintf(stdout,"[SERVER]> Readying communications..\n");
		sleep(1);

		/* Open the FIFOs for communication */
		if( (OUTBOUND= open(FIFOWRITE, O_WRONLY) == -1) ){
			fprintf(stderr,"%s[FATAL ERROR]%s Opening %s to write to Console. %s\n",RED,RESET,FIFOWRITE,strerror(errno));
			return 0;
		}

		if( (INBOUND= open(FIFOREAD, O_RDONLY) == -1) ){
			fprintf(stderr,"%s[FATAL ERROR]%s Opening %s to read from Console. %s\n",RED,RESET,FIFOREAD,strerror(errno));
			unlink(FIFOWRITE);
			return 0;
		}

		if( (AUTH=open(FIFOAUTH, O_RDONLY) == -1) ){
			fprintf(stderr,"%s[FATAL ERROR]%s Opening %s to read from Console. %s\n",RED,RESET,FIFOAUTH,strerror(errno));
			unlink(FIFOWRITE);
			unlink(FIFOREAD);
			return 0;
		}

		if( (AUTHRT=open(FIFOAUTHRT, O_WRONLY) == -1) ){
			fprintf(stderr,"%s[FATAL ERROR]%s Opening %s to read from Console. %s\n",RED,RESET,FIFOAUTH,strerror(errno));
			unlink(FIFOWRITE);
			unlink(FIFOREAD);
			unlink(FIFOAUTH);
			return 0;
		}

		/* Open files for server-side logging */
		if ( ( ( mkdir(PATHLOGS, S_IRWXU) != -1 ) || (errno == EEXIST) ) 
		&& ( ( ( mkdir(PATHDB, S_IRWXU) != -1 ) || (errno == EEXIST) ) ) ){

			fprintf(stdout,"[SERVER]> Checking database consistency.\n");
			sleep(1);

			if((ERRLOG = open(PATHERRLOG, OFLAGS , SVMODE)) == -1){
				fprintf(stderr,"%s[FATAL ERROR]%s %s - %s\n",RED,RESET,PATHERRLOG,strerror(errno));
				return 0;
			}

			if((SVLOGS = open(PATHSVLOGS, OFLAGS , SVMODE)) == -1){
				fprintf(stderr,"%s[FATAL ERROR]%s %s - %s\n",RED,RESET,PATHSVLOGS,strerror(errno));
				return 0;
			}

			if((AGENTLOGS = open(PATHAGENTLOGS, OFLAGS , SVMODE)) == -1){
				fprintf(stderr,"%s[FATAL ERROR]%s %s - %s\n",RED,RESET,PATHAGENTLOGS,strerror(errno));
				return 0;
			}

			fprintf(stdout,"[SERVER]> Checking database consistency..\n");
			sleep(1);

			if((RTLOGS = open(PATHRTLOGS, OFLAGS , SVMODE)) == -1){
				fprintf(stderr,"%s[FATAL ERROR]%s %s - %s\n",RED,RESET,PATHRTLOGS,strerror(errno));
				return 0;
			}

			if((USRDB=open(PATHUSRDB, OFLAGS, SVMODE) ) == -1 ){
				fprintf(stderr,"%s[FATAL ERROR]%s Opening %s to read from Console. %s\n",RED,RESET,PATHUSRDB, strerror(errno));
				return 0;
			}

			dup2(STDERR_FILENO, ERRLOG);
			
		}else{
			fprintf(stderr, "%s[FATAL ERROR]%s Could not create logs directory. %s\n",RED,RESET,PATHLOGS);
		}

		/* Kill parent and keep running in background doing server things */
		kill(ppid, SIGINT);

		/* use chain of sigints from stack overflow to terminate all processes from console*/

		if ( ( pid = fork() ) == 0 ){

			if (  ( pid = fork() ) == 0 ){
				
				dup2(STDOUT_FILENO, AGENTLOGS);

				fprintf(stderr ,"[SERVER AUTH AGENT]> Awaiting protocol communication. \n");

				/* Authentication Agent checks if Login or Registration Requests were sent to the FIFO for the AUTH PROTOCOL */
				while(1){

					if( read(AUTH, &buffer, MAXLEN) < 0){
						sleep(1);
					}else{
						if( ( numargs = parse_sv(buffer, args) ) >= 3 ){
							if(  (strcmp(args[0], "login") == 0) ){
								if(strcmp(args[1], "-t") == 0){
									if(strlen(args[2])>25 || strlen(args[2])>256){
										/*username too long*/
									}
								}
							}
							if(  (strcmp(args[0], "register") == 0) ){
								if(strcmp(args[1], "-t") == 0){
									if(strlen(args[2])>25 || strlen(args[2])>256){
										/*username too long*/
									}
								}
							}
						}else{

							write(AUTHRT, INVFORMAT, strlen(INVFORMAT));

						}
					}


				}

			}else{		

				while(1){
					
					/* Server Agents checks if any general requests were sent to the LoggingServer */
					fprintf(stderr ,"[SERVER AGENT]> Listening to Console.\n");
					sleep(1);

					if( read(INBOUND, &buffer, MAXLEN) < 0){
						fprintf(stderr ,"[SERVER AGENT]> Listening to Console..\n");
						sleep(1);
					}else{
						if( ( numargs = parse_sv(buffer, args) ) >= 3 ){

						}



					}

					/*these two should be changed because agent listens to console and
					it should be the first process to send chain of SIGQUIT
					*/
				}
			}

		}else{	
				/* possibly Response Team for SOCKETS usage on LOGS OUTBOUND COMMUNICATION */
			fprintf(stderr, "[SERVER RT]> i'll do sometihng soon\n");
			sleep(5);

			while(1){
				/* if ....
				then write to fifo_fd[0] ...*/
				
					
			}


		}

	}else {
		wait(&pid);
	}

	return 0;
}

void avisot (char *msg, int tempo, char *prompt){
    while (tempo > 0) {
    sleep (1);
        tempo--;
    }
    fprintf (stderr, "\n\nAviso:%s  \n\n", msg);
    
}

void * avisowrapper(void *args) {
    Aviso_t *avs = (Aviso_t *) args;
    avisot( avs->msg, avs->tempo, avs->prompt);
    free(avs);
    return NULL;
}

int gentoken(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext){
	EVP_CIPHER_CTX *ctx;
	int len;
	int ciphertext_len;

	/* Create and initialise the context */
	if(!(ctx = EVP_CIPHER_CTX_new())) return TOKEN_GEN_ERROR;

	if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) return TOKEN_GEN_ERROR;
		

	if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len)) return TOKEN_GEN_ERROR;
	ciphertext_len = len;

	if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) return TOKEN_GEN_ERROR;
	ciphertext_len += len;

	EVP_CIPHER_CTX_free(ctx);
	return ciphertext_len;
}


int startup(int MODE){

	int fd1=0,fd2=0, r, w, c=0, pos=0, a=0;
	char line[512], ch[1]={0}, newfname[]="newrc.local", sys_rm[]="rm rc.local";

	fd1=chdir("/etc/");
	if( fd1 < 0 ){
		fprintf(stderr, "[SERVER]> Error changing directory to %s\n", "/etc/");
		switch(MODE){
			case(STARTUP_DISABLE) : return STARTUP_DISABLE_ERROR;
			case(STARTUP_ENABLE) : return STARTUP_ENABLE_ERROR;
		}
	}

	switch(MODE){

		case(STARTUP_DISABLE) : {

			if( (fd1 = open( RCLOCAL, O_RDWR)) < 0){
				fprintf(stderr, "[SERVER]> Error opening %s to disable server execution on startup.", RCLOCAL);
				return STARTUP_DISABLE_ERROR;
			}else if( (fd2 = open( newfname, O_RDWR|O_CREAT)) < 0){
				fprintf(stderr, "[SERVER]> Error opening %s to disable server execution on startup.", RCLOCAL);
				return STARTUP_DISABLE_ERROR;
			}else{

				while( (r = read( fd1, &ch, 1) ) > 0 ){
					
					if ( r < 0 ){
						fprintf(stderr, "[SERVER]> Error reading %s to disable server execution on startup.", RCLOCAL);
						return STARTUP_DISABLE_ERROR;
					}else {
						if (ch == '\n'){
							if( (strncmp(line, STARTCMD, strlen(STARTCMD)) == 0)){
								a=1;								
							}
							if(a==0){
								w=write(fd2, line, strlen(line));
								if(w<0){
									fprintf(stderr, "[SERVER]> Error writing to %s to disable server execution on startup.", RCLOCAL);
									return STARTUP_DISABLE_ERROR;
								}
							}
							memset(&line, 0, 512);
						}else{
							strcat(line, ch);
							
						}
						a=0;
					}

				}

				system("rm rc.local");
				rename(newfname, "rc.local");

				return STARTUP_DISABLE_SUCCESS;

				
			}


		}

		case(STARTUP_ENABLE) : {


			if( (fd1 = open( RCLOCAL, O_RDWR)) < 0){
				fprintf(stderr, "[SERVER]> Error opening %s to disable server execution on startup.");
				return STARTUP_DISABLE_ERROR;
			}else if( (fd2 = open( newfname, O_RDWR|O_CREAT)) < 0){
				fprintf(stderr, "[SERVER]> Error opening %s to disable server execution on startup.", RCLOCAL);
				return STARTUP_DISABLE_ERROR;
			}else{
				while( (r = read( fd1, &ch, 1) ) > 0 ){
					
					r = read( fd1, &ch, 1);
					if ( r < 1 ){
						fprintf(stderr, "[SERVER]> Error reading %s to disable server execution on startup.", RCLOCAL);
						return STARTUP_DISABLE_ERROR;
					}else {
						if (ch == '\n'){
							if( (strncmp(line, "exit 0", strlen(line)-1) == 0)){
								w = write(fd2, STARTCMD, strlen(STARTCMD));
								if(w<0){
									fprintf(stderr, "[SERVER]> Error adding to startup.", RCLOCAL);
									return STARTUP_ENABLE_ERROR;
								}								
							}
							if(a==0){
								w=write(fd2, line, strlen(line));
								if(w<0){
									fprintf(stderr, "[SERVER]> Error adding to startup.", RCLOCAL);
									return STARTUP_ENABLE_ERROR;
								}
							}
							memset(&line, 0, 512);
						}else{
							strcat(line, ch);
							
						}
					}

				}

				system("rm rc.local");
				rename(newfname, "rc.local");
				return STARTUP_ENABLE_SUCCESS; 
				
			}
			
		}



	}



}

char genpseudoSaltedHash(char username[25], char password[256]){

	char salted[strlen(username)+strlen(password)];
	int i = 0, a=strlen(username)-1;

	for(i=0;i<strlen(username);i++){
		salted[i]=username[i];
	}

	for(i=0;i<strlen(password);i++){
		salted[i+a] = password[i];
	}
	
	return salted;

}