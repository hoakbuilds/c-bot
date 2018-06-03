
/* Client Files 
    /ccli.c     -> Just Console things
*/

/* Libs */
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <ctype.h>
#include <errno.h>
#include <dirent.h>
#include <stdbool.h>
#include <crypt.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <time.h>
#include <locale.h>
#include "parson.h"

/* Path Macros */
#define FIFOWRITE "/tmp/LoggingAgent"
#define FIFOREAD  "/tmp/LoggingResponseTeam"
#define FIFOAUTH "/tmp/LoggingAuthProtocol"
#define FIFOAUTHRT "/tmp/LoggingAuthRT"
#define CMCTICKER "https://api.coinmarketcap.com/v2/ticker/"

/* Function Macros 
#define ASSERT(x) { \
  if(!(x)) { \
    fprintf(stderr, "Assertion: %s: function %s, line %d\n", (char*)(__FILE__), (char*)(__func__), (int)__LINE__); \
    raise(SIGTRAP); \
  } \
}*/

/* Constant Macros */
#define MAXPROC 50
#define MAXLEN 1024
#define TIME_INTERVAL 5

/* Text Formatting */
#define YELLOW             "\x1b[33m"
#define BLUE               "\x1b[34m"
#define MAGENTA            "\x1b[35m"
#define GREEN              "\x1b[32m"
#define RED                "\x1b[31m"
#define RESET              "\x1b[0m"
#define CYAN               "\x1b[36m"
#define SCALP              "\x1b[34m\033[1m"
#define GREEN_BOLD         "\x1b[32m\033[1m"
#define RED_BOLD           "\x1b[31m\033[1m"
#define STYLE_BOLD         "\033[1m"
#define STYLE_NO_BOLD      "\033[22m"
#define STYLE_UNDERLINE    "\033[4m"
#define STYLE_NO_UNDERLINE "\033[24m" 

/* Data Structures 

  See /parson.h for info on parson's structures for JSON

  */
typedef struct auth_t{
  int tempo;
  char token[256];
}timed_auth_t;

typedef struct cleaner_t{
  int id;
  char filename[256];
}cleaner_t;

enum auth_mode {   
  REGISTRATION_FAILED = -3,
  AUTH_FAILED = -2,
  AUTH_ERROR   = -1,
  AUTH_SUCCESS = 0,
  USRPWD = 1,
  AUTH_TOKEN = 2,
  REGISTRATION_SUCCESS = 3
};

/*  Function Definitions */

int main(int argc, char **argv);
void cbot(int pid, char authtoken[256], int timed);
int parse_appcli(char *buf, char **args);
void strtime(char *buffer);
void printGlobalData(JSON_Value *root);
void printCoins(JSON_Value *root);
void printInfoID(JSON_Value *root, int id);
void printTopMC(JSON_Value *root);
int spawnTracker( int id);
int getAuthenticated( int auth_mode, char username[25], char password[256], char authtoken[256]);
void timedauth_t( int tempo );
void * timedauthwrapper(void *args);
void cleanerthread_t ( char *nome, int id );
void * cleanerwrapper(void *args);

/* Global Variables */