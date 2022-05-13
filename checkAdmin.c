#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>
#include <time.h>

struct admin_data
{
    int admin_id;
    char password[30];
};

int main(int argc, char **argv)
{
    int n;
    int fd = open("ADMIN.dat",O_RDWR); 
    read(fd,&n,sizeof(n));
    struct admin_data dat[n];
    read(fd,&dat,sizeof(dat));
    printf("%d\n",n);
    for (int i = 0; i < n; i++)
    {
        printf("Admin_ID - %d Password - %s\n",dat[i].admin_id,dat[i].password);
    }
    
}