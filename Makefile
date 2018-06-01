CC=cc
FLAGS=-c -Wall
LIBS=-lm -L. libparson.so
OBS= server.o parse_sv.o usracc_sys.o ccli.o parse_appcli.o ccli.o crypto.o btccli.o
OBSSERVER= server.o parse_sv.o usracc_sys.o 
OBSCCLI= ccli.o parse_ccli.o
OBSCRYPTO= crypto.o parse_appcli.o
 
 
 
all :  LoggingServer Ccli Crypto  
 
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

crypto.o 		: crypto.h crypto.c
	$(CC) $(FLAGS) crypto.c

LoggingServer   : $(OBSSERVER)
	$(CC)  -o LoggingServer  $(OBSSERVER) $(LIBS)

Ccli   : $(OBSCCLI)
	$(CC)  -o Ccli  $(OBSCCLI) $(LIBS)

Crypto   : $(OBSCRYPTO)
	$(CC)  -o c-bot  $(OBSCRYPTO) $(LIBS) 
	
clean limpar:
	rm -f LoggingServer *.o
	rm -f Ccli *.o
	rm -f crypto *.o
	echo "Cleaning executables, compilation files and other trash."
