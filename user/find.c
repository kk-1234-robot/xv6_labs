#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find(char *path, const char *filename)
{
    char buf[512], *p;
    int fd;
    struct dirent de;
    struct stat st;

    if ((fd = open(path, 0)) < 0)
    {
        fprintf(2, "find: cannot open %s\n", path);
        return;
    }

    if (fstat(fd, &st) < 0)
    {
        fprintf(2, "find: cannot stat %s\n", path);
        close(fd);
        return;
    }

    // 参数错误，find的第一个参数必须是目录
    if (st.type != T_DIR)
    {
        fprintf(2, "find: %s is not a directory\n", path);
        close(fd);
        return;
    }

    // 路径太长
    if (strlen(path) + 1 + DIRSIZ + 1 > sizeof(buf))
    {
        fprintf(2, "find: path too long\n");
        close(fd);
        return;
    }

    strcpy(buf, path);     // 将path复制到buf
    p = buf + strlen(buf); // p指向buf的末尾
    *p++ = '/';            // 在buf末尾添加'/'
    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
        if (de.inum == 0)
            continue;
        memmove(p, de.name, DIRSIZ); // 将de.name复制到p
        p[DIRSIZ] = 0;               // 添加字符串结束符
        if (stat(buf, &st) < 0)
        {
            fprintf(2, "find: cannot stat %s\n", buf);
            continue;
        }
        // 不在“.”和“..”目录中递归
        if (st.type == T_DIR && (strcmp(p, ".") != 0 && strcmp(p, "..") != 0))
        {
            find(buf, filename);
        }
        else if (strcmp(p, filename) == 0)
        {
            printf("%s\n", buf);
        }
    }
    close(fd);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(2, "usage: find <path> <filename>\n");
        exit(1);
    }
    else
    {
        find(argv[1], argv[2]);
        exit(0);
    }
}