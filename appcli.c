#include "appcli.h"

void ShowCerts(SSL* ssl)
{   X509 *cert;
    char *line;
 
    cert = SSL_get_peer_certificate(ssl); /* get the server's certificate */
    if ( cert != NULL )
    {
        printf("Server certificates:\n");
        line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
        printf("Subject: %s\n", line);
        free(line);       /* free the malloc'ed string */
        line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
        printf("Issuer: %s\n", line);
        free(line);       /* free the malloc'ed string */
        X509_free(cert);     /* free the malloc'ed certificate copy */
    }
    else
        printf("Info: No client certificates configured.\n");
}

/*int socket_connect(){

    char *header = "GET /v2/ticker/1/ HTTP/1.1\r\nHost: api.coinmarketcap.com\r\n\r\n";
	struct addrinfo ping;
    struct addrinfo *r, *res;
	int sock, byte_count;
    char buffer[4112]={0};

    memset(&ping, 0, sizeof(struct addrinfo));

    ping.ai_family = AF_UNSPEC;
    ping.ai_socktype = SOCK_STREAM;
    ping.ai_flags = AI_PASSIVE;

    if( getaddrinfo("api.coinmarketcap.com", "80", &ping, &res) != 0 ){
		fprintf(stderr, "%s[CONNECTION:ERROR]%s Could not resolve hostname %s. \n",RED,RESET,CMCURL);
		exit(1);
    }

    fprintf(stdout,"%s[CONSOLE]%s> API is online at %s\n", CYAN, RESET,"api.coinmarketcap.com");
    sock = socket(res->ai_family,res->ai_socktype,res->ai_protocol);

	if(sock == -1){
		fprintf(stderr, "%s[CONNECTION:ERROR]%s Could not open socket to connect to %s.\n",RED,RESET,CMCURL);
		exit(1);
	}
	
    for (r = res; r != NULL; r = r->ai_next) {
        sock = socket(r->ai_family, r->ai_socktype,r->ai_protocol);
        if (sock == -1)
            continue;

        if (connect(sock, r->ai_addr, r->ai_addrlen) != -1)
            break;                   Success 

        close(sock);
    }

    if( r == NULL ){
        fprintf(stderr, "%s[CONNECTION:ERROR]%s Could not estabilish connection to %s.\n",RED,RESET,CMCURL);
        exit(1);
    }
    
    freeaddrinfo(res);
    
    send(sock,header,strlen(header),0);

    byte_count = recv(sock,buffer,sizeof(buffer)-1,0); // <-- -1 to leave room for a null terminator
    buffer[byte_count-1] = '\0'; // <-- add the null terminator
    fprintf(stdout,"%s[CONSOLE]%s> Received %d bytes from API.\n", CYAN, RESET,byte_count);
    printf("%s",buffer);

	return sock;
}*/
 

int main(int argc, char **argv){

    char buffer[MAXLEN] /*input*/, *args[64] /*partitioned input*/;
    int cts=0, stc=0, len = 0, numargs=0;

    char *header = "GET /v2/ticker/1/ HTTP/1.1\r\nHost: api.coinmarketcap.com\r\n\r\n";
	struct addrinfo ping;
    struct addrinfo *r, *res;
	int sock=-1, byte_count, err;
    char bufferssl[4112];

    long ressl = 1;

    SSL_CTX* ctx = NULL;
    BIO *web = NULL, *out = NULL;
    SSL *ssl = NULL;

    SSL_library_init ();
    OpenSSL_add_all_algorithms();  /* Load cryptos, et.al. */
    SSL_load_error_strings();   /* Bring in and register error messages */

    const SSL_METHOD* method = SSLv23_method();
    if(!(NULL != method)) fprintf(stderr,"%s[FATAL:ERROR]%s SSL Method.\n",RED,RESET);

    SSL_CTX *ssl_ctx = SSL_CTX_new (SSLv23_client_method ());
    SSL *conn = SSL_new(ssl_ctx);

    memset(&ping, 0, sizeof(struct addrinfo));

    ping.ai_family = AF_INET;
    ping.ai_socktype = SOCK_STREAM;
    ping.ai_flags = AI_PASSIVE;

    /* Cannot fail ??? */
    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, verify_callback);

    /* Cannot fail ??? */
    SSL_CTX_set_verify_depth(ctx, 4);

    /* Cannot fail ??? */
    const long flags = SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_COMPRESSION;
    SSL_CTX_set_options(ctx, flags);
	
	
    if( (  cts = open(FIFOWRITE , O_WRONLY)  < 0 ) || ( ( stc = open(FIFOREAD, O_RDONLY) ) < 0) ){
        fprintf(stderr,"%s[CONNECTION:ERROR]%s Could not estabilish connection to server.\n",RED,RESET);
        sleep(2);
        fprintf(stderr,"%s[CONNECTION:ERROR]%s Could not estabilish connection to server..\n",RED,RESET);
        sleep(2);
        fprintf(stderr,"%s[CONNECTION:ERROR]%s Could not estabilish connection to server...\n",RED,RESET);
        sleep(2);
        return main(argc, argv);
    }else{       

        fprintf(stdout,"\n    	>> Welcome to the Crypto Asset Price Logger <<          \n");
        fprintf(stdout,"%s[CONSOLE]%s> Logging Server is online.\n",CYAN, RESET);

        if( sock == -1 ){
            
            if( getaddrinfo("api.coinmarketcap.com", "443", &ping, &res) != 0 ){
		        fprintf(stderr, "%s[CONNECTION:ERROR]%s Could not resolve hostname %s. \n",RED,RESET,CMCURL);
		        exit(1);
            }

            fprintf(stdout,"%s[CONSOLE]%s> API is online at %s\n", CYAN, RESET,"api.coinmarketcap.com");
            sock = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
            
            if(sock == -1){
		        fprintf(stderr, "%s[CONNECTION:ERROR]%s Could not open socket to connect to %s.\n",RED,RESET,CMCURL);
		        exit(1);
	        }

            for(r = res; r != NULL; r = r->ai_next){

                sock = socket(r->ai_family, r->ai_socktype,r->ai_protocol);
                SSL_set_fd(conn, sock);
            
                err = SSL_connect(conn);
                if (err != 1){
                    continue;
                }else{
                    break;
                }
            }

            if( r == NULL ){
                fprintf(stderr, "%s[CONNECTION:ERROR]%s Could not reach servers at %s.\n",RED,RESET,CMCURL);
                exit(1);
            }

            /*SSL_set_fd(conn, sock);
            
            err = SSL_connect(conn);
            if (err != 1){
                fprintf(stderr, "%s[CONNECTION:ERROR]%s Could not estabilish connection to %s.\n",RED,RESET,CMCURL);
                exit(1);
            }*/

            SSL_write(conn, header, strlen(header));

            byte_count = SSL_read(conn, bufferssl, strlen(buffer)-1); // <-- -1 to leave room for a null terminator
            bufferssl[byte_count-1] = '\0'; // <-- add the null terminator
            fprintf(stdout,"%s[CONSOLE]%s> Received %d bytes from API.\n", CYAN, RESET,byte_count);
            printf("%s",buffer);

        }

        while(1){

            fprintf(stdout,"%s[CONSOLE]%s>", CYAN, RESET);

            if ( fgets(buffer,1023,stdin) == NULL){
		        fprintf(stdout,"%s[CONSOLE]%s>", CYAN, RESET);
	        }

            if( (len = strlen(buffer)) == 1) continue;

            if( buffer[len-1] == '\n' ) buffer[len-1] = '\0';

            numargs = parse_appcli(buffer, args);

            if ( !cmd(numargs, args) ){
                fprintf(stdout,"%s[CONSOLE]%s> That is not an available command. Type 'help' for a list of available commands.\n", CYAN, RESET);
                fprintf(stdout,"%s[CONSOLE]%s>", CYAN, RESET);
            }

        }

    }

    return 0;
    
}

void init_openssl_library(void)
{
    /* https://www.openssl.org/docs/ssl/SSL_library_init.html */
    (void)SSL_library_init();
    /* Cannot fail (always returns success) ??? */
    
    /* https://www.openssl.org/docs/crypto/ERR_load_crypto_strings.html */
    SSL_load_error_strings();
    /* Cannot fail ??? */
    
    /* SSL_load_error_strings loads both libssl and libcrypto strings */
    ERR_load_crypto_strings(); 
    /* Cannot fail ??? */
    
    /* OpenSSL_config may or may not be called internally, based on */
    /*  some #defines and internal gyrations. Explicitly call it    */
    /*  *IF* you need something from openssl.cfg, such as a         */
    /*  dynamically configured ENGINE.                              */
    OPENSSL_config(NULL);
    /* Cannot fail ??? */
    
    /* Include <openssl/opensslconf.h> to get this define     */
#if defined (OPENSSL_THREADS)
    /* TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO */
    /* https://www.openssl.org/docs/crypto/threads.html */
    fprintf(stdout, "Warning: thread locking is not implemented\n");
#endif
}


void print_cn_name(const char* label, X509_NAME* const name)
{
    int idx = -1, success = 0;
    unsigned char *utf8 = NULL;
    
    do
    {
        if(!name) break; /* failed */
        
        idx = X509_NAME_get_index_by_NID(name, NID_commonName, -1);
        if(!(idx > -1))  break; /* failed */
        
        X509_NAME_ENTRY* entry = X509_NAME_get_entry(name, idx);
        if(!entry) break; /* failed */
        
        ASN1_STRING* data = X509_NAME_ENTRY_get_data(entry);
        if(!data) break; /* failed */
        
        int length = ASN1_STRING_to_UTF8(&utf8, data);
        if(!utf8 || !(length > 0))  break; /* failed */
        
        fprintf(stdout, "  %s: %s\n", label, utf8);
        success = 1;
        
    } while (0);
    
    if(utf8)
        OPENSSL_free(utf8);
    
    if(!success)
        fprintf(stdout, "  %s: <not available>\n", label);
}

void print_san_name(const char* label, X509* const cert)
{
    int success = 0;
    GENERAL_NAMES* names = NULL;
    unsigned char* utf8 = NULL;
    
    do
    {
        if(!cert) break; /* failed */
        
        names = X509_get_ext_d2i(cert, NID_subject_alt_name, 0, 0 );
        if(!names) break;
        
        int i = 0, count = sk_GENERAL_NAME_num(names);
        if(!count) break; /* failed */
        
        for( i = 0; i < count; ++i )
        {
            GENERAL_NAME* entry = sk_GENERAL_NAME_value(names, i);
            if(!entry) continue;
            
            if(GEN_DNS == entry->type)
            {
                int len1 = 0, len2 = -1;
                
                len1 = ASN1_STRING_to_UTF8(&utf8, entry->d.dNSName);
                if(utf8) {
                    len2 = (int)strlen((const char*)utf8);
                }
                
                if(len1 != len2) {
                    fprintf(stderr, "  Strlen and ASN1_STRING size do not match (embedded null?): %d vs %d\n", len2, len1);
                }
                
                /* If there's a problem with string lengths, then     */
                /* we skip the candidate and move on to the next.     */
                /* Another policy would be to fails since it probably */
                /* indicates the client is under attack.              */
                if(utf8 && len1 && len2 && (len1 == len2)) {
                    fprintf(stdout, "  %s: %s\n", label, utf8);
                    success = 1;
                }
                
                if(utf8) {
                    OPENSSL_free(utf8), utf8 = NULL;
                }
            }
            else
            {
                fprintf(stderr, "  Unknown GENERAL_NAME type: %d\n", entry->type);
            }
        }

    } while (0);
    
    if(names)
        GENERAL_NAMES_free(names);
    
    if(utf8)
        OPENSSL_free(utf8);
    
    if(!success)
        fprintf(stdout, "  %s: <not available>\n", label);
    
}

int verify_callback(int preverify, X509_STORE_CTX* x509_ctx)
{
    /* For error codes, see http://www.openssl.org/docs/apps/verify.html  */
    
    int depth = X509_STORE_CTX_get_error_depth(x509_ctx);
    int err = X509_STORE_CTX_get_error(x509_ctx);
    
    X509* cert = X509_STORE_CTX_get_current_cert(x509_ctx);
    X509_NAME* iname = cert ? X509_get_issuer_name(cert) : NULL;
    X509_NAME* sname = cert ? X509_get_subject_name(cert) : NULL;
    
    fprintf(stdout, "verify_callback (depth=%d)(preverify=%d)\n", depth, preverify);
    
    /* Issuer is the authority we trust that warrants nothing useful */
    print_cn_name("Issuer (cn)", iname);
    
    /* Subject is who the certificate is issued to by the authority  */
    print_cn_name("Subject (cn)", sname);
    
    if(depth == 0) {
        /* If depth is 0, its the server's certificate. Print the SANs */
        print_san_name("Subject (san)", cert);
    }
    
    if(preverify == 0)
    {
        if(err == X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY)
            fprintf(stdout, "  Error = X509_V_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY\n");
        else if(err == X509_V_ERR_CERT_UNTRUSTED)
            fprintf(stdout, "  Error = X509_V_ERR_CERT_UNTRUSTED\n");
        else if(err == X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN)
            fprintf(stdout, "  Error = X509_V_ERR_SELF_SIGNED_CERT_IN_CHAIN\n");
        else if(err == X509_V_ERR_CERT_NOT_YET_VALID)
            fprintf(stdout, "  Error = X509_V_ERR_CERT_NOT_YET_VALID\n");
        else if(err == X509_V_ERR_CERT_HAS_EXPIRED)
            fprintf(stdout, "  Error = X509_V_ERR_CERT_HAS_EXPIRED\n");
        else if(err == X509_V_OK)
            fprintf(stdout, "  Error = X509_V_OK\n");
        else
            fprintf(stdout, "  Error = %d\n", err);
    }

#if !defined(NDEBUG)
    return 1;
#else
    return preverify;
#endif
}

void print_error_string(unsigned long err, const char* const label)
{
    const char* const str = ERR_reason_error_string(err);
    if(str)
        fprintf(stderr, "%s\n", str);
    else
        fprintf(stderr, "%s failed: %lu (0x%lx)\n", label, err, err);
}
