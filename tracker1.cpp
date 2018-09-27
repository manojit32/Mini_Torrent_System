//Manojit Chakraborty
//2018201032

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netdb.h> 
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <gcrypt.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <math.h>
#include <assert.h>
#include <stdint.h>
#include <linux/limits.h>
#include <fstream>
#include <malloc.h>
#include <bits/stdc++.h>
#include<pthread.h>

void delete_line(int n);
using namespace std;
struct sockaddr_in address;
char ip[100],port[100],ip2[100],port2[100];
int addrlen = sizeof(address);
char* file_name;
int top=-1;
int ark[1000];

void *in_while(void* new_socket1)
{
  int *k=(int *)new_socket1;
  int new_socket=*k;
  char buffer[1024]={0};
  char buffer1[1024]={0};
  char buffer3[1024]={0};
  int opt=1,cou,valread;
  memset(buffer1,0,sizeof(buffer1));
  memset(buffer,0,sizeof(buffer));
  valread = read( new_socket , buffer1, 1024);
  int sock1=0;
  sock1=socket(AF_INET,SOCK_STREAM,0);
  struct sockaddr_in addr1;
  addr1.sin_family = AF_INET;
  int abb;
  sscanf(port2,"%d",&abb);
  addr1.sin_port = htons(abb);
  if(connect(sock1,(struct sockaddr*)&addr1,sizeof(addr1))==0)
  send(sock1,buffer1,sizeof(buffer1),0);
  char *tokens,*token_1;
  cou=0;
  int g;
  for(g=0;g<=strlen(buffer1);g++)
  {
    buffer[g]=buffer1[g];
  }
  tokens=strtok(buffer," ");
  while(tokens!=NULL)
  {
  cou++;
  tokens=strtok(NULL," ");
}
  FILE* fp1=fopen(file_name,"r+");
  FILE* fp2=fp1;
  char line[1000];
  int flag=0,lineno=1,l=0;
  for(;;)
  {
    int c = fgetc(fp1);
    if (EOF == c) {
       break;
     }
     else if ('\n' == c) {
        if(cou==2)
        {
          char buffer2[1024]={0};
          for(g=0;g<=strlen(buffer1);g++)
          {
            buffer2[g]=buffer1[g];
          }
          flag=1;
          char *token,*token_1;
          char **arr=(char**)malloc(4 * sizeof(char*));
          int coo=0;
          token_1=strtok(line," ");
          while(token_1!=NULL)
          {

            arr[coo]=token_1;
            coo++;
            token_1=strtok(NULL," ");
          }
          int cop=0;
          char **arr1=(char**)malloc(2 * sizeof(char*));
          token_1=strtok(buffer2," ");
          while(token_1!=NULL)
          {
            arr1[cop]=token_1;
            cop++;
            token_1=strtok(NULL," ");
          }
          if(strcmp(arr[2],arr1[0])==0&&strcmp(arr[3],arr1[1])==0)
          {
            l=lineno;
            ark[++top]=l;
          }
          memset(line,0,sizeof(line));
          memset(buffer2,0,sizeof(buffer2));
          memset(buffer,0,sizeof(buffer));
        }
       else if(cou==4)
       {
           if(strcmp(line,buffer1)==0)
           {
               flag=1;
               memset(line,0,sizeof(line));
               break;
             }
       else
         {
         memset(line,0,sizeof(line));
         }
       }
 else if(cou==3)
  {
    char buffer2[1024]={0};
    for(g=0;g<=strlen(buffer1);g++)
    {
      buffer2[g]=buffer1[g];
    }
    flag=1;
    char *token,*token_1;
    char **arr=(char**)malloc(4 * sizeof(char*));
    int coo=0;
    token_1=strtok(line," ");
    while(token_1!=NULL)
    {

      arr[coo]=token_1;
      coo++;
      token_1=strtok(NULL," ");
    }
    int cop=0;
    char **arr1=(char**)malloc(3 * sizeof(char*));
    token_1=strtok(buffer2," ");
    while(token_1!=NULL)
    {
      arr1[cop]=token_1;
      cop++;
      token_1=strtok(NULL," ");
    }
    if(strcmp(arr[0],arr1[0])==0&&strcmp(arr[2],arr1[1])==0&&strcmp(arr[3],arr1[2])==0)
    {
      l=lineno;
    }
    memset(line,0,sizeof(line));
    memset(buffer2,0,sizeof(buffer2));
    memset(buffer,0,sizeof(buffer));
  }
  fp2=fp1;
  lineno++;
  }
  else{
    char ch[2];
    ch[0]=c;
    ch[1]='\0';
    strcat(line,ch);
  }
}
    fclose(fp1);
    if(cou==3)
    {
      delete_line(l);

    }
    if(cou==2)
    {
      for(int i=0;i<=top;i++)
      {
        delete_line(ark[i]);
        for(int j=i+1;j<=top;j++)
        {
          ark[j]=ark[j]-1;
        }
    }
    top=-1;
    }
  if(flag==0&&cou==4){
  FILE *fp=fopen(file_name,"a+");
  fprintf(fp,"%s\n",buffer1);
  fclose(fp);
}
pthread_exit(NULL);
}

void delete_line(int n){
  ifstream is(file_name);
   ofstream ofs;
   ofs.open("temp1.txt", ofstream::out);
   char c;
   int line_no = 1;
   while (is.get(c))
   {
       if (line_no != n)
           ofs << c;
      if (c == '\n')
           line_no++;
   }
   ofs.close();
   is.close();
   remove(file_name);
   rename("temp1.txt", file_name);
}


int main(int argc,char**argv)
{
    if(argc<5){
      printf("Give proper arguments");
      exit(0);
    }
    FILE* logf = fopen(argv[4],"w");
    fclose(logf);
    int server_fd;
    int new_socket;
    char* token1;
    char* app[2];
    int i=0;
    file_name=argv[3];
    token1=strtok(argv[1],":");
    while(token1!=NULL)
    {
      app[i]=token1;
      i++;
      token1=strtok(NULL,":");
    }
    for(i=0;i<=strlen(app[0]);i++)
    {
      ip[i]=app[0][i];
    }
    for(i=0;i<=strlen(app[1]);i++)
    {
      port[i]=app[1][i];
    }
    char *apr[2];
    char *token_1;
    i=0;
    token_1=strtok(argv[2],":");
    while(token_1!=NULL)
    {
    apr[i]=token_1;
      i++;
      token_1=strtok(NULL,":");
    }
    for(i=0;i<=strlen(apr[0]);i++)
    {
      ip2[i]=apr[0][i];
    }
    for(i=0;i<=strlen(apr[1]);i++)
    {
      port2[i]=apr[1][i];
    }
    int opt=1;
      if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
      {
          perror("socket failed");
          exit(EXIT_FAILURE);
      }
      if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt)))
      {
          perror("setsockopt");
          exit(EXIT_FAILURE);
      }
      address.sin_family = AF_INET;
      address.sin_addr.s_addr = inet_addr(ip);
      int ab;
      sscanf(port,"%d",&ab);
      address.sin_port = htons( ab );
      if (bind(server_fd, (struct sockaddr *)&address,sizeof(address))<0)
      {
          perror("bind failed");
          exit(EXIT_FAILURE);
      }
      FILE* seed = fopen(file_name,"ab+");
      fclose(seed);
      while(1)
      {
          if (listen(server_fd, 3) < 0)
          {
              perror("listen");
              exit(EXIT_FAILURE);
          }
          if ((new_socket = accept(server_fd, (struct sockaddr *)&address,(socklen_t*)&addrlen))<0)
          {
              perror("accept");
              exit(EXIT_FAILURE);
          }
          pthread_t th;
          pthread_create(&th,NULL,in_while,(void*)&new_socket);
          pthread_detach(th);
      }
}
