#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <errno.h>

#define PORT_NUMBER 5050

struct user_data
{
    int user_id;
    char password[30];
    int account_id;
};

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

struct admin_data
{
    int admin_id;
    char password[30];
};

void manage_login(int);
void user_login(int, int);
void admin_login(int, int);
struct user_data user_info(int, int);
struct account_data account_info(int, int);
struct admin_data admin_info(int, int);
void admin_operations(int, struct admin_data);
void user_operations(int, struct user_data);
void del_acc(int);
void add_acc(int);
void search_acc(int, int);
void mod_acc(int);
int getNumber();
void deposit(int, int);
int withdraw(int, int);
void pchange(int, int);

int main(int argc, char *argv[])
{
    struct sockaddr_in server_address, client_address;
    int socket_d, client_conn;

    socket_d = socket(AF_INET, SOCK_STREAM, 0);

    int opt = 1;
    setsockopt(socket_d, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT_NUMBER);

    bind(socket_d, (void *)&server_address, sizeof(server_address));

    listen(socket_d, 5);
    int client_address_size = sizeof(client_address);

    while (1)
    {
        if ((client_conn = accept(socket_d, (void *)&client_address, &client_address_size)) > 0)
        {
            if (!fork())
            {
                close(socket_d);
                manage_login(client_conn);
                exit(0);
            }
            else
                close(client_conn);
        }
    }
}

void manage_login(int client_conn)
{
    while (1)
    {
        int login_type;
        read(client_conn, &login_type, sizeof(login_type));

        if (login_type == 1 || login_type == 2)
            user_login(client_conn, login_type);
        else if (login_type == 3)
            admin_login(client_conn, login_type);
        else
        {
            close(client_conn);
            exit(0);
        }
    }
}

void user_login(int client_conn, int type)
{
    write(1, "|Waiting for user to log in....                                    |\n", 70);

    int status = 0;

    int user_id;
    read(client_conn, &user_id, sizeof(user_id));

    char password[30];
    read(client_conn, password, sizeof(password));

    struct user_data user;
    user.account_id = user_info(user_id, client_conn).account_id;
    strcpy(user.password, user_info(user_id, client_conn).password);
    user.user_id = user_info(user_id, client_conn).user_id;

    struct account_data account;
    account.account_id = account_info(user.account_id, client_conn).account_id;
    account.balance = account_info(user.account_id, client_conn).balance;
    account.type = account_info(user.account_id, client_conn).type;

    if (!strcmp(user.password, password) && user_id == user.user_id)
    {
        status = 1;
        write(1, "|User Logged In Successfully                                       |\n", 70);
        write(client_conn, &status, sizeof(status));
        user_operations(client_conn, user);
    }
    else
    {
        write(client_conn, &status, sizeof(status));
        write(1, "|Wrong User Credentials                                            |\n", 70);
    }
}

void admin_login(int client_conn, int type)
{
    write(1, "|Waiting for Admin to log in....                                   |\n", 70);

    int status = 0;

    int admin_id;
    read(client_conn, &admin_id, sizeof(admin_id));

    char password[30];
    read(client_conn, password, sizeof(password));

    struct admin_data admin;
    admin = admin_info(admin_id, client_conn);

    if (!strcmp(admin.password, password) && admin_id == admin.admin_id)
    {
        status = 1;
        write(1, "|Admin Logged In Successfully                                      |\n", 70);
        write(client_conn, &status, sizeof(status));
        admin_operations(client_conn, admin);
    }
    else
    {
        write(client_conn, &status, sizeof(status));
        write(1, "|Wrong Admin Credentials                                           |\n", 70);
    }
}

struct user_data user_info(int user_id, int client_conn)
{
    struct user_data user;

    int fd = open("USER.dat", O_RDWR);

    static struct flock lock;

    lock.l_type = F_RDLCK;
    lock.l_start = (user_id - 1) * sizeof(struct user_data) + sizeof(int);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct user_data);
    lock.l_pid = getpid();

    fcntl(fd, F_SETLK, &lock);

    lseek(fd, (user_id - 1) * sizeof(struct user_data) + sizeof(int), SEEK_SET);
    read(fd, &user, sizeof(struct user_data));

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);

    return user;
}

struct account_data account_info(int account_id, int client_conn)
{
    struct account_data acc;

    int fd = open("DB.dat", O_RDWR);

    static struct flock lock;

    lock.l_type = F_RDLCK;
    lock.l_start = (account_id - 1) * sizeof(struct account_data) + sizeof(int);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct account_data);
    lock.l_pid = getpid();

    fcntl(fd, F_SETLK, &lock);

    lseek(fd, (account_id - 1) * sizeof(struct account_data) + sizeof(int), SEEK_SET);
    read(fd, &acc, sizeof(struct account_data));

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);

    return acc;
}

struct admin_data admin_info(int admin_id, int client_conn)
{
    struct admin_data acc;

    int fd = open("ADMIN.dat", O_RDWR);

    static struct flock lock;

    lock.l_type = F_RDLCK;
    lock.l_start = (admin_id - 1) * sizeof(struct admin_data) + sizeof(int);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct admin_data);
    lock.l_pid = getpid();

    fcntl(fd, F_SETLK, &lock);

    lseek(fd, (admin_id - 1) * sizeof(struct admin_data) + sizeof(int), SEEK_SET);
    read(fd, &acc, sizeof(struct admin_data));

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);

    return acc;
}

void user_operations(int client_conn, struct user_data user)
{
    while (1)
    {
        int option;
        read(client_conn, &option, sizeof(option));

        int checker;

        if (option == 1)
            deposit(client_conn, user.account_id);
        else if (option == 2)
        {
            int possible = withdraw(client_conn, user.account_id);
            write(client_conn, &possible, sizeof(possible));
        }
        else if (option == 3)
            search_acc(client_conn, user.account_id);
        else if (option == 4)
            search_acc(client_conn, user.account_id);
        else if (option == 5)
            pchange(client_conn, user.account_id);
        else if (option == 0)
        {
            close(client_conn);
            exit(0);
        }
        else
        {
            write(1, "|Invalid Choice. Try Again!                                        |\n", 70);
            continue;
        }
    }
}

void admin_operations(int client_conn, struct admin_data admin)
{
    while (1)
    {
        int option;
        read(client_conn, &option, sizeof(option));

        int checker;

        if (option == 1)
            add_acc(client_conn);
        else if (option == 2)
            del_acc(client_conn);
        else if (option == 3)
            mod_acc(client_conn);
        else if (option == 4)
        {
            int account_id;
            read(client_conn, &account_id, sizeof(account_id));
            search_acc(client_conn, account_id);
        }
        else if (option == 0)
        {
            close(client_conn);
            exit(0);
        }
        else
        {
            write(1, "|Invalid Choice. Try Again!                                        |\n", 70);
            continue;
        }
    }
}

void add_acc(int client_conn)
{
    struct account_data account;

    read(client_conn, &account.balance, sizeof(account.balance));
    read(client_conn, &account.type, sizeof(account.type));
    account.account_id = getNumber();

    int fd = open("DB.dat", O_RDWR);
    static struct flock lock;

    lock.l_type = F_WRLCK;
    lock.l_start = ((account.account_id - 1) * sizeof(struct account_data) + sizeof(int));
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct account_data);
    lock.l_pid = getpid();

    fcntl(fd, F_SETLK, &lock);

    lseek(fd, 0, SEEK_SET);
    write(fd, &account.account_id, sizeof(account.account_id));
    lseek(fd, (account.account_id - 1) * sizeof(struct account_data) + sizeof(int), SEEK_SET);
    write(fd, &account, sizeof(struct account_data));

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);
}

int getNumber()
{
    int fd = open("DB.dat", O_RDWR);

    static struct flock lock;
    int highest;

    lock.l_type = F_RDLCK;
    lock.l_start = SEEK_SET;
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(int);
    lock.l_pid = getpid();

    fcntl(fd, F_SETLK, &lock);

    lseek(fd, 0, SEEK_SET);
    read(fd, &highest, sizeof(highest));
    highest++;
    lseek(fd, 0, SEEK_SET);
    write(fd, &highest, sizeof(highest));

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);

    return highest;
}

void del_acc(int client_conn)
{
    int account_id_del;
    int num2;
    read(client_conn, &account_id_del, sizeof(account_id_del));

    struct account_data account;

    int fd = open("DB.dat", O_RDWR);

    static struct flock lock;

    lock.l_type = F_WRLCK;
    lock.l_start = (account_id_del - 1) * sizeof(struct account_data) + sizeof(int);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct account_data);
    lock.l_pid = getpid();

    fcntl(fd, F_SETLK, &lock);

    lseek(fd, 0, SEEK_SET);
    read(fd, &num2, sizeof(int));
    num2 -= 1;
    lseek(fd, 0, SEEK_SET);
    write(fd, &num2, sizeof(int));
    lseek(fd, (account_id_del - 1) * sizeof(struct account_data) + sizeof(int), SEEK_SET);
    read(fd, &account, sizeof(struct account_data));
    lseek(fd, (account_id_del - 1) * sizeof(struct account_data) + sizeof(int), SEEK_SET);
    account.account_id = -1;
    write(fd, &account, sizeof(struct account_data));

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);

    int fd1 = open("USER.dat", O_RDWR);

    int num;

    struct user_data user;
    lock.l_type = F_WRLCK;
    lock.l_start = 0;
    lock.l_whence = SEEK_SET;
    lock.l_len = 0;
    lock.l_pid = getpid();

    read(fd1, &num, sizeof(num));

    fcntl(fd1, F_SETLK, &lock);

    while (read(fd1, &user, sizeof(struct user_data)))
    {
        if (user.account_id == account_id_del)
        {
            user.account_id = -1;
            lseek(fd1, -1 * sizeof(struct user_data), SEEK_CUR);
            write(fd1, &user, sizeof(struct user_data));
        }
    }

    lock.l_type = F_UNLCK;
    fcntl(fd1, F_SETLK, &lock);

    close(fd1);
}

void mod_acc(int client_conn)
{
    int account_id_mod;
    read(client_conn, &account_id_mod, sizeof(account_id_mod));

    struct account_data account;

    int fd = open("DB.dat", O_RDWR);

    static struct flock lock;

    lock.l_type = F_WRLCK;
    lock.l_start = (account_id_mod - 1) * sizeof(struct account_data) + sizeof(int);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct account_data);
    lock.l_pid = getpid();

    fcntl(fd, F_SETLK, &lock);

    lseek(fd, (account_id_mod - 1) * sizeof(struct account_data) + sizeof(int), SEEK_SET);
    read(fd, &account, sizeof(struct account_data));

    if (account.type == 1)
        account.type = 2;
    else
        account.type = 1;

    lseek(fd, (account_id_mod - 1) * sizeof(struct account_data) + sizeof(int), SEEK_SET);
    write(fd, &account, sizeof(struct account_data));

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);
}

void search_acc(int client_conn, int account_id)
{
    struct account_data account;
    account = account_info(account_id, client_conn);

    write(client_conn, &account, sizeof(struct account_data));
}

void deposit(int client_conn, int account_id)
{
    int amt;
    read(client_conn, &amt, sizeof(amt));

    struct account_data account;

    int fd = open("DB.dat", O_RDWR);

    static struct flock lock;

    lock.l_type = F_WRLCK;
    lock.l_start = (account_id - 1) * sizeof(struct account_data) + sizeof(int);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct account_data);
    lock.l_pid = getpid();

    fcntl(fd, F_SETLK, &lock);

    lseek(fd, (account_id - 1) * sizeof(struct account_data) + sizeof(int), SEEK_SET);
    read(fd, &account, sizeof(struct account_data));

    account.balance = account.balance + amt;

    lseek(fd, (account_id - 1) * sizeof(struct account_data) + sizeof(int), SEEK_SET);
    write(fd, &account, sizeof(struct account_data));

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);
}

int withdraw(int client_conn, int account_id)
{
    int amt;
    read(client_conn, &amt, sizeof(amt));

    int checker = 1;

    struct account_data account;

    int fd = open("DB.dat", O_RDWR);

    static struct flock lock;

    lock.l_type = F_WRLCK;
    lock.l_start = (account_id - 1) * sizeof(struct account_data) + sizeof(int);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct account_data);
    lock.l_pid = getpid();

    fcntl(fd, F_SETLK, &lock);

    lseek(fd, (account_id - 1) * sizeof(struct account_data) + sizeof(int), SEEK_SET);
    read(fd, &account, sizeof(struct account_data));

    if (account.balance < amt)
        checker = 0;
    else
    {
        account.balance = account.balance - amt;

        lseek(fd, (account_id - 1) * sizeof(struct account_data) + sizeof(int), SEEK_SET);
        write(fd, &account, sizeof(struct account_data));
    }

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);

    return checker;
}

void pchange(int client_conn, int user_id)
{
    char newpass[30];
    read(client_conn, &newpass, sizeof(newpass));

    struct user_data user;
    int fd = open("USER.dat", O_RDWR);

    static struct flock lock;

    lock.l_type = F_WRLCK;
    lock.l_start = (user_id - 1) * sizeof(struct user_data) + sizeof(int);
    lock.l_whence = SEEK_SET;
    lock.l_len = sizeof(struct user_data);
    lock.l_pid = getpid();

    fcntl(fd, F_SETLK, &lock);

    lseek(fd, (user_id - 1) * sizeof(struct user_data) + sizeof(int), SEEK_SET);
    read(fd, &user, sizeof(struct user_data));

    strcpy(user.password, newpass);

    lseek(fd, (user_id - 1) * sizeof(struct user_data) + sizeof(int), SEEK_SET);
    write(fd, &user, sizeof(struct user_data));

    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);
}
