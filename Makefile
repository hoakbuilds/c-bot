CC=cc
FLAGS=-c -Wall
LIBS=-lm -pthread -lpthread -lssl -lcrypto -lcjson
OBS= server.o parse_sv.o usracc_sys.o ccli.o appcli.o ccli.o appcli.o btccli.o
OBSSERVER= server.o parse_sv.o usracc_sys.o 
OBSCCLI= ccli.o parse_ccli.o
OBSAPPCLI= appcli.o parse_appcli.o
OBSAPPCLI= btccli.o parse_appcli.o
 
 
all :  LoggingServer Ccli Appcli Btccli
 
server.o 		: server.h server.c
	$(CC) $(FLAGS) server.c

parse_sv.o 		: server.h parse_sv.c
	$(CC) $(FLAGS) parse_sv.c

parse_ccli.o 		: server.h parse_ccli.c
	$(CC) $(FLAGS) parse_ccli.c

parse_appcli.o 		: server.h parse_appcli.c
	$(CC) $(FLAGS) parse_appcli.c

usracc_sys.o 	: server.h usracc_sys.c
	$(CC)	$(FLAGS) usracc_sys.c
	
ccli.o 			: ccli.h ccli.c
	$(CC) $(FLAGS) ccli.c

appcli.o 		: appcli.h appcli.c
	$(CC) $(FLAGS) appcli.c

btccli.o 		: appcli.h btccli.c
	$(CC) $(FLAGS) btccli.c

LoggingServer   : $(OBSSERVER)
	$(CC)  -o LoggingServer  $(OBSSERVER) $(LIBS)

Ccli   : $(OBSCCLI)
	$(CC)  -o Ccli  $(OBSCCLI) $(LIBS)

Appcli   : $(OBSAPPCLI)
	$(CC)  -o Appcli  $(OBSAPPCLI) $(LIBS)

Btccli   : $(OBSAPPCLI)
	$(CC)  -o Btccli  $(OBSAPPCLI) $(LIBS)
	
clean limpar:
	rm -f LoggingServer *.o
	rm -f Ccli *.o
	rm -f Appcli *.o
	echo "Cleaning executables, compilation files and other trash."
