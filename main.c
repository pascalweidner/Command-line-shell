#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <limits.h>
#include <direct.h>
#include "list_h.h"

char path[_MAX_PATH];

h_list *handle_list;
int threadCount;
DWORD threadID;

void echoCmd(char *str)
{
    char *cache = (char *)malloc((strlen(str) + 1) * sizeof(char));
    strcpy(cache, str);
    strtok(cache, " \n");
    if (strtok(NULL, " \n") == NULL)
    {
        printf("Missing echo text. \n");
    }
    printf("%s \n", str + 5);
    free(cache);
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
                printf("Directory does not exist. \n");
                break;
            }
        }
        token = strtok(NULL, "/\n");
    }
}

char *formatName(char *token)
{
    char *filename = malloc((strlen(path) + strlen(token) + 1) * sizeof(char));
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
        printf("mv: unable to rename file. \n");
    }

    free(old);
    free(new);
}

void makeDir(char *str)
{
    strcpy(str, str + 6);
    char *token = strtok(str, " \n");
    char *filename = formatName(token);

    int ret = mkdir(filename);

    if (ret != 0)
    {
        printf("mkdir: unable to create dir. \n");
    }

    free(filename);
}

void removeDir(char *str)
{
    strcpy(str, str + 6);
    char *token = strtok(str, " \n");
    char *filename = formatName(token);

    int ret = rmdir(filename);

    if (ret != 0)
    {
        printf("rmdir: unable to remove dir. \n");
    }

    free(filename);
}

void printFile(char *str)
{
    strcpy(str, str + 4);
    char *token = strtok(str, "\n ");
    char *filename = formatName(token);

    FILE *fptr;
    fptr = fopen(filename, "r");

    if (fptr == NULL)
    {
        printf("cat: not able to open file %s. \n", token);
        return;
    }

    char buffer[100];
    while (fgets(buffer, 100, fptr))
    {
        printf("%s", buffer);
    }

    fclose(fptr);
    free(filename);
}

void runNewThread(char *str, void (*func)(char *))
{
    HANDLE *handle = (HANDLE *)malloc(sizeof(HANDLE));
    *handle = CreateThread(NULL, 0, (void *)func, str, 0, &threadID);
    if (*handle == NULL)
    {
        printf("Create Thread Failed. Error no: %d\n", GetLastError);
        return;
    }
    threadCount++;
    add_h(handle_list, handle, threadCount);
    printf("[%d] \n", threadCount);
}

void runCommand(char *str, void (*func)(char *))
{
    if (func != NULL)
    {
        if (strrchr(str, '&') - str == strlen(str) - 2)
        {
            str[strlen(str) - 2] = '\0';
            runNewThread(str, func);
        }
        else
        {
            (*func)(str);
        }
    }
}

void interpret(char *str)
{

    char com[100];
    strcpy(com, str);

    void (*func)(char *) = NULL;

    char *token = strtok(com, " \n");
    if (strcmp(token, "exit") == 0)
    {
        exit(0);
    }
    if (strcmp(token, "echo") == 0)
    {
        func = &echoCmd;
    }
    else if (strcmp(token, "cd") == 0)
    {
        func = &cdShell;
    }
    else if (strcmp(token, "pwd") == 0)
    {
        printf("%s", path);
    }
    else if (strcmp(token, "mv") == 0)
    {
        func = &renameFile;
    }
    else if (strcmp(token, "mkdir") == 0)
    {
        func = &makeDir;
    }
    else if (strcmp(token, "rmdir") == 0)
    {
        func = &removeDir;
    }
    else if (strcmp(token, "cat") == 0)
    {
        func = &printFile;
    }
    else
    {
        printf("The command \"%s\" is either misspelled or could not be found. \n");
    }

    runCommand(str, func);
}

int main()
{
    strcpy(path, getcwd(NULL, 0));

    handle_list = createHList();
    threadCount = -1;

    while (1)
    {
        char com[100];
        printf("KS %s>", path);
        fgets(com, sizeof(com), stdin);

        interpret(com);

        int cache = 0;
        for (int i = 0; i < threadCount; i++)
        {
            DWORD result = WaitForSingleObject(*get_h(handle_list, i), 0);
            printf("%d \n", result);
            if (result == WAIT_OBJECT_0)
            {
                CloseHandle(*get_h(handle_list, i));
                int ind = delete_h(handle_list, i);
                printf("[%d] Done \n", ind);
                cache++;
            }
        }
        threadCount -= cache;
    }

    for (int i = 0; i < threadCount; i++)
    {
        DWORD result = WaitForSingleObject(*get_h(handle_list, i), INFINITE);
        CloseHandle(*get_h(handle_list, i));
    }
    return 0;
}