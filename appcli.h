
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
#include "parson.h"

/* Path Macros */
#define FIFOWRITE "/tmp/LoggingAgent"
#define FIFOREAD  "/tmp/LoggingResponseTeam"
#define FIFOAUTH "/tmp/LoggingAuthProtocol"
#define CMCTICKER "https://www.api.coinmarketcap.com/v2/ticker/"
#define CMCLISTINGS "https://www.api.coinmarketcap.com/v2/listings/"

/* Function Macros */
#define ASSERT(x) { \
  if(!(x)) { \
    fprintf(stderr, "Assertion: %s: function %s, line %d\n", (char*)(__FILE__), (char*)(__func__), (int)__LINE__); \
    raise(SIGTRAP); \
  } \
}

/* Constant Macros */
#define MAXLEN 1024
#define TIME_INTERVAL 5
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
#define CIPHERS  "HIGH:!aNULL:!kRSA:!PSK:!SRP:!MD5:!RC4" 

/* Data Structures 

  See /parson.h for info on parson's structures for JSON

  */
 
 

/*  Function Definitions */
int main();
int parse_appcli(char *buf, char **args);
int cmd(int numargs, char **args);
int socket_connect( );
void printGlobalData(JSON_Value *root);
void printCoins(JSON_Value *root);
int spawnTracker(char *curl_link);
void printTopMC(JSON_Value *root);
void strtime(char *buffer);