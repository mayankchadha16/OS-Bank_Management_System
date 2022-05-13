#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <time.h>

enum type
{
    Normal = 1,
    Joint = 2,
    Administrator = 3
};

struct account_data
{
    int account_id;
    int balance;
    int type;
};

int main(int argc, char **argv)
{
    int n;
    int fd = open("DB.dat",O_RDWR); 
    read(fd,&n,sizeof(n));
    struct account_data dat[n];
    read(fd,&dat,sizeof(dat));
    printf("%d\n",n);
    for (int i = 0; i < n; i++)
    {
        printf("Account_id - %d Balance - %d Type - %d\n",dat[i].account_id,dat[i].balance,dat[i].type);
    }
    
}