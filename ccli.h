
/* Client Files 
    /appcli.c -> Just Application things
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

#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <openssl/evp.h>
#include <openssl/conf.h>
#include <openssl/crypto.h>

/* Path Macros */
#define FIFOWRITE "/tmp/LoggingAgent"
#define FIFOREAD  "/tmp/LoggingResponseTeam"
#define FIFOAUTH "/tmp/LoggingAuthProtocol"
#define FIFOAUTHRT "/tmp/LoggingRT"

/* Function Macros */


/* Constant Macros */
#define MAXLEN 1024
#define GREEN   "\x1b[32m"
#define RED     "\x1b[31m"
#define RESET   "\x1b[0m"
#define CYAN    "\x1b[36m"
#define MAGENTA "\x1b[35m"

/* Data Structures */


/* Function Definitions */

int parse_ccli(char *buf, char **args);
int cmd(int numargs, char **args, int auth);