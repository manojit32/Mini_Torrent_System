//Manojit Chakraborty
//2018201032

#include <arpa/inet.h>
#include <gcrypt.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <math.h>
#include <assert.h>
#include <stdint.h>
#include <linux/limits.h>
#include <dirent.h>
#include <time.h>
#include <stdlib.h>
#include <termios.h>
#include <pwd.h>
#include <grp.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <thread>
#include <sstream>

#define CHUNK_SIZE 262144
#define SHA1_SIZE 20
#define SIZ_MAX 15

using namespace std;
int SIZ_CHUNK = CHUNK_SIZE*2;
char** args;
char client[2][100];
char tracker1[2][100];
char tracker2[2][100];

typedef struct mtorrent {
    char trackerurl1[SIZ_MAX];
    char trackerurl2[SIZ_MAX];
    int32_t no_chunk;
    unsigned char **chunks;
    char shared_file_name[PATH_MAX];
} mtorrent;

int create_mtorrent(char *, char *,char *);
void get_mtorrent(char *, mtorrent *);
void share_mtorrent(mtorrent);
void free_mtorrent(mtorrent);
void phash(unsigned char *);

void phash(unsigned char *hash) {
    int i;
    unsigned char *p = hash;
    for (i=0; i < SHA1_SIZE; i++, p += 2) {
        printf("%02x", hash[i]);
    }
    printf("\n");
}
void phash_new(unsigned char *hash) {
    int i;
    unsigned char *p = hash;
    for (i=0; i < SHA1_SIZE; i++, p += 2) {
    }
}

void phash2(unsigned char *hash,int fileptr2) {
    int i;
    unsigned char *p = hash;
    for (i=0; i <SHA1_SIZE; i++, p += 2) {
        char* buf;
        snprintf(buf,3,"%02x",hash[i]);
        write(fileptr2,(void*)buf,2);

    }
}

int getch(void)
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}

int is_mtorrent_file(char *fn) {
    char *ext = ".mtorrent";
    char *ptr = fn;
    int is_mtorrent = 0;
    while (strstr(ptr, ext))  {
        ptr = strstr(ptr, ext);
        if(strcmp(ptr, ext) == 0) {
            is_mtorrent = 1;
            break;
        }
        ptr += strlen(ext);
    }
    return is_mtorrent;
}

void cphash(unsigned char *src, unsigned char **dst) {
    *dst = (unsigned char *) malloc(SHA1_SIZE);
    memcpy((void*)*dst, (void*)src, SHA1_SIZE);
}

char *get_file(char *path) {
    char *p;
    if (strchr(path, '/') == NULL)
        p = path;
    else
        p = strrchr(path, '/') + 1;
    return p;
}

int create_mtorrent(char *path, char *host1,char* host2) {
    gcry_md_hd_t hd;
    int fileptr, fptr, tot_num, i,fileptr2;
    int32_t no_chunk;
    char buffer[SIZ_CHUNK];
    unsigned char hash[SHA1_SIZE];
    unsigned char *file_hash = NULL;
    struct stat st;
    unsigned char **chunks;
    char *mtorrent_path, *stripped_path;
    char host_buf[SIZ_MAX];
    char track1[SIZ_MAX];
    char track2[SIZ_MAX];
    if (path == NULL) {
        return -1;
    }
    stripped_path = get_file(path);
    if ((fptr = open(path, O_RDONLY)) == -1) {
        perror("open");
        return -1;
    }
    mtorrent_path = (char*) malloc(strlen(stripped_path)+strlen(".mtorrent")+1);
    sprintf(mtorrent_path, "%s.mtorrent", stripped_path);
    if ((fileptr = creat(mtorrent_path, 0644)) == -1) {
        perror("creat");
        free(mtorrent_path);
        return -1;
    }
    fstat(fptr, &st);
    no_chunk = (int32_t)ceil((double)st.st_size/(double)SIZ_CHUNK);
    chunks =(unsigned char**) malloc(sizeof(char*) * no_chunk);
    gcry_md_open(&hd, GCRY_MD_SHA1, 0);
    fileptr2=creat("hash.txt",0644);
    for(i=0;i<no_chunk;i++){
        tot_num = read(fptr, (void*)buffer, SIZ_CHUNK);
        gcry_md_hash_buffer(GCRY_MD_SHA1, hash, buffer, tot_num);
        phash_new(hash);
        phash2(hash,fileptr2);
        cphash(hash, chunks+i);
        gcry_md_write(hd, (void*)buffer, tot_num);
    }
    assert(read(fptr, (void*)buffer, SIZ_CHUNK) == 0);
    memset(host_buf, 0, SIZ_MAX);
    strcpy(track1,host1);
    strcpy(track2,host2);
    cphash(gcry_md_read(hd, GCRY_MD_SHA1), &file_hash);
    FILE* f = fopen(stripped_path, "r");
    if (f == NULL) {
        printf("File Not Found!\n");
        return -1;
    }
    fseek(f, 0L, SEEK_END);
    long int res = ftell(f);
    fclose(f);
    //printf("%ld\n",res);
    stringstream strs;
    strs << res;
    string temp_str = strs.str();
    char* char_type = (char*) temp_str.c_str();
    write(fileptr, (void*)track1, SIZ_MAX);
    write(fileptr, (void*)track2, SIZ_MAX);
    write(fileptr, (void*)&no_chunk, sizeof(no_chunk));
    for(i=0; i<no_chunk; i++) {
        write(fileptr, (void*)chunks[i], SHA1_SIZE);
        free(chunks[i]);
    }
    write(fileptr, (void*)stripped_path, strlen(stripped_path));
    write(fileptr, (void*)char_type,strlen(char_type));
    free(chunks);
    free(mtorrent_path);
    free(file_hash);
    gcry_md_close(hd);
    close(fptr);
    close(fileptr);
    close(fileptr2);
    return 0;
}

void get_mtorrent(char *path, mtorrent *t,int length) {
    int fptr, i;
    if((fptr = open(path, O_RDONLY)) == -1) {
        perror("open");
        return;
    }
    memset(t, 0, sizeof(*t));
    read(fptr, (void*)t->trackerurl1, SIZ_MAX);
    read(fptr, (void*)t->trackerurl2, SIZ_MAX);
    read(fptr, (void*)&t->no_chunk, sizeof(t->no_chunk));
    t->chunks = (unsigned char**)malloc(sizeof(unsigned char*) * t->no_chunk);
    for(i=0;i<t->no_chunk;i++) {
        t->chunks[i] = (unsigned char *) malloc(SHA1_SIZE);
        read(fptr, (void*)t->chunks[i], SHA1_SIZE);
    }
    read(fptr, (void*)t->shared_file_name, length);
    close(fptr);
}

void remove_mtorrent(mtorrent t)
{
  char message[1000];
  memset(message,0,sizeof(message));
  strcat(message,t.shared_file_name);
  strcat(message," ");
  strcat(message,client[0]);
  strcat(message," ");
  strcat(message,client[1]);
  int csocket3=0;
  csocket3=socket(AF_INET,SOCK_STREAM,0);
  struct sockaddr_in addr3;
  addr3.sin_family = AF_INET;
  addr3.sin_addr.s_addr = inet_addr(tracker1[0]);
  int ab;
  sscanf(tracker1[1],"%d",&ab);
  addr3.sin_port = htons(ab);
  int t2 = connect(csocket3,(struct sockaddr*)&addr3,sizeof(addr3));
  //printf("%d\n",t2);
  if(t2!=0)
  {
    struct sockaddr_in addr5;
    addr5.sin_family = AF_INET;
    addr5.sin_addr.s_addr = inet_addr(tracker2[0]);
    sscanf(tracker2[1],"%d",&ab);
    addr5.sin_port = htons(ab);
    int t3=connect(csocket3,(struct sockaddr*)&addr5,sizeof(addr5));
    //printf("%d\n",t3);
    if(t3==0){
      send(csocket3,message,sizeof(message),0);
    }
    close(csocket3);
  }
  else{
    send(csocket3,message,sizeof(message),0);
    close(csocket3);
  }
}

void share_mtorrent(mtorrent t) {
    int i;
    char message1[1000];
    memset(message1,0,sizeof(message1));
    //printf("%s\n", t.shared_file_name);
    strcat(message1,t.shared_file_name);
    strcat(message1," ");
    char buffer1[SIZ_CHUNK];
    int fptr,tot_num;
    fptr = open("hash.txt",O_RDONLY);
    unsigned char hash[SHA1_SIZE];
    tot_num = read(fptr, (void*)buffer1, SIZ_CHUNK);
    gcry_md_hash_buffer(GCRY_MD_SHA1, hash, buffer1, tot_num);
    unsigned char *p = hash;
    for (i=0; i <SHA1_SIZE; i++, p += 2) {
        char buf[3];
        snprintf(buf,3,"%02x",hash[i]);
        strcat(message1,buf);
    }
    close(fptr);
    strcat(message1," ");
    strcat(message1,client[0]);
    strcat(message1," ");
    strcat(message1,client[1]);
    //printf("%s\n",message1);
    int csocket=0;
    csocket=socket(AF_INET,SOCK_STREAM,0);
    //printf("%d\n",csocket);
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(tracker1[0]);
    int ab;
    sscanf(tracker1[1],"%d",&ab);
    address.sin_port = htons(ab);
    int t1=connect(csocket,(struct sockaddr*)&address,sizeof(address));
    //printf("%d\n",t1);
    if(t1!=0)
    {
      struct sockaddr_in address1;
      address1.sin_family = AF_INET;
      address1.sin_addr.s_addr = inet_addr(tracker2[0]);
      sscanf(tracker2[1],"%d",&ab);
      address1.sin_port = htons(ab);
      int t =connect(csocket,(struct sockaddr*)&address1,sizeof(address1));
      //printf("%d\n",t);
      if(t==0)
      send(csocket,message1,sizeof(message1),0);
    }
    else{
    send(csocket,message1,sizeof(message1),0);
    close(csocket);
    }
}

void exit_client()
{
  char message1[1000];
  memset(message1,0,sizeof(message1));
  strcat(message1,client[0]);
  strcat(message1," ");
  strcat(message1,client[1]);
  int csocket=0;
  csocket=socket(AF_INET,SOCK_STREAM,0);
  //printf("%d\n",csocket);
  struct sockaddr_in address;
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr(tracker1[0]);
  int ab;
  sscanf(tracker1[1],"%d",&ab);
  address.sin_port = htons(ab);
  int t1=connect(csocket,(struct sockaddr*)&address,sizeof(address));
  //printf("%d\n",t1);
  if(t1!=0)
  {
    struct sockaddr_in address1;
    address1.sin_family = AF_INET;
    address1.sin_addr.s_addr = inet_addr(tracker2[0]);
    sscanf(tracker2[1],"%d",&ab);
    address1.sin_port = htons(ab);
    int t =connect(csocket,(struct sockaddr*)&address1,sizeof(address1));
    //printf("%d\n",t);
    if(t==0)
    send(csocket,message1,sizeof(message1),0);
  }
  else{
    send(csocket,message1,sizeof(message1),0);
    close(csocket);
  }
}

void free_mtorrent(mtorrent t) {
    int i;
    for(i=0; i<t.no_chunk; i++) {
        free(t.chunks[i]);
    }
    free(t.chunks);
}


int main(int argc, char **argv) {
    int i;
    args=argv;
    if(argc<5){
      printf("Give Proper Arguments \n");
      return 0;
    }
    printf("\nWrite share <filename> <filename.torrent> for sharing.\nWrite remove <filename.torrent> for removing a torrent file.\nWrite exit to exit from the application\n\n");
    char tr1[100],tr2[100];
    strcpy(tr1,argv[2]);
    strcpy(tr2,argv[3]);
    FILE* logfile=fopen(argv[4],"w");
    fclose(logfile);
    char* token_1;
    i=0;
    token_1=strtok(argv[1],":");
    while(token_1!=NULL)
    {
      strcpy(client[i],token_1);
      i++;
      token_1=strtok(NULL,":");
    }
    i=0;
    char* token_2;
    token_2=strtok(argv[2],":");
    while(token_2!=NULL)
    {
      strcpy(tracker1[i],token_2);
      i++;
      token_2=strtok(NULL,":");
    }
    i=0;
    token_1=strtok(argv[3],":");
    while(token_1!=NULL)
    {
      strcpy(tracker2[i],token_1);
      i++;
      token_1=strtok(NULL,":");
    }
    i=0;
    while(1)
    {
    char ch=getchar();
    char input[100];
    if(ch!='\n'){
        char c[2];
        c[0]=ch;
        c[1]='\0';
        strcat(input,c);
    }
    else{
        char* com;
        char com_token1[3][100];
        int i=0;
        com=strtok(input," ");
        while(com!=NULL)
        {
        strcpy(com_token1[i],com);
        i++;
        com=strtok(NULL," ");
        }
        if(strcmp(com_token1[0],"share")==0){
            create_mtorrent(com_token1[1],tr1,tr2);
            mtorrent t1;
            memset(&t1, 0, sizeof(t1));
            char* newname=get_file(com_token1[1]);
            int length = strlen(newname);
            printf("%d\n",length);
            get_mtorrent(com_token1[2],&t1,length);
            share_mtorrent(t1);
            free_mtorrent(t1);
            printf("SUCCESS : %s\n",com_token1[2]);
            printf("\nWrite share <filename> <filename.torrent> for sharing.\nWrite remove <filename.torrent> for removing a torrent file.\nWrite exit to exit from the application\n\n");
        }
        else if(strcmp(com_token1[0],"remove")==0){
            mtorrent t;
            memset(&t, 0, sizeof(t));
            int length = strlen(com_token1[1])-9;
            get_mtorrent(com_token1[1],&t,length);
            remove_mtorrent(t);
            free_mtorrent(t);
            remove(com_token1[1]);
            printf("SUCCESS:FILE_REMOVED\n");
            printf("\nWrite share <filename> <filename.torrent> for sharing.\nWrite remove <filename.torrent> for removing a torrent file.\nWrite exit to exit from the application\n\n");
        }
        else if(strcmp(com_token1[0],"exit")==0){
            exit_client();
            exit(0);
        }
        memset(input,0,sizeof(input));
        }
    }
    return 0;
}
