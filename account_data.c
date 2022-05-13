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
    int count = 0;

    int fd = open("DB.dat", O_RDWR | O_CREAT | O_APPEND, 0744);

    if (fd == -1)
    {
        perror("open ");
        exit(1);
    }

    write(1, "\e[1;1H\e[2J", 11);
    write(1, "+==================================================================+\n", 70);

    write(1, "|                        MENU TO ADD ACCOUNTS                      |\n", 70);
    write(1, "+==================================================================+\n", 70);

    write(1, "|How many ACCOUNTS do you want to add: ", 39);
    scanf("%d", &n);

    struct account_data data[n];
    write(fd, &n, sizeof(n));
    int counter = 1;

    while (1)
    {
        int balance = 0;
        int type;

        srand(time(0));

        write(1, "|==================================================================|\n", 70);

        write(1, "|Sequentially Assigning Account ID....                             |", 69);

        int id = counter++;

    out:

        write(1, "\n|Starting balance is 0                                             |\n", 71);
        write(1, "|Do you want to add a starting balance: 1 for YES, 0 for NO: ", 61);

        int check;
        scanf("%d", &check);

        if (check == 1)
        {
            write(1, "|Starting balance is : ", 24);
            scanf("%d", &balance);
        }

        else if (check == 0)
            balance = 0;

        else
        {
            write(1, "|                                                                  |\n", 70);
            write(1, "|Invalid Input. Try Again...                                       |", 69);
            goto out;
        }

        write(1, "|==================================================================|\n", 70);

        write(1, "|Tell us your account type                                         |\n", 70);
        write(1, "|==================================================================|\n", 70);

        write(1, "|If you want to open a Normal Account - Enter 1                    |\n", 70);
        write(1, "|If you want to open a Joint Account - Enter 2                     |\n", 70);
        write(1, "|If you want to open a Administrator Account - Enter 3             |\n", 71);

    again:

        write(1, "|Press the Appropriate Button: ", 31);
        scanf("%d", &type);

        if (type != 1 && type != 2 && type != 3)
        {
            write(1, "|                                                                  |\n", 70);
            write(1, "|Invalid Input. Try Again...                                       |\n", 70);
            goto again;
        }

        data[count].type = type;
        data[count].account_id = id;
        data[count].balance = balance;

        count++;
        write(1, "|==================================================================|\n", 70);

        if (count == n)
            break;
    }

    for (int i = 0; i < n; i++)
    {
        printf("Account_id - %d Balance - %d Type - %d\n",data[i].account_id,data[i].balance,data[i].type);
    }

    lseek(fd, 0, SEEK_SET);
    write(fd, data, sizeof(data));

    close(fd);
}