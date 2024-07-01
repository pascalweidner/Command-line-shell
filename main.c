#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <limits.h>
#include <direct.h>
#include <windows.h>

char path[_MAX_PATH];

HANDLE handle;

void echoCmd(char *str)
{
    strtok(str, " \n");
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
        printf("mv: unable to rename file.");
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
        printf("mkdir: unable to create dir.");
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
        printf("rmdir: unable to remove dir.");
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
        printf("cat: not able to open file %s.", token);
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
    DWORD threadID;
    handle = CreateThread(NULL, 0, (void *)func, str, 0, &threadID);
    if (handle == NULL)
    {
        printf("Create Thread Failed. Error no: %d\n", GetLastError);
    }
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
        printf("The command \"%s\" is either misspelled or could not be found.");
    }

    runCommand(str, func);
}

int main()
{
    strcpy(path, getcwd(NULL, 0));

    while (1)
    {
        char com[100];
        printf("KS %s>", path);
        fgets(com, sizeof(com), stdin);

        interpret(com);

        printf("\n");
    }

    printf("TestEnde");

    WaitForSingleObject(handle, INFINITE);
    return 0;
}