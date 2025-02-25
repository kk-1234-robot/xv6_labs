#include "kernel/types.h"
#include "user/user.h"

#define RD 0 // pipe的read端
#define WR 1 // pipe的write端

int main(int argc, char const *argv[])
{
    char buf = 'P'; // 用于传送的字节

    int fd_c2p[2]; // 子进程->父进程
    int fd_p2c[2]; // 父进程->子进程

    pipe(fd_c2p); // 创建子进程->父进程的管道
    pipe(fd_p2c); // 创建父进程->子进程的管道

    int pid = fork(); // 创建子进程
    int exit_status = 0;

    if (pid < 0)
    {
        fprintf(2, "fork() error\n");
        close(fd_c2p[RD]); // 关闭子进程->父进程的管道的读端
        close(fd_c2p[WR]); // 关闭子进程->父进程的管道的写端
        close(fd_p2c[RD]); // 关闭父进程->子进程的管道的读端
        close(fd_p2c[WR]); // 关闭父进程->子进程的管道的写端
        exit(1);           // 退出
    }
    else if (pid == 0) // 子进程
    {
        close(fd_p2c[WR]); // 关闭子进程->父进程的管道的写端
        close(fd_c2p[RD]); // 关闭父进程->子进程的管道的读端

        if (read(fd_p2c[RD], &buf, sizeof(char)) != sizeof(char))
        {
            fprintf(2, "child read() error\n");
            exit_status = 1; // 标记出错
        }
        else
        {
            fprintf(1, "%d: received ping\n", getpid());
        }

        if (write(fd_c2p[WR], &buf, sizeof(char)) != sizeof(char))
        {
            fprintf(2, "child write() error\n");
            exit_status = 1; // 标记出错
        }

        close(fd_p2c[RD]); // 关闭父进程->子进程的管道的读端
        close(fd_c2p[WR]); // 关闭子进程->父进程的管道的写端

        exit(exit_status); // 退出
    }
    else
    {
        close(fd_c2p[WR]); // 关闭子进程->父进程的管道的写端
        close(fd_p2c[RD]); // 关闭父进程->子进程的管道的读端

        if (write(fd_p2c[WR], &buf, sizeof(char)) != sizeof(char))
        {
            fprintf(2, "parent write() error\n");
            exit_status = 1; // 标记出错
        }

        if (read(fd_c2p[RD], &buf, sizeof(char)) != sizeof(char))
        {
            fprintf(2, "parent read() error\n");
            exit_status = 1; // 标记出错
        }
        else
        {
            fprintf(1, "%d: received pong\n", getpid());
        }

        close(fd_c2p[RD]); // 关闭子进程->父进程的管道的读端
        close(fd_p2c[WR]); // 关闭父进程->子进程的管道的写端

        exit(exit_status); // 退出
    }
}