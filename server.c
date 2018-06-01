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

#include "server.h"

int main(int argc, char **argv){
	
	char buffer[MAXLEN]="", *args[64];
	int INBOUND=0, OUTBOUND=0, AUTH=0, pid=0, sidagent=0, numargs=0;
	int ppid = getpid();
	int ERRLOG=0, SVLOGS=0, AGENTLOGS=0, AUTHRT=0, RTLOGS=0, USRDB=0;
	User *users = (User*)malloc(sizeof(User)*MAXLEN);
	Client *clients = (Client*)malloc(sizeof(User)*MAXLEN);

	if( ( pid = fork() ) == 0){

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

		if ( ( pid = fork() ) == 0 ){

			if (  ( pid = fork() ) == 0 ){

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

			}		

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
