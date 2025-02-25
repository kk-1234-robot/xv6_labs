#include "kernel/types.h"
#include "user/user.h"

#define RD 0 // pipe的read端
#define WR 1 // pipe的write端

const uint INT_LEN = sizeof(int);

/**
 * @brief 读取左邻居的第一个数据
 * @param lpipe 左邻居的管道符
 * @param dst 存储第一个数据的地址
 * @return int 读取成功返回0，否则返回-1
 */
int lpipe_first_data(int lpipe[2], int *dst)
{
    if (read(lpipe[RD], dst, INT_LEN) == INT_LEN)
    {
        printf("prime %d\n", *dst);
        return 0;
    }
    return -1;
}

/**
 * @brief 读取左邻居的数据，将不能被第一个数据整除的数据写入右邻居的管道
 * @param lpipe 左邻居的管道符
 * @param rpipe 右邻居的管道符
 * @param first 第一个数据
 */
void transmit_data(int lpipe[2], int rpipe[2], int first)
{
    int data;
    // 读取左邻居的数据
    while (read(lpipe[RD], &data, INT_LEN) == INT_LEN)
    {
        // 如果不能被第一个数据整除，则写入右邻居的管道
        if (data % first != 0)
        {
            write(rpipe[WR], &data, INT_LEN);
        }
    }
    close(lpipe[RD]); // 关闭左管道读端
    close(rpipe[WR]); // 关闭右管道写端
}

/**
 * @brief 素数筛
 * @param lpipe 左邻居的管道符
 */
void prime_sieve(int lpipe[2])
{
    close(lpipe[WR]); // 关闭写端
    int first;
    if (lpipe_first_data(lpipe, &first) == 0)
    {
        int rpipe[2];
        pipe(rpipe);
        transmit_data(lpipe, rpipe, first);

        if (fork() == 0)
        {
            prime_sieve(rpipe); // 递归调用
        }
        else
        {
            close(rpipe[RD]); // 关闭读端
            wait(0);          // 等待子进程结束
        }
    }
    exit(0);
}

int main(int argc, char const *argv[])
{
    int pipefd[2];
    pipe(pipefd);

    for (int i = 2; i <= 35; i++)
    {
        write(pipefd[WR], &i, INT_LEN);
    }

    if (fork() == 0)
    {
        prime_sieve(pipefd);
    }
    else
    {
        close(pipefd[WR]);
        close(pipefd[RD]);
        wait(0);
    }

    exit(0);
}