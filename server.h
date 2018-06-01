
/* Server Files 
    /usracc_sys.c -> Login / Create Acc and auxiliary functions
    /server.c     -> Just Server things
    /parse_sv.c   -> The server's parser for IPC
*/

/* Libs */
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <sys/prctl.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>
#include <errno.h>
#include <dirent.h>
#include <stdbool.h>
#include <crypt.h>

#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <openssl/evp.h>
#include <openssl/conf.h>
#include <openssl/crypto.h>
#include <openssl/err.h>

/* Path Macros */
#define PATHDB "db/"
#define PATHUSRDB "db/users.dat"
#define PATHLOGS "logs/"
#define PATHERRLOG "logs/error"
#define PATHSVLOGS "logs/stdout"
#define PATHAGENTLOGS "logs/agent"
#define PATHRTLOGS "logs/response_team"
#define FIFOREAD "/tmp/LoggingAgent"
#define FIFOWRITE "/tmp/LoggingResponseTeam"
#define FIFOAUTH "/tmp/LoggingAuthProtocol"
#define FIFOAUTHRT "/tmp/LoggingAuthRT"

/* Function Macros */

/* Constant Macros */
#define MAXLEN 1024
#define SVMODE S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH
#define OFLAGS  O_WRONLY | O_CREAT | O_TRUNC
#define GREEN   "\x1b[32m"
#define RED     "\x1b[31m"
#define RESET   "\x1b[0m"
#define CYAN    "\x1b[36m"
#define MAGENTA "\x1b[35m"

/* String Macros */
#define INVFORMAT "[SERVER AUTH AGENT]> Authentication format is invalid.\n"

/* Data Structures */

typedef struct X{
    int id;
    char *user;
    struct X *nseg;
}User;

typedef struct Y{
    int auth_id;
    struct Y *nseg;
}Client;

int parse_sv(char *buf, char **args);
int login(char user[25], char password[64]);
int createacc(int uid, char user[25], char password[64]);
char* genpseudoSaltedHash(char username[25], char password[256]);
