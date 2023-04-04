#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/fcntl.h>
#include <string.h>
#define MAX_LINE 80
#define READ 0
#define WRITE 1
char *first_; // <,>,& 앞쪽 문자열
char *second; // <,>,& 뒷 문자
int fd;
int main()
{
    char input[MAX_LINE];
    //자식 프로세스의 식별
    pid_t pid;
    pid_t pid2;
    int should_run = 1; //프로그램 시작과 종료
    while (should_run)
    {
        printf("osh> ");
        fflush(stdout);
        fgets(input, MAX_LINE, stdin); //유저의 인풋 읽기
        int background = 0;
        input[strlen(input) - 1] = '\0'; //사용자 명령어 저장 배열
        char *tmp = input;
        pid = fork();
        char *arg1[MAX_LINE / 2 + 1];
        int index = 0;
        // 명령어에 &가 존재한다면
        if(strchr(tmp,'&')!=NULL)
            background=1;
        // 명령어에 > 존재한다면
        if (strchr(tmp, '>') != NULL)
        {
            first_ = strtok(tmp, ">");  // < 왼쪽 명령어
            second = strtok(NULL, ">"); // < 오른쪽 명령어
            arg1[index] = strtok(first_, " ");
            while (arg1[index])
            {
                index++;
                arg1[index] = strtok(NULL, " ");
            }
            second = strtok(second, " "); //띄어쓰기 기준으로 토크나이즈

            if (pid < 0)
            {
                printf("fork Error");
                exit(1);
            }
            if (pid == 0) // 파일 입출력
            {
                fd = open(second, O_WRONLY | O_CREAT | O_APPEND, 0755);
                dup2(fd, STDOUT_FILENO);
                close(fd);
                printf("[1] %d \n", getpid());
                execvp(arg1[0], arg1);
                exit(0);
            }
            if (pid > 0)
            {
                if (background != 1)
                {
                    int retval = waitpid(pid, NULL, 0);

                    if (retval > 0)
                    {
                        ;
                    }
                    else
                    {
                        printf("Unexpected error\n");
                        exit(0);
                    }
                }
                else
                {
                    waitpid(-1, NULL, WNOHANG);
                    printf("background process\n");
                }
            }
            wait(NULL);
        }
        //명령어에 < 존재한다면
        if (strchr(tmp, '<') != NULL)
        {
            first_ = strtok(tmp, "<"); //왼쪽 명령어
            second = strtok(NULL, "<"); // 오른쪽 명령어
            arg1[index] = strtok(first_, " ");
            while (arg1[index])
            {
                index++;
                arg1[index] = strtok(NULL, " ");
            }
            second = strtok(second, " "); //띄어쓰기 기준으로 토크나이즈
            if (pid < 0)
            {
                printf("fork Error");
                exit(1);
            }
            if (pid == 0)  //파일 입출력
            {
                fd = open(second, O_RDONLY | O_CREAT, 0755);
                dup2(fd, STDIN_FILENO);
                close(fd);
                printf("[1] %d \n", getpid());
                execvp(arg1[0], arg1);
                exit(0);
            }
            // 백그라운드 처리
            if (pid > 0)
            {
                if (background != 1)
                {
                    int retval = waitpid(pid, NULL, 0);

                    if (retval > 0)
                    {
                        ;
                    }
                    else
                    {
                        printf("Unexpected error\n");
                        exit(0);
                    }
                }
                else
                {
                    waitpid(-1, NULL, WNOHANG);
                    printf("background process\n");
                }
            }
            wait(NULL);
        }
        //명령어에 |가 존재한다면
        else if (strchr(tmp, '|') != NULL)
        {
            first_ = strtok(tmp, "|");
            second = strtok(NULL, "|");
            char *arg2[MAX_LINE / 2 + 1];
            int index2 = 0;
            arg1[index] = strtok(first_, " ");
            while (arg1[index])
            {
                index++;
                arg1[index] = strtok(NULL, " ");
            }
            arg2[index2] = strtok(second, " ");
            while (arg2[index2])
            {
                index2++;
                arg2[index2] = strtok(NULL, " ");
            }
            int fd1[2];
            //새로운 자식 생성
            pid2 = fork();
            if (pid2 < 0)
            {
                exit(1);
            }
            if (pid2 == 0)
            {
                dup2(fd1[0], STDIN_FILENO);
                close(fd1[1]);
                execvp(arg2[0], arg2);
            }
            dup2(fd1[1], STDOUT_FILENO);
            close(fd1[0]);
        }
        // <, > & , | 가 없을 때
        else
        {
            arg1[index] = strtok(input, " ");
            while (arg1[index] != NULL)
            {
                index++;
                arg1[index] = strtok(NULL, " ");
            }
            if (pid < 0)
            {
                printf("fork Error");
                exit(1);
            }
            if (pid == 0)
            {
                printf("[1] %d \n", getpid());
                execvp(arg1[0], arg1);
                exit(0);
            }
            if (pid > 0)
            {
                if (background != 1)
                {
                    int retval = waitpid(pid, NULL, 0);

                    if (retval > 0)
                    {
                        ;
                    }
                    else
                    {
                        printf("Unexpected error\n");
                        exit(0);
                    }
                }
                else
                {
                    waitpid(-1, NULL, WNOHANG);
                    printf("background process\n");
                }
            }
            wait(NULL);
        }
    }
}