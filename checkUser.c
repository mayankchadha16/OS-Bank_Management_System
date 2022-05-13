#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <time.h>

struct user_data
{
    int user_id;
    char password[30];
    int account_id;
};

int main(int argc, char **argv)
{
    int n;
    int fd = open("USER.dat",O_RDWR); 
    read(fd,&n,sizeof(n));
    struct user_data dat[n];
    read(fd,&dat,sizeof(dat));
    printf("%d\n",n);
    for (int i = 0; i < n; i++)
    {
        printf("User_id - %d Password - %s Account_ID - %d\n",dat[i].user_id,dat[i].password,dat[i].account_id);
    }
    
}