#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <unistd.h>

struct admin_data
{
    int admin_id;
    char password[30];
};

int main(int argc, char **argv)
{
    int n;
    int fd = open("ADMIN.dat", O_RDWR | O_CREAT | O_APPEND, 0744);

    if (fd == -1)
    {
        perror("open ");
        exit(1);
    }

    write(1, "\e[1;1H\e[2J", 11);
    write(1, "+==================================================================+\n", 70);

    write(1, "|                     MENU TO ADD ADMINISTRATORS                   |\n", 70);
    write(1, "+==================================================================+\n", 70);
    write(1, "|How many ADMINS do you want to add: ", 38);
    scanf("%d", &n);

    struct admin_data data[n];
    write(fd, &n, sizeof(n));
    int admin_id = 1;

    for (int i = 0; i < n; i++)
    {
        write(1, "|==================================================================|\n", 70);

        data[i].admin_id = admin_id++;

        printf("|Enter the Password for ADMIN %d: ", i + 1);
        scanf("%s", data[i].password);

        write(1, "|==================================================================|\n", 70);
    }

    lseek(fd, 0, SEEK_SET);
    write(fd, data, sizeof(data));

    close(fd);
}