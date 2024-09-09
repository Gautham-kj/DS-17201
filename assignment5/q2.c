#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <dirent.h>
#include <string.h>

#define MAX_FILES 1024

typedef struct
{
    char owner[256];
    off_t size;
} FileInfo;

void get_file_info(const char *path, FileInfo *file_info)
{
    struct stat file_stat;
    if (stat(path, &file_stat) == 0)
    {
        struct passwd *pw = getpwuid(file_stat.st_uid);
        if (pw != NULL)
        {
            strcpy(file_info->owner, pw->pw_name);
            file_info->size = file_stat.st_size;
        }
    }
}

void map_phase(const char *directory, FileInfo file_info_list[], int *file_count)
{
    DIR *dir = opendir(directory);
    if (dir == NULL)
    {
        perror("opendir");
        exit(EXIT_FAILURE);
    }

    struct dirent *entry;
    *file_count = 0;
    while ((entry = readdir(dir)) != NULL)
    {
        // Check if the entry is a regular file
        if (entry->d_type == DT_REG)
        {
            char file_path[1024];
            snprintf(file_path, sizeof(file_path), "%s/%s", directory, entry->d_name);
            get_file_info(file_path, &file_info_list[*file_count]);
            (*file_count)++;
        }
    }
    closedir(dir);
}

void reduce_phase(FileInfo file_info_list[], int file_count)
{
    off_t max_size = 0;
    for (int i = 0; i < file_count; i++)
    {
        if (file_info_list[i].size > max_size)
        {
            max_size = file_info_list[i].size;
        }
    }

    printf("Users owning files with maximum size (%ld bytes):\n", max_size);
    for (int i = 0; i < file_count; i++)
    {
        if (file_info_list[i].size == max_size)
        {
            printf("%s\n", file_info_list[i].owner);
        }
    }
}

int main()
{
    FileInfo file_info_list[MAX_FILES];
    int file_count;

    map_phase(".", file_info_list, &file_count);
    reduce_phase(file_info_list, file_count);

    return 0;
}