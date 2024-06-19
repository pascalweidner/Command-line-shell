#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <limits.h>
#include <direct.h>
#include <windows.h>

char path[_MAX_PATH];

void echoCmd(char *str)
{
    if (strtok(NULL, " \n") == NULL)
    {
        printf("Missing echo text.");
    }
    printf("%s", str + 5);
}

void cdShell(char *str)
{
    strcpy(str, str + 3);
    char *token = strtok(str, "/\n");

    while (token != NULL)
    {
        if (strcmp(token, "..") == 0)
        {
            char *e = strrchr(path, '\\');
            int index = (int)(e - path);

            char *dest = malloc((index + 1) * sizeof(char));
            strncpy(dest, path, index);
            dest[index] = '\0';
            strcpy(path, dest);
        }
        else
        {
            char temp[200];
            strcpy(temp, path);
            strcat(temp, "\\");
            strcat(temp, token);

            const char *folder = temp;
            struct stat sb;
            if (stat(folder, &sb) == 0 && S_ISDIR(sb.st_mode))
            {
                strcat(path, "\\");
                strcat(path, token);
            }
            else
            {
                printf("Directory does not exist.");
                break;
            }
        }
        token = strtok(NULL, "/\n");
    }
}

char *formatName(char *token)
{
    char *filename = malloc((strlen(path) + strlen(token) + 2) * sizeof(char));
    strcpy(filename, path);
    strcat(filename, "\\");
    strcat(filename, token);

    return filename;
}

void renameFile(char *str)
{
    strcpy(str, str + 3);
    char *token = strtok(str, " \n");
    char *old = formatName(token);

    token = strtok(NULL, " \n");
    char *new = formatName(token);

    int ret = rename(old, new);

    if (ret != 0)
    {
        printf("mv: unable to rename file.");
    }
}

void makeDir(char *str)
{
    strcpy(str, str + 6);
    char *token = strtok(str, " \n");
    char *filename = formatName(token);

    int ret = mkdir(filename);

    if (ret != 0)
    {
        printf("mkdir: unable to create dir.");
    }
}

void removeDir(char *str)
{
    strcpy(str, str + 6);
    char *token = strtok(str, " \n");
    char *filename = formatName(token);

    int ret = rmdir(filename);

    if (ret != 0)
    {
        printf("rmdir: unable to remove dir.");
    }
}

void interpret(char *str)
{

    char com[100];
    strcpy(com, str);

    char *token = strtok(com, " \n");
    if (strcmp(token, "exit") == 0)
    {
        exit(0);
    }
    if (strcmp(token, "echo") == 0)
    {
        echoCmd(str);
    }
    else if (strcmp(token, "cd") == 0)
    {
        cdShell(str);
    }
    else if (strcmp(token, "pwd") == 0)
    {
        printf("%s", path);
    }
    else if (strcmp(token, "mv") == 0)
    {
        renameFile(str);
    }
    else if (strcmp(token, "mkdir") == 0)
    {
        makeDir(str);
    }
    else if (strcmp(token, "rmdir") == 0)
    {
        removeDir(str);
    }
    else
    {
        printf("The command \"%s\" is either misspelled or could not be found.");
    }
}

int main()
{
    strcpy(path, getcwd(NULL, 0));

    while (1)
    {
        char com[100];
        printf("KS %s> ", path);
        fgets(com, sizeof(com), stdin);

        interpret(com);

        printf("\n");
    }
    return 0;
}