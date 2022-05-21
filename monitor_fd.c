#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

int check_args(int args, int required);
void *safe_malloc(size_t size);
char *trim(char *str);
int get_pid_by_name(char name[], int start, int end);
int count_descriptors (char name[]);
int is_number (char *nb);
int get_pid_by_name_advanced (char name[]);

#define BUF_SIZE 1024
char buffer[BUF_SIZE];

int is_number (char *nb) {
    for (int i = 0; nb[i] != '\0'; ++i) {
        if (!isdigit(nb[i])) {
            return 1;
        }
    }
    return 0;
}

int check_args (int args, int required) {
    if (args != required) {
        printf("use: ./monitor_fd process_name\n");
        return 1;
    }
    else {
        return 0;
    }
}

void *safe_malloc (size_t size) {
    void *ptr = malloc(size);
    if(ptr != NULL) {
        return ptr;
    }
    else {
        fprintf(stderr, "malloc of size %lu failed\n", size);
        exit(EXIT_FAILURE);
    }
}

char *trim (char *str) {
    char *end;

    // Trim leading space
    while(isspace((unsigned char)*str)) str++;

    if(*str == 0)  // All spaces?
        return str;

    // Trim trailing space
    end = str + strlen(str) - 1;
    while(end > str && isspace((unsigned char)*end)) end--;

    // Write new null terminator character
    end[1] = '\0';

    return str;
}

int get_pid_by_name (char name[], int start, int end) {
    // naive function that relies on /proc to search
    // for process dirs and status files to find the
    // pid based on the name

    for (int i = start; i < end; ++i) {

        // read the content of the file in memory
        // 13 for /proc and /status, 6 max for proc nb +1
        char *status_path = safe_malloc(13 + 6 + 1);
        sprintf(status_path, "/proc/%d/status", i);

        FILE *fp;
        fp  = fopen(status_path, "r");
        long length;

        if (fp != NULL) {
            // read the file's content
            while (fgets(buffer, BUF_SIZE, fp) != NULL) {
                /* Total character read count */
                length = strlen(buffer);
                 // Trim new line character from last if exists.
                buffer[length-1] = buffer[length-1] == '\n' ? '\0' : buffer[length-1];

                // find the name
                char *res = strstr(buffer, "Name");
                if (res) {
                    char proc_name[255];
                    strcpy(proc_name, trim(&buffer[5]));
                    // printf("name %s - proc_name %s - pid %d\n", name, proc_name, i);

                    if (strcmp(name, proc_name) == 0) {
                        printf("name : %s - pid: %d\n", proc_name, i);
                        return i;
                    }
                }
            }
            //break;
        }
        else {
            continue;
        }
        // free mallocs
        free(status_path);
    }
    return -1;
}

int get_pid_by_name_advanced (char name[]) {
    DIR *proc;
    proc = opendir("/proc");
    char *status_path = safe_malloc(50);
    struct dirent *de;

    while ((de = readdir(proc)) != NULL) {

        // skip files

        if (de->d_type != DT_DIR) {
            continue;
        }

        // skip . and ..
        if (is_number(de->d_name) == 1) {
            continue;
        }

        sprintf(status_path, "/proc/%s/status", de->d_name);
        FILE *fp;
        fp  = fopen(status_path, "r");
        long length;

        if (fp != NULL) {
            // read the file's content
            while (fgets(buffer, BUF_SIZE, fp) != NULL) {
                /* Total character read count */
                length = strlen(buffer);
                // Trim new line character from last if exists.
                buffer[length-1] = buffer[length-1] == '\n' ? '\0' : buffer[length-1];

                // find the name
                char *res = strstr(buffer, "Name");
                if (res) {
                    char proc_name[255];
                    strcpy(proc_name, trim(&buffer[5]));
                    // printf("name %s - proc_name %s - pid %d\n", name, proc_name, i);

                    if (strcmp(name, proc_name) == 0) {
                        printf("name : %s - pid: %s\n", proc_name, de->d_name);
                        return atoi(de->d_name);
                    }
                }
            }
        }
        else {
            continue;
        }

    }

    free(status_path);

    return -1;

}

int count_descriptors (char name[]) {
    int nb_fd = 0;
    // TODO: find way to put defaults
    // int pid = get_pid_by_name(name, 300000, 500000);
    int pid = get_pid_by_name_advanced(name);

    if (pid == -1) {
        printf("process %s was not found on the system.\n", name);
        return -1;
    }

    char *proc_path = safe_malloc(9 + 6 + 1);
    sprintf(proc_path, "/proc/%d/fd", pid);

    // open the directory and read all the files
    DIR *proc_dir;
    proc_dir = opendir(proc_path);
    struct dirent *de;

    while ((de = readdir(proc_dir)) != NULL) {
        nb_fd++;
    }

    return nb_fd;
}


int main (int argc, char *argv[]) {
    if (check_args(argc, 2) == 1) {
        exit(1);
    }

    char *name = argv[1];
    int descriptors = count_descriptors(name);

    if (descriptors > 0) {
        printf("process %s: %d \n", name, descriptors);
        return 0;
    }
    else if (descriptors == 0) {
        printf("no descriptors open for process %s\n", name);
    }
    else {
        printf("no pid found for name %s\n", name);
        return 1;
    }
}