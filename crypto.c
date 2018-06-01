#include "appcli.h"
#include "parse_appcli.c"

JSON_Value *root_listings;
JSON_Value *root_global;
JSON_Value *root_top;
char curl_listings[256]={0};
char curl_global[256]={0};
char curl_teste[256]={0};
char timestring[64]={0};

int main( ) {

    char buffer[MAXLEN], *args[64];
    /*char runfile[]={0};
    char run_curl[]={0};*/

    char cleanup_command[256];
    char listingsfile[] = "data.json";
    char globaldatafile[] = "globaldata.json";
    int cts=0, stc=0, len = 0, numargs=0;

    /* it ain't pretty, but it's not a libcurl tutorial /
     void json_set_allocation_functions( );
    sprintf(curl_listings,
            "curl -s https://api.coinmarketcap.com/v2/listings/  > %s", listingsfile);
    sprintf(curl_global,
            "curl -s https://api.coinmarketcap.com/v2/global/  > %s", globaldatafile);
    sprintf(cleanup_command,
             "rm -f %s", listingsfile);*/
    //system(curl_listings);

    //time = ctime( &t );
    strtime(timestring);
    fprintf(stdout,
        "\n                	                            >> PROJECT %sS%s.%sC%s.%sA%s.%sL%s.%sP%s. << (dev-build 0.0a)\n",
            SCALP,RESET,SCALP,RESET,SCALP,RESET,SCALP,RESET,SCALP,RESET);
    fprintf(stdout,"%s{%s ~ c-bot}%s> Logging Server is online.\n",
    CYAN,
    timestring, RESET);

    
    /* parsing json and validating output */
    /* root das listagens 
    root_listings = json_parse_file(listingsfile);
    if ( json_value_get_type(root_listings) != JSONObject  || root_listings ==NULL) {
        printf("root_listings:\n");
        system(cleanup_command);
        return 0;
    }*/
    /* root do global info 
    root_global = json_parse_file(globaldatafile);

    if ( json_value_get_type(root_global) == JSONError ) {
        printf("root_global: \n");
        system(cleanup_command);
        return 0;
    }*/

    strtime(timestring);
    fprintf(stdout,"%s{%s ~ c-bot}%s> API is online @ %s.\n",
        CYAN,
         timestring,  
          RESET, 
           "api.coinmarketcap.com");
    
    strtime(timestring);
    fprintf(stdout,"%s{%s ~ c-bot}%s> Bot is ready, waiting for a command. Type 'help' for commands.\n",CYAN,timestring, RESET);
    fprintf(stdout,"%s{%s ~ c-bot}%s> ",CYAN,timestring, RESET);

    while(1){
        

        if ( fgets(buffer,1023,stdin) == NULL){
            strtime(timestring);
            fprintf(stdout,"%s{%s ~ c-bot}%s> ",CYAN,timestring, RESET);
        }

        if( (len = strlen(buffer)) < 4) continue;

        if( buffer[len-1] == '\n' ) buffer[len-1] = '\0';

        numargs = parse_appcli(buffer, args);

        if ( !cmd(numargs, args) ){
            strtime(timestring);
            fprintf(stdout,"%s{%s ~ c-bot}%s> That is not an available command. Type 'help' for a list of available commands.\n",CYAN,timestring, RESET);
            fprintf(stdout,"%s{%s ~ c-bot}%s> ",CYAN,timestring, RESET);
        }



    
    /* root do top100
    root_top = json_parse_file(testfile);
            if ( root_top ==NULL) {
                    printf("root_top:  \n");
                    //system(cleanup_command);
                return 0;
            }
    */        
    strtime(timestring);
    fprintf(stdout,"%s{%s ~ c-bot}%s> Ready.\n",CYAN,timestring, RESET);
    fprintf(stdout,"%s{%s ~ c-bot}%s> ",CYAN,timestring, RESET);
    
    //printTopMC(root_top);
    //printGlobalData(root_global);
    //printCoins(root_listings);

    sleep(30);
    strtime(timestring);
    fprintf(stdout,"%s{%s ~ c-bot}%s> Bot is ready, waiting for a command. Type 'help' for commands.\n",CYAN,timestring, RESET);
    fprintf(stdout,"%s{%s ~ c-bot}%s>",CYAN,timestring, RESET);

    }

    /*
    json_value_free(root_top);
    json_value_free(root_listings);
    json_value_free(root_global);
      cleanup code */
    free(curl_teste);
    
    system(cleanup_command);
    return 0;
}

int spawnTracker(char *curl_link){
    
    int pid = -1;

    /*
        sprintf(testfile,
        "%d-%d-%d-%d-%d",
            rand()%10,
             rand()%100,
              rand()%1000,
               rand()%10000,
                rand()%100000);

        sprintf(curl_teste,
        "curl -s https://api.coinmarketcap.com/v2/ticker/?structure=array  > %s",
            testfile);

        system(curl_teste);
    
    
     */

    if( ( pid=fork()) == 0 ){


    }else return pid;

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
    int i = 0,j=0;;

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
            "CIRC. SUPPLY",
             "CHANGE 24H (%)",
              "ASK ($)", 
               RED_BOLD,
                "RANK",
                 "COIN",
                  "CIRC. SUPPLY",
                   "CHANGE 24H (%)",
                    "ASK ($)",RESET);
    for (i = 0; i < 50; i++){
        coin = json_array_get_object(data, i);
        coin2 = json_array_get_object(data, (i+50));
        fprintf(stdout, 
            "%-5.lf %-15.15s (%s$%-5s%s)   %-25.lf %-5.2lf%%       $%-10.1lf| %-5.lf %-15.15s (%s$%-5s%s)   %-25.lf %-5.2lf%%       $%-10.1lf \n",
             json_object_get_number(coin, "rank"),
              json_object_get_string(coin, "name"),
               YELLOW,json_object_get_string(coin,"symbol"),RESET,
                json_object_get_number(coin,"circulating_supply"),
                 json_object_dotget_number(coin,"quotes.USD.percent_change_24h") ,
                  json_object_dotget_number(coin,"quotes.USD.price"),
                   json_object_get_number(coin2, "rank"),
                    json_object_get_string(coin2, "name"),
                     YELLOW,json_object_get_string(coin2,"symbol"),RESET,
                      json_object_get_number(coin2,"circulating_supply"),
                       json_object_dotget_number(coin2,"quotes.USD.percent_change_24h") ,
                        json_object_dotget_number(coin2,"quotes.USD.price")
                    );
    }

}

void strtime(char *buffer){
     
    time_t rawtime;
    struct tm * timeinfo;

    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

    sprintf(buffer, "%d-%d-%d_(%d:%d:%d)",timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

}