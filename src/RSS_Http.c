#include "RSS.h"
#include "RSS_Buffer.h"
#include "RSS_Http.h"

#ifndef RSS_NO_HTTP_SUPPORT

#ifdef _WIN32
# include <winsock.h>
# define CLOSESOCKET closesocket
# ifdef _MSC_VER
#  pragma comment(lib, "Ws2_32.lib")
# endif
#else
# include <sys/types.h>
# include <sys/socket.h>
# include <netinet/in.h>
# include <netdb.h>
# include <unistd.h>
# define SOCKET          int
# define SOCKADDR        struct sockaddr
# define SOCKADDR_IN     struct sockaddr_in
# define INVALID_SOCKET  -1
# define SOCKET_ERROR    -1
# define CLOSESOCKET     close
#endif

RSS_Url* RSS_create_url(const RSS_char* url) {
    size_t          len;
    const RSS_char* tmp_host;
    const RSS_char* tmp_path;
    const RSS_char* tmp_port;
    RSS_Url*        rss_url;

    if(!url)
        return NULL;
    
    /* create RSS_Url to return */
    rss_url = (RSS_Url*)malloc(sizeof(RSS_Url));
    
    /* cut the http:// */
    tmp_host = RSS_strstr(url, RSS_text("http://"));
    if(tmp_host)
        tmp_host += 7; /* TODO: check if it will work also on UTF-8 */
    else
        tmp_host = url;

    /* find path */
    tmp_path = RSS_strstr(tmp_host, RSS_text("/"));
    if(tmp_path)
        rss_url->path = RSS_str2char(tmp_path);
    else
        rss_url->path = strdup("/");

    /* find host */
    if(len > 0)    {
        tmp_port = RSS_strstr(tmp_host, RSS_text(":"));
        if (tmp_port)
        {
            rss_url->port = RSS_atoi(tmp_port + 1);
            len = RSS_strlen(tmp_host) - strlen(tmp_port);
        }
        else
        {
            len = RSS_strlen(tmp_host) - strlen(rss_url->path);
        }
        if(!tmp_path)
        {
            len += 2;
        }
        if (rss_url->port == 0)
        {
            rss_url->port = 80;
        }
        
        rss_url->host = RSS_str2char(tmp_host);
        rss_url->host[len] = 0;
    } else {
        /* no host info, url is useless */
        if(rss_url->path)
            free(rss_url->path);
        free(rss_url);
        return NULL;
    }

    return rss_url;
}

void RSS_free_url(RSS_Url* url) {
    if(!url)
        return;

    if(url->host)
        free(url->host);

    if(url->path);
        free(url->path);

    free(url);
}

/* Under Windows: REMEMBER TO INIT WINSOCK (WSAStartup, WSACleanUp) */
RSS_Http_error RSS_http_get_page(const RSS_Url* url, char** buffer) {
    SOCKET          sock;
    struct hostent* remoteHost;
    SOCKADDR_IN     sin;
    char*           header_buff;
    int             s; /* for recv size */
    int             received;
    int             buffer_size;
    char            recv_buff[4096];
    char*           data;
    char*           tmp_buff;

    if(!url || !url->host || !url->path)
        return RSS_HTTP_BAD_ARG;

    /* Create socket */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == INVALID_SOCKET)
        return RSS_HTTP_SOCKET;

    /* Get hosts IP */
    remoteHost = gethostbyname(url->host);        
    if(remoteHost == NULL) {
        CLOSESOCKET(sock);
        return RSS_HTTP_GETHOSTBYNAME;
    }

    if(remoteHost->h_addrtype != AF_INET) {
        CLOSESOCKET(sock);
        return RSS_HTTP_NOT_IPV4;
    }

    /* Connect */
    memset(&sin, 0, sizeof(SOCKADDR_IN));
    /* TODO: this can be not x64 compatybile */
    sin.sin_addr.s_addr = *(unsigned long*)remoteHost->h_addr_list[0];
    sin.sin_family = AF_INET;
    sin.sin_port = htons(url->port);

    if(connect(sock, (SOCKADDR *)&sin, sizeof(sin)) == SOCKET_ERROR) {
        CLOSESOCKET(sock);
        return RSS_HTTP_CONN_FAILED;
    }

    /* Send headers */
    s = sizeof(RSS_HTTP_HEADER1) + 
        sizeof(RSS_HTTP_HEADER2) +
        sizeof(RSS_HTTP_HEADER3) +
        strlen(url->host) +
        strlen(url->path) + 3;
    header_buff = (char*)malloc(s+1);
    /* TODO: port in host header */
    sprintf(header_buff, 
        RSS_HTTP_HEADER1 \
        "%s" \
        RSS_HTTP_HEADER2 \
        "%s" \
        RSS_HTTP_HEADER3, url->path, url->host);
    
    if(send(sock, header_buff, (int)strlen(header_buff), 0) <= 0) {
        free(header_buff);
        CLOSESOCKET(sock);
        return RSS_HTTP_SEND_FAILED;
    }
    free(header_buff);

    /* Get response */
    received = 0;
    tmp_buff = (char*)malloc(RSS_HTTP_INITIAL_BUFFER_SIZE);
    buffer_size = RSS_HTTP_INITIAL_BUFFER_SIZE;
    while((s = recv(sock, recv_buff, 4096, 0)) > 0)    {
        if(received + s >= buffer_size) {
            char*    new_buf;

            new_buf = (char*)malloc(buffer_size << 1);
            if(!new_buf) {
                free(tmp_buff);
                return RSS_HTTP_PAGE_TOO_BIG;
            }

            memcpy(new_buf, tmp_buff, buffer_size);
            buffer_size <<= 1;
            free(tmp_buff);
            tmp_buff = new_buf;
        }

        memcpy(tmp_buff+received, recv_buff, s);
        received += s;
    }
    tmp_buff[received] = 0;

    /* Check return code */
    if(received < 13) {
        CLOSESOCKET(sock);
        free(tmp_buff);
        return RSS_HTTP_NOT_200;
    }

    /* TODO: 50x - redirection */
    if(tmp_buff[9] != '2' || tmp_buff[10] != '0' || tmp_buff[11] != '0') {
        CLOSESOCKET(sock);
        free(tmp_buff);
        return RSS_HTTP_NOT_200;
    }
    
    data = strstr(tmp_buff, "\r\n\r\n");
    if(!data) {
        CLOSESOCKET(sock);
        free(tmp_buff);
        return RSS_HTTP_NO_DATA;
    }

    *buffer = strdup(data + 4);

    CLOSESOCKET(sock);
    free(tmp_buff);

    return RSS_HTTP_OK;
}

#endif

RSS_char* RSS_url_decode(const RSS_char* url) {
    RSS_Buffer* temp;
    size_t      pos;
    RSS_char*   decoded;

    if(!url)
        return NULL;

    if(RSS_strstr(url, RSS_text("%")) == NULL)
        return RSS_strdup(url);

    temp = RSS_create_buffer();
    pos = 0;
    while(url[pos] != 0) {
        if(url[pos] == RSS_text('+')) {
            RSS_add_buffer(temp, RSS_text(' '));
            pos++;
        } else if(url[pos] == RSS_text('%') && url[pos+1] != 0 && url[pos+2] != 0) {
            RSS_char    hex[2];
            size_t        j;
            int            valid;

            valid = 1;

            for(j=0; j<2; j++)
                hex[j] = url[pos+j+1];

            for(j=0; j<2; j++) {
                if(hex[j] >= L'0' && hex[j] <= L'9')
                    hex[j] -= 0x30;
                else if(hex[j] >= L'A' && hex[j] <= L'F')
                    hex[j] -= 55;
                else if(hex[j] >= L'a' && hex[j] <= L'f')
                    hex[j] -= 87;
                else {
                    valid = 0;
                    break;
                }
            }
            
            if(valid == 1) {
                RSS_add_buffer(temp, (hex[0]<<4) + hex[1]);
                pos += 3;
            } else {
                RSS_add_buffer(temp, RSS_text('%'));
                pos++;
            }
        } else {
            RSS_add_buffer(temp, url[pos]);
            pos++;
        }
    }

    if(temp->len > 0)
        decoded = RSS_strdup(temp->str);
    else
        decoded = NULL;

    RSS_free_buffer(temp);

    return decoded;
}

