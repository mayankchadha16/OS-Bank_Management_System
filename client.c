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

void login_manager(int);
void user_operations(int);
int password_validator(int, char pass[30], int);
void admin_operation(int);
void add_acc(int, int);
void del_acc(int, int);
void mod_acc(int, int);
void search_acc(int, int);
void deposit(int, int);
void withdraw(int, int);
void details(int, int);
void balance(int, int);
void pchange(int, int);

int main(int argc, char *argv[])
{
    struct sockaddr_in server_address;
    int socket_d;

    socket_d = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT_NUMBER);
    connect(socket_d, (void *)&server_address, sizeof(server_address));

    login_manager(socket_d);
    user_operations(socket_d);
}

void user_operations(int socket_d)
{
    while (1)
    {
        write(1, "+==================================================================+\n", 70);
        write(1, "|                   MENU FOR ACCESSING USER OPERATIONS             |\n", 70);
        write(1, "+==================================================================+\n", 70);
        write(1, "|1|-> Deposit                                                      |\n", 70);
        write(1, "|2|-> Withdraw                                                     |\n", 70);
        write(1, "|3|-> Account Details                                              |\n", 70);
        write(1, "|4|-> Balance Enquiry                                              |\n", 70);
        write(1, "|5|-> Password Change                                              |\n", 70);
        write(1, "+==================================================================+\n", 70);
        write(1, "|0|-> Exit                                                         |\n", 70);
        write(1, "+==================================================================+\n", 70);

        int menu_id;
        write(1, "|Select an Operation to Perform : ", 35);
        scanf("%d", &menu_id);

        if (menu_id == 1)
            deposit(socket_d, menu_id);
        else if (menu_id == 2)
            withdraw(socket_d, menu_id);
        else if (menu_id == 4)
            balance(socket_d, menu_id);
        else if (menu_id == 5)
            pchange(socket_d, menu_id);
        else if (menu_id == 3)
            details(socket_d, menu_id);
        else if (menu_id == 0)
        {
            write(1, "|Sayonara!                                                         |\n", 70);
            exit(1);
        }
        else
        {
            write(1, "|Enter a valid Choice                                              |\n", 70);
            continue;
        }
    }
}

void login_manager(int socket_d)
{
    while (1)
    {
        write(1, "+==================================================================+\n", 70);
        write(1, "|                           MENU FOR LOGGING IN                    |\n", 70);
        write(1, "+==================================================================+\n", 70);
        write(1, "|1|-> Login as Normal Account Holder                               |\n", 70);
        write(1, "|2|-> Login as Joint Account Holder                                |\n", 70);
        write(1, "|3|-> Login as Administrator                                       |\n", 70);
        write(1, "+==================================================================+\n", 70);
        write(1, "|0|-> Exit                                                         |\n", 70);
        write(1, "+==================================================================+\n", 70);

        int menu_id;
        write(1, "|Select an Operation to Perform : ", 35);
        scanf("%d", &menu_id);

        if (menu_id == 1)
            write(socket_d, &menu_id, sizeof(int));
        else if (menu_id == 2)
            write(socket_d, &menu_id, sizeof(int));
        else if (menu_id == 3)
            write(socket_d, &menu_id, sizeof(int));
        else if (menu_id == 0)
        {
            write(1, "|Sayonara!                                                         |\n", 70);
            exit(0);
        }
        else
        {
            write(1, "|Enter a valid Choice                                              |\n", 70);
            continue;
        }

        write(1, "|Starting the Login Process Now...                                 |\n", 70);

        int id;
        write(1, "|Enter your User_ID: ", 22);
        scanf("%d", &id);

        char pass[30];
        write(1, "|Enter your Password: ", 23);
        scanf("%s", pass);

        write(socket_d, &id, sizeof(id));
        write(socket_d, &pass, sizeof(pass));

        int valid;
        read(socket_d, &valid, sizeof(valid));

        if (valid)
        {
            write(1, "|Logged in successfully!                                           |\n", 70);
            if (menu_id == 1 || menu_id == 2)
                user_operations(socket_d);
            else
                admin_operation(socket_d);
        }
        else
        {
            write(1, "|Invalid Credentials!                                              |\n", 70);
            write(1, "|Try Again!                                                        |\n", 70);
        }
    }
}

void admin_operation(int socket_d)
{
    while (1)
    {
        write(1, "+==================================================================+\n", 70);
        write(1, "|                           MENU FOR LOGGING IN                    |\n", 70);
        write(1, "+==================================================================+\n", 70);
        write(1, "|1|-> Add Account to the Database                                  |\n", 70);
        write(1, "|2|-> Delete Account to the Database                               |\n", 70);
        write(1, "|3|-> Modify Account Type                                          |\n", 70);
        write(1, "|4|-> Search Account Details                                       |\n", 70);
        write(1, "+==================================================================+\n", 70);
        write(1, "|0|-> Exit                                                         |\n", 70);
        write(1, "+==================================================================+\n", 70);

        int menu_id;
        write(1, "|Select an Operation to Perform : ", 35);
        scanf("%d", &menu_id);

        if (menu_id == 1)
            add_acc(socket_d, menu_id);
        else if (menu_id == 2)
            del_acc(socket_d, menu_id);
        else if (menu_id == 3)
            mod_acc(socket_d, menu_id);
        else if (menu_id == 4)
            search_acc(socket_d, menu_id);
        else if (menu_id == 0)
        {
            write(1, "|Sayonara!                                                         |\n", 70);
            exit(0);
        }
        else
        {
            write(1, "|Enter a valid Choice                                              |\n", 70);
            continue;
        }
    }
}

void add_acc(int socket_d, int menu_id)
{
    write(socket_d, &menu_id, sizeof(menu_id));

    write(1, "|Enter Account Balance: ", 25);
    int bal;
    scanf("%d", &bal);

    write(1, "|Enter Account Type: ", 22);
    int type;
    scanf("%d", &type);

    write(socket_d, &bal, sizeof(bal));
    write(socket_d, &type, sizeof(type));

    write(1, "|Executing query.... Account Added Successfully                    |\n", 70);
}

void del_acc(int socket_d, int menu_id)
{
    write(socket_d, &menu_id, sizeof(menu_id));

    write(1, "|Enter Account_ID to delete: ", 30);
    int bal;
    scanf("%d", &bal);

    write(socket_d, &bal, sizeof(bal));
    write(1, "|Executing query.... Account Deleted Successfully                  |\n", 70);
}

void mod_acc(int socket_d, int menu_id)
{
    write(socket_d, &menu_id, sizeof(menu_id));

    write(1, "|Enter Account_ID which needs to be modified: ", 47);
    int bal;
    scanf("%d", &bal);

    write(socket_d, &bal, sizeof(bal));

    write(1, "|Executing query.... Account Modified Successfully                 |\n", 70);
}

void search_acc(int socket_d, int menu_id)
{
    write(socket_d, &menu_id, sizeof(menu_id));

    struct account_data temp;

    write(1, "|Enter Account_ID: ", 20);
    int bal;
    scanf("%d", &bal);

    write(socket_d, &bal, sizeof(bal));

    read(socket_d, &temp, sizeof(temp));
    write(1, "|Displaying the Account Info                                       |\n", 70);
    printf("|Account_ID --> %d     Balance --> %d     Account_Type --> %d\n", temp.account_id, temp.balance, temp.type);
}

void deposit(int socket_d, int menu_id)
{
    write(1, "|Enter the Amount to be Deposited: ", 36);

    int amt;
    scanf("%d", &amt);
    write(socket_d, &menu_id, sizeof(menu_id));
    write(socket_d, &amt, sizeof(amt));

    write(1, "|Executing query.... Amount Deposited Successfully                 |\n", 70);
}

void withdraw(int socket_d, int menu_id)
{
    write(socket_d, &menu_id, sizeof(menu_id));

    write(1, "|Enter the Amount to be Withdrawn: ", 36);

    int amt;
    scanf("%d", &amt);
    write(socket_d, &amt, sizeof(amt));

    int valid;
    read(socket_d, &valid, sizeof(valid));

    if (valid)
        write(1, "|Balance Amount Updated!                                           |\n", 70);
    else
        write(1, "|Error! Amount withdrawn is more than current balance              |\n", 70);
}

void details(int socket_d, int menu_id)
{
    write(socket_d, &menu_id, sizeof(menu_id));

    struct account_data temp;
    read(socket_d, &temp, sizeof(temp));

    write(1, "|Displaying the Account Info                                       |\n", 70);
    printf("|Account_ID --> %d     Balance --> %d     Account_Type --> %d\n", temp.account_id, temp.balance, temp.type);
    fflush(stdout);
}

void balance(int socket_d, int menu_id)
{
    write(socket_d, &menu_id, sizeof(menu_id));

    struct account_data temp;
    read(socket_d, &temp, sizeof(temp));

    write(1, "|Displaying the Account Info                                  |\n", 70);
    printf("|Account_ID --> %d     Balance --> %d\n", temp.account_id, temp.balance);
    fflush(stdout);
}

void pchange(int socket_d, int menu_id)
{
    write(socket_d, &menu_id, sizeof(menu_id));

    write(1, "|Enter the New Password: ", 26);

    char password[30];
    scanf("%s", password);

    write(socket_d, &password, sizeof(password));

    write(1, "|Executing query.... Password Changed Successfully                 |\n", 70);
}