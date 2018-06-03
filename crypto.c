#include "appcli.h"

JSON_Value *root_listings, *root_top, *root_global, *root_tracker;
int outbound=0, inbound=0, protocol=0, protocolrt=0;



int main(int argc, char **argv){
    
    int auth=0, pid=0;
    char password[256]={0}, username[25]={0}, authtoken[256]={0};
	
    if( (  outbound = open(FIFOWRITE , O_WRONLY)  < 0 ) 
    || ( ( inbound = open(FIFOREAD, O_RDONLY) ) < 0) ){
        fprintf(stderr,"%s[CONNECTION:ERROR]%s Could not estabilish connection to server.\n",RED,RESET);
        sleep(2);
        fprintf(stderr,"%s[CONNECTION:ERROR]%s Could not estabilish connection to server..\n",RED,RESET);
        sleep(2);
        fprintf(stderr,"%s[CONNECTION:ERROR]%s Could not estabilish connection to server...\n",RED,RESET);
        sleep(2);
        return main(argc, argv);
    }else if(argc < 2){
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
                        cbot(pid, authtoken, 0);
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
                        cbot(pid, authtoken, 1);
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
                        cbot(pid, authtoken, 1);
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

    return 0;

}


void cbot(int pid, char authtoken[256], int timed) {

    
    char buffer[MAXLEN], *args[64];
    char curl_listings[256]={0}, curl_global[256]={0}, curl_top[256]={0};
    char globaldatafile[] = "globaldata.json", listingsfile[] = "listings.json", topfile[]= "top.json";
    char timestring[64]={0};
    char cleanup_command[256];
    int len = 0, numargs=0;
    JSON_Status err;
    JSON_Array *tracker;

    root_tracker = json_value_init_array();

    setlocale(LC_NUMERIC, "");

    fprintf(stdout,
        "\n                     >> PROJECT %sS%s.%sC%s.%sA%s.%sL%s.%sP%s. << \n",
            SCALP,RESET,SCALP,RESET,SCALP,RESET,SCALP,RESET,SCALP,RESET);

    /* it ain't pretty, but it's not a libcurl tutorial /*/
    sprintf(curl_listings,
        "curl -s \"https://api.coinmarketcap.com/v2/listings/\" > %s", listingsfile);
    sprintf(curl_global,
        "curl -s \"https://api.coinmarketcap.com/v2/global/\" > %s", globaldatafile);
    sprintf(curl_top,
        "curl -s \"https://api.coinmarketcap.com/v2/ticker/?structure=array/\" > %s", topfile);

    //"rm -f %s", listingsfile);
    //time = ctime( &t );

    strtime(timestring);
    fprintf(stdout,"%s{%s ~ c-bot}%s> Logging Server is online.\n", CYAN, timestring, RESET);

    if( (mkdir("data", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) < 0) && (EEXIST != errno) ){
        fprintf(stderr, "%s{%s ~ c-bot}%s> Error assembling local file structure.\n",
            CYAN, timestring, RESET );
                return;
    }else {
        fprintf(stderr, "%s{%s ~ c-bot}%s> Successfully assembled local file structure.\n",
            CYAN, timestring, RESET );
    }

    strtime(timestring);
    fprintf(stdout,
        "%s{%s ~ c-bot}%s> API is online @ %s.\n",
            CYAN, timestring, RESET, "api.coinmarketcap.com");

    strtime(timestring);
    fprintf(stdout,
        "%s{%s ~ c-bot}%s> Bot is ready, waiting for a command. Type 'help' for commands.\n",
            CYAN,timestring, RESET);
    fprintf(stdout,
        "%s{%s ~ c-bot}%s> ",
            CYAN,timestring, RESET);

    while(1){
        
        if ( fgets(buffer,1023,stdin) == NULL){
            strtime(timestring);
            fprintf(stdout,"%s{%s ~ c-bot}%s> ",CYAN,timestring, RESET);
        }

        if( (len = strlen(buffer)) < 4) continue;

        if( buffer[len-1] == '\n' ) buffer[len-1] = '\0';

        numargs = parse_appcli(buffer, args);

        if (strcmp(args[0], "help") == 0 ){
            fprintf(stdout, "\nHow to use %sc-bot%s\n\n",GREEN_BOLD,RESET);
            fprintf(stdout, "%strack {ID}              %s\n",CYAN,RESET);
            fprintf(stdout, "Adds cryptocurrency with specified {ID} to bot's tracklist.\n\n"RESET);
            fprintf(stdout, "%stop 			           %s\n",CYAN,RESET);
            fprintf(stdout, "Displays Top 100 cryptocurrencies based on Market Capitalization.\n\n");
            fprintf(stdout, "%sinfo	{ID}			   %s\n",CYAN,RESET);
            fprintf(stdout, "Displays cryptocurrency indicated by {ID}.\n\n");
            fprintf(stdout, "%slistings			   %s\n",CYAN,RESET);
            fprintf(stdout, "Displays all cryptocurrency listings available on Coin Market Cap.\n\n");
            fprintf(stdout, "%sglobal data			   %s\n",CYAN,RESET);
            fprintf(stdout, "Displays global data from cryptocurrency ecosystem.\n\n");
            
        }else if ((strcmp(args[0], "track") == 0) ){

            tracker = json_value_get_array(root_tracker);
            if( tracker != JSONArray ){
                fprintf(stderr, "spawntracker:\n");
            }

            /*system(curl_tracker);
            root_tracker[0] = json_parse_file(tracker_file);

            if ( json_value_get_type(root_tracker[0]) != JSONObject  || root_tracker[0] == NULL) {
                printf("root_tracker:\n");
                system(cleanup_command);
                return 0;
            }else{
                printf("added to tracklist:\n");
                //printCoins(root_tracker[0]);
            }*/

            system("cd .."); 
            system("cd ..");           
                
        }else if ((strcmp(args[0], "top") == 0) ){
            
            system(curl_top);
            root_top = json_parse_file(topfile);

            if ( root_top == NULL) {
                    printf("root_top:  \n");
                return;
            }else{
                printTopMC( root_top );		
            }

        }else if ((strcmp(args[0], "info") == 0) ){
            //printInfoID(  , atoi(args[1]));
            
        }else if ((strcmp(args[0], "listings") == 0) ){
            system(curl_listings);
            root_listings = json_parse_file(listingsfile);

            if ( json_value_get_type(root_listings) != JSONObject  || root_listings == NULL) {
                printf("root_listings:\n");
                return;
            }else{
                printCoins(root_listings);
            }

        }else if ((strcmp(args[0], "global") == 0 ) && ( strcmp(args[1], "data") == 0) ){
            system(curl_global);
            root_global = json_parse_file(globaldatafile);

            if ( json_value_get_type(root_global) == JSONError ) {
                printf("root_global: \n");
                return;
            }else {
                printGlobalData(root_global);
            }

        }else{
            strtime(timestring);
            fprintf(stdout,"%s{%s ~ c-bot}%s> That is not an available command. Type 'help' for a list of available commands.\n",CYAN,timestring, RESET);
        }
        
    strtime(timestring);
    //fprintf(stdout,"%s{%s ~ c-bot}%s> Ready.\n",CYAN,timestring, RESET);
    fprintf(stdout,"%s{%s ~ c-bot}%s> ",CYAN,timestring, RESET);

    //printTopMC(root_top);
    //printGlobalData(root_global);
    //printCoins(root_listings);
    //strtime(timestring);
    //fprintf(stdout,"\n%s{%s ~ c-bot}%s> Bot is ready, waiting for a command. Type 'help' for commands.\n",CYAN,timestring, RESET);
    //fprintf(stdout,"%s{%s ~ c-bot}%s> ",CYAN,timestring, RESET);

    }
    
    /*
    json_value_free(root_top);
    json_value_free(root_listings);
    json_value_free(root_global);
      cleanup code */
    
    //system(cleanup_command);

}

int spawnTracker(int id){
    
    int pid = -1;
    char curl[256]={0}, tracker_file[256]={0};
    char timestring[64]={0};
    char a[5]={0};
    pthread_t th;
    cleaner_t *cleaner = (cleaner_t*)malloc(sizeof(cleaner));

    sprintf(a, "%d/", id);
    chdir("data/");
    chdir(a);

    if( ( pid=fork()) == 0 ){

        while(1){              
            strtime(timestring);  
            sprintf(tracker_file,
                        "%s-%d-%d-%d-%d-%d.json",
                            timestring,
                                rand()%10,
                                rand()%100,
                                rand()%1000,
                                rand()%10000,
                                rand()%10000);

            sprintf(curl,
                "curl -s \"%s%d/\" > %s",
                    CMCTICKER, id, tracker_file);

            system(curl);
            strcpy(cleaner->filename, tracker_file);
            cleaner->id = id;
            pthread_create(&th, NULL, &cleanerwrapper, (void*)cleaner );
            memset(curl, 0, 256);
            memset(tracker_file, 0, 256);
            sleep(5);
        }

    }
    
    return pid;

}

void printCoins(JSON_Value *root){

    JSON_Object *json_o_listings, *coin, *coin2, *coin3, *metadata;
    JSON_Array *data;
    int i=0;

    /* getting array from root value and printing commit info */
    json_o_listings = json_value_get_object(root);
    if (  json_o_listings == NULL) {
        fprintf(stderr,"printCoins: json_obj:  \n");
        return;

    }
    //json_serialize_to_file(root_listings, "teste.json");
    metadata = json_object_get_object(json_o_listings,"metadata");

     if (  metadata == NULL) {
        fprintf(stderr,"printCoins: metadata: \n");
        return;

    }
    fprintf(stderr,"Total Number of Cryptocurrencies: %.lf \n",
            json_object_get_number(metadata, "num_cryptocurrencies"));

    data = json_object_get_array(json_o_listings, "data");

    if(data==NULL){
        fprintf(stderr,"printCoins: data:  \n");
        return;
    }
 
    //printf("%-20.10s %-30.10s %-10s %-20.10s %-30.10s %s\n", "ID", "Coin", "Ticker", "ID", "Coin", "Ticker");
    fprintf(stdout,
         "%s%-20.10s  %-28.10s  %-10s   |   %-20.10s   %-28.10s %-10s    |    %-20.10s   %-28.10s  %-10s %s\n"
         ,GREEN, "ID", "Coin", "Ticker", "ID", "Coin", "Ticker", "ID", "Coin", "Ticker", RESET);
    //fprintf(stdout,"%s[CONSOLE]%s> Logging Server is online.\n");

    for (i = 0; i < 10; i++) {
        coin = json_array_get_object(data, i);
        coin2 = json_array_get_object(data, i+10);
        coin3 = json_array_get_object(data, i+20);
        fprintf(stdout, "%-20.lf %-30.10s  %s$%s%-10s |   %-20.lf %-30.10s %s$%s%-10s   |    %-20.lf %-30.10s  %s$%s%-10s\n",
               json_object_get_number(coin, "id"),
               json_object_get_string(coin, "name"),YELLOW,RESET,
               json_object_get_string(coin, "symbol"),
               json_object_get_number(coin2, "id"),
               json_object_get_string(coin2, "name"),YELLOW,RESET,
               json_object_get_string(coin2, "symbol"),
               json_object_get_number(coin3, "id"),
               json_object_get_string(coin3, "name"),YELLOW,RESET,
               json_object_get_string(coin3, "symbol"));

    }
  
}

void printGlobalData(JSON_Value *root){

    
    JSON_Object *json_o_global, *data_global,*quotes;    

    json_o_global = json_value_get_object(root);
    if(json_o_global ==NULL){
        printf("printGlobalData: json_o_global:\n");
        return;
    }
    data_global = json_object_get_object(json_o_global, "data");

    if(data_global==NULL  ){
        printf("printGlobalData: data_global:\n");
        return;
    }
    quotes = json_object_get_object(data_global, "quotes");

    fprintf(stdout, "%s%sActive Cryptos:%s %-20.lf\n%s%sActive Markets:%s %-20.lf\n%s%sBitcoin Dominance:%s %-15.2lf\n%s%sTotal Market Cap:%s        $%.lf\n%s%sTotal Volume 24h:%s        $%.lf%s\n",
        BLUE,STYLE_BOLD,RESET,
        json_object_get_number(data_global,"active_cryptocurrencies"),
        BLUE,STYLE_BOLD,RESET,
        json_object_get_number(data_global,"active_markets"),
        BLUE,STYLE_BOLD,RESET,
        json_object_get_number(data_global,"bitcoin_percentage_of_market_cap"),
        BLUE,STYLE_BOLD,RESET,
        json_object_dotget_number(quotes,"USD.total_market_cap"),
        BLUE,STYLE_BOLD,RESET,
        json_object_dotget_number(quotes,"USD.total_volume_24h"),
        STYLE_NO_BOLD);
 
}

void printTopMC(JSON_Value *root){

    JSON_Object *json_o_global, *coin, *coin2;
    JSON_Array *data;
    int i = 0;

    json_o_global = json_value_get_object(root);
    if(json_o_global ==NULL){
        printf("printTopMC: json_o_global:\n");
        return;
    }
    data = json_object_get_array(json_o_global, "data");

    if(data==NULL  ){
        printf("printTopMC: data:\n");
        return;
    }

    fprintf(stdout, 
        "%s%-10s  %-10s          %-10s           %-10s   %-10s | %s%-10s  %-10s            %-10s        %-10s     %-10s         %s  \n",
         RED_BOLD,
          "RANK",
           "COIN",
            "MARKET CAP ($)",
             "CHANGE 24H (%)",
              "ASK ($)", 
               RED_BOLD,
                "RANK",
                 "COIN",
                  "MARKET CAP ($)",
                   "CHANGE 24H (%)",
                    "ASK ($)",RESET);
    for (i = 0; i < 50; i++){
        coin = json_array_get_object(data, i);
        coin2 = json_array_get_object(data, (i+50));
        fprintf(stdout, 
            "%-5.lf %-15.15s (%s$%-5s%s)   %'-25.lf  %-5.2lf%%       $%'-10.1lf| %-5.lf %-15.15s (%s$%-5s%s)   %'-25.lf  %-5.2lf%%       $%'-10.1lf \n",
             json_object_get_number(coin, "rank"),
              json_object_get_string(coin, "name"),
               YELLOW,json_object_get_string(coin,"symbol"),RESET,
                json_object_dotget_number(coin,"quotes.USD.market_cap"),
                 json_object_dotget_number(coin,"quotes.USD.percent_change_24h") ,
                  json_object_dotget_number(coin,"quotes.USD.price"),
                   json_object_get_number(coin2, "rank"),
                    json_object_get_string(coin2, "name"),
                     YELLOW,json_object_get_string(coin2,"symbol"),RESET,
                      json_object_dotget_number(coin2,"quotes.USD.market_cap"),
                       json_object_dotget_number(coin2,"quotes.USD.percent_change_24h") ,
                        json_object_dotget_number(coin2,"quotes.USD.price")
                    );
    }

}

void printInfoID(JSON_Value *root, int id){
    printf("soon\n");
}

void strtime(char *buffer){
     
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    sprintf(buffer, "%d-%d-%d_(%d:%d:%d)",timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

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

            if( (w = write(outbound, message, strlen(message))) < 0 ){
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

void cleanerthread_t ( char *nome, int id ){

    int fd1=0,fd2=0, r, w;
	char line[512], ch[1]={0}, datafile[5]={0};

    sprintf(datafile, "%d", id);

    if((fd1 = open(nome,O_RDWR))<0){
        fprintf(stderr, "{ cleaner/tracker %d ~ c-bot}> Error opening %s to avoid trash.",id, nome);
        return;
    }else if( (fd2 = open(datafile, O_APPEND))<0 ){
        fprintf(stderr, "{ cleaner/tracker %d ~ c-bot}> Error opening %s to avoid trash.",id, nome);
        return; 
    }else {
        while( (r = read( fd1, &ch, 1) ) > 0 ){
            
            if ( r < 0 ){
                fprintf(stderr, "{ cleaner/tracker %d ~ c-bot}> Error reading %s to avoid trash.",id, nome);
                return;
            }else {
                if (ch == '\n'){
                    w = write(fd2, &line, strlen(line));
                    if( w < 0){
                        fprintf(stderr, "{ cleaner/tracker %d ~ c-bot}> Error writing %s to %s to avoid trash.",id, nome, datafile);
                        return;
                    }
                    
                    memset(&line, 0, 512);
                }else{

                    strcat(line, ch);
                    
                }
                
            }

        }
    }
    
}

void * cleanerwrapper(void *args){

    cleaner_t * c = (cleaner_t*) args;
    cleanerthread_t(c->filename,c->id);
    free(c);

    return NULL;
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