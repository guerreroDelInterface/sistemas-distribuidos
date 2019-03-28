#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/wait.h> 

#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <arpa/inet.h>

#include <math.h>

#include <netdb.h> 


#define MAX_NSERVERS 100
#define MAX_NRESOURCES 100
#define MAX_REQUESTS 100
#define MAX_PATH 200
#define PORT 8080 


struct request
{
   char        server_n_path[MAX_PATH];
   int         range[2];
};

struct resource
{
   struct      request request[MAX_REQUESTS];
   int         size;
   char        path[MAX_PATH];
};

struct resources {
   int         n;
   struct      resource resource[MAX_NRESOURCES];
};

struct server {
   int            sockfd;
   char           url[MAX_PATH*2 + 2];
   struct hostent *ip;
};

struct servers {
   int         n;
   struct      server server[MAX_NSERVERS];
};

typedef struct Data {
   struct      resources resources;
   struct      servers servers;
} data;


void parse_arguments(int argc, char* argv[], data *dp);
void set_resources_sizes(data *dp);
int get_size(char* server, char* path);
void create_sockets(data *dp);

void parse_arguments(int argc, char* argv[], data *dp)
{
   dp->servers.n = 0;
   dp->resources.n = 0;

   for(int i=1;i<argc;i++)
   {
      char* tmp = argv[i];
      if(tmp[0] == ':') {
         strncpy(dp->servers.server[dp->servers.n].url, &tmp[1], strlen(tmp) - 1);
         dp->servers.n++;
      } else {
         strcpy(dp->resources.resource[dp->resources.n].path, tmp);
         dp->resources.n++;
      }
   }
}

int get_size(char* server, char* path)
{
   CURL *curl = curl_easy_init();
   char final_path[MAX_PATH];
   int size;
   if(curl) {
      snprintf(final_path, sizeof(final_path), "%s/~%s", server, path);
      curl_easy_setopt(curl, CURLOPT_URL, final_path);
      curl_easy_setopt(curl, CURLOPT_NOBODY, 1L); /* get us the resource without a body! */
      if(curl_easy_perform(curl) == CURLE_OK) {
         curl_easy_getinfo(curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD_T, &size);
      }
   }
   return size;
}

void set_ips(data *dp)
{
   struct hostent *host;
   for(int i = 0; i < dp->servers.n; i++)
   {
      if ((host = gethostbyname(dp->servers.server[0].url)) == NULL) {
         fprintf(stdout,"ERROR, no such host\n");
         exit(1);
      }
      dp->servers.server[i].ip = host;
   }
}

void set_resources_sizes(data *dp)
{
   for(int i = 0; i < dp->resources.n; i++)
   {
      char *server = dp->servers.server[i % dp->servers.n].url;
      char *path = dp->resources.resource[i].path;
      int full_size = get_size(server, path);
      dp->resources.resource[i].size = full_size;
      printf("path: %s/~%s\tsize:%d\n", server, path, dp->resources.resource[i].size);

      float range = ceil(full_size / dp->servers.n);
      for(int j = 0; j < dp->servers.n; j ++)
      {
         float start = j * range;
         float end   = (j+1) * range - 1;
         dp->resources.resource[i].request[j].range[0] = (int)start;
         dp->resources.resource[i].request[j].range[1] = (int)end;
         
         char final_path[MAX_PATH*2 + 4];
         sprintf(final_path, "%s/~%s", dp->servers.server[j].url, path);
         strcpy(dp->resources.resource[i].request[j].server_n_path, final_path);
      }
   }
}

void create_sockets(data *dp)
{
   int sockfd; /* Socket (de tipo TCP) para transmision de datos */
   struct sockaddr_in  server;  /* Direccion TCP servidor */ 

   for(int i = 0; i<dp->resources.n; i++)
   {
      /* Creacion del socket TCP */
      fprintf(stdout,"CLIENTE:  Creacion del socket TCP: ");
      if((sockfd=socket(AF_INET,SOCK_STREAM,0))<0)
      {
         fprintf(stdout,"ERROR\n");
         exit(1);
      }
      fprintf(stdout,"OK\n");

      /* Nos conectamos con el servidor */
      bzero((char*)&server,sizeof(struct sockaddr_in)); /* Pone a 0 la estructura */
      server.sin_family=AF_INET;
      server.sin_port=htons(80);
      server.sin_addr.s_addr = inet_addr("138.100.9.22");

      fprintf(stdout,"CLIENTE:  Conexion al puerto servidor: ");
      if(connect(sockfd, (struct sockaddr*)&server, sizeof(struct sockaddr_in)) < 0)
      {
         fprintf(stdout,"ERROR %d\n",-13);
         close(sockfd); exit(1);
      }
      fprintf(stdout,"OK\n");
      dp->servers.server[i].sockfd = sockfd;
   }
}


int main(int argc, char* argv[])
{
   if(argc <= 2)
   {
      printf("Arg inválidos");
   } else {
   }

   struct Data d;
   
   parse_arguments(argc, argv, &d);


   set_resources_sizes(&d);
   create_sockets(&d);

   return 0;
}