/*Hanlin Shen
 *14/10/2014
 *HLS shell v2.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <signal.h>
#include <time.h>
void parse(char command_line[], char* args[]);
void sig_handler(int);

int main(int argc, char **argv)
{
    signal(SIGTSTP, sig_handler);
    signal(SIGINT, sig_handler);
    char command_line[50] = "NULL";
    char command_line2[50] = "NULL";
    char* args[99];
    char* args2[99];
    char* prompt = "Han-2.0-->";
    int exitLoop = 0;
    int i = 0;
    int j = 1;
    int index = 0;
    int index2 = 0;
    int buildIn = 0;
    int runInBackGround = 0;
    int redirection = 0;
    int isPipe = 0;
    pid_t pid = 0;
    char fileName[20];


    printf(" __    __     _ \n");
    printf("/ / /\\ \\ \\___| | ___ ___  _ __ ___   ___\n");
    printf("\\ \\/  \\/ / _ \\ |/ __/ _ \\| '_ ` _ \\ / _ \\\n");
    printf(" \\  /\\  /  __/ | (_| (_) | | | | | |  __/\n");
    printf("  \\/  \\/ \\___|_|\\___\\___/|_| |_| |_|\\___|\n");
    printf(" __          __   __          __  __    __\n");
    printf("/ _\\  /\\  /\\/ /  / _\\  /\\  /\\/__\\/ /   / /\n");
    printf("\\ \\  / /_/ / /   \\ \\  / /_/ /_\\ / /   / /\n");
    printf("_\\ \\/ __  / /___ _\\ \\/ __  //__/ /___/ /__\n");
    printf("\\__/\\/ /_/\\____/ \\__/\\/ /_/\\__/\\____/\\____/\n");
    printf("version 2.0\n");





    while (exitLoop == 0)
    {

        for (i = 0; i < 50; i++)
        {
            command_line[i] = '\0';
            command_line2[i] = '\0';
        }
        for (i = 0; i < 99; i++)
        {
            args[i] = NULL;
        }

        for (i = 0; i < 20; i++)
        {
            fileName[i] = '\0';
        }
        printf("%s ", prompt);
        fflush(stdout);
        fgets(command_line, 99, stdin);
        if (command_line[0] != '\n')
        {

            //remove enter key
            for (i = 0; i <= 99; i++)
            {


                //find a > need to redirection
                if (i > 0 && command_line[i - 1] == '>')
                {
                    command_line[i - 1] = '\0';
                    printf("i find a > need to redirection\n");
                    redirection = 1;

                    for (j = i+1; j <= 99; j++)
                    {
                        if (command_line[j] == '\n')
                        {
                            break;
                        }
                        fileName[index] = command_line[j];
                        index++;
                    }
                    break;
                }

                //find a | need to use pipe
                if (i > 0 && command_line[i - 1] == '|')
                {
                    command_line[i - 1] = '\0';
                    printf("i find a | need to use pipe\n");
                    for (j = i + 1; j <= 99; j++)
                    {
                        if (command_line[j] == '\n' || command_line[j] == '>')
                        {
                            break;
                        }
                        command_line2[index2] = command_line[j];
                        index2++;
                    }
                    isPipe = 1;
                }


                //find a & need run in background
                if (command_line[i] == '\n')
                {
                    command_line[i] = '\0';

                    if (i > 0 && command_line[i - 1] == '&')
                    {
                        command_line[i - 1] = '\0';
                        printf("i find a & need run in background\n");
                        runInBackGround = 1;
                    }
                    break;
                }
            }

            //if user use pipe print out the front and back of the pipe
            if (isPipe == 1)
            {
                printf("comand befor pipe: %s\n", command_line);
                printf("comand after pipe: %s\n", command_line2);
            }


            //parse the command if pipe parse the both part of command_line and command_lin2
            if (isPipe == 1)
            {
                parse(command_line, args);
                parse(command_line2, args2);
            } else
            {
                parse(command_line, args);
            }

            //if user enter exit then exit the loop
            if (i > 1 && strcmp(args[0], "exit") == 0)
            {
                exitLoop = 1;
                buildIn = 1;
                pid = 1;
            }


            //if args = cd then its is not a build in command				
            if (i > 1 && strcmp(args[0], "cd") == 0)
            {
                chdir(args[1]);
                system("pwd");
                buildIn = 1;
                pid = 1;
            }

            //if it is not a buld in commands
            if (i > 1 && buildIn == 0)
            {
                pid = fork();
            }

            // start to fork child and execute the command
            if (pid == 0)
            {
                //redirection
                if (redirection == 1)
                {

                    printf("out put is redirection to file: %s\n", fileName);
                    int file_id = open(fileName, O_CREAT | O_WRONLY, 0666);
                    close(1);
                    dup(file_id);
                    close(file_id);
                }


                //is pipe
                if (isPipe == 1)
                {
                    int fd[2];
                    pipe(fd);
                    if (fork() == 0)
                    {
                        close(fd[0]); //child colses input end of pipe, closing the file
                        close(1);
                        dup(fd[1]);
                        close(fd[1]);
                        execvp(command_line, args);
                        exit(1);

                    } else
                    {
                        close(fd[1]); // parent closes output end of pipe
                        close(0);
                        dup(fd[0]);
                        close(fd[0]);
                        execvp(command_line2, args2);
                        exit(1);
                    }
                }
                else
                { //normal
                    execvp(command_line, args);
                    printf("error commands\n");
                    exit(1);
                }



            } else
            {
                //if there is a child Parent will wait	
                if (buildIn == 0)
                {
                    if (runInBackGround != 1)
                    {
                        wait(NULL);
                    }
                }

            }

            //start reset  
            buildIn = 0;
            runInBackGround = 0;
            j = 1;
            redirection = 0;
            isPipe = 0;
            index = 0;
            index2 = 0;
            // end of reset
        }
    }
    printf("exit sussce\n");
    printf("   ______                ____ \n");
    printf("  / ____/___  ____  ____/ / /_  __  _____\n");
    printf(" / / __/ __ \\/ __ \\/ __  / __ \\/ / / / _ \\\n");
    printf("/ /_/ / /_/ / /_/ / /_/ / /_/ / /_/ /  __/\n");
    printf("\\____/\\____/\\____/\\__,_/_.___/\\__, /\\___/\n");
    printf("                             /____/      \n");

    return 0;
}

void parse(char command_line[], char* args[])
{
    int i = 0;
    args[i] = strtok(command_line, " ");
    while (args[i] != NULL)
    {
        i++;
        args[i] = strtok(NULL, " ");
    }
}

void sig_handler(int signo)
{
    extern char *__progname;
    char timebuff[100];
    char datebuff[100];
    time_t now = time(0);
    strftime(timebuff, 100, "%H:%M:%S", localtime(&now));
    strftime(datebuff, 100, "%Y-%m-%d", localtime(&now));

    if (signo == SIGINT)
    {
        printf("\nStop [%s] at [%s] on [%s]\n", __progname, timebuff, datebuff);
        fflush(stdout);
        exit(0);
    }

    if (signo == SIGTSTP)
    {
        printf("\nSuspending [%s] at [%s] on [%s]\n", __progname, timebuff, datebuff);
        printf("Han-2.0-->");
        fflush(stdout);
    }

}
