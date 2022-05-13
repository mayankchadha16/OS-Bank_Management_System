#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>

struct user_data
{
    int user_id;
    char password[30];
    int account_id;
};

int main(int argc, char **argv)
{
    int n;
    int fd = open("USER.dat", O_RDWR | O_CREAT | O_APPEND, 0744);

    if (fd == -1)
    {
        perror("open ");
        exit(1);
    }

    write(1, "\e[1;1H\e[2J", 11);
    write(1, "+==================================================================+\n", 70);

    write(1, "|                          MENU TO ADD USERS                       |\n", 70);
    write(1, "+==================================================================+\n", 70);
    write(1, "|How many USERS do you want to add: ", 36);
    scanf("%d", &n);

    struct user_data data[n];
    int user_id = 1;
    int acc_id = 1;

    write(fd, &n, sizeof(n));

    for (int i = 0; i < n; i++)
    {
        write(1, "|==================================================================|\n", 70);

        printf("|User_ID = %d and Account_ID = %d", i + 1, i + 1);
        printf("\n");

        data[i].user_id = user_id++;

        printf("|Enter the Password for USER %d: ", i + 1);
        scanf("%s", data[i].password);

        data[i].account_id = acc_id++;
        write(1, "|==================================================================|\n", 70);
    }

    lseek(fd, 0, SEEK_SET);
    write(fd, data, sizeof(data));
}