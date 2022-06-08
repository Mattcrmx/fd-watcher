#include <stdio.h>
#include <argp.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "monitor_fd.h"

int watch(int pid, int interval, int time_limit);

const char *argp_program_version = "fd-watcher 0.1";
static char doc[] = "File descriptor watcher";
static char args_doc[] = "[FILENAME]...";
static struct argp_option options[] = {
        {"time", 't', "TIME", OPTION_ARG_OPTIONAL, "How much time should the watchdog run.", 0},
        {"name", 'n', "NAME", OPTION_ARG_OPTIONAL, "The name of the process to monitor.", 0},
        {"pid",  'p', "PID", OPTION_ARG_OPTIONAL, "The pid of the process to monitor.", 0},
        {"interval",  'i', "INTERVAL", OPTION_ARG_OPTIONAL, "The interval between watch, in seconds.", 0},
        {0} // needs to be terminated by a zero
};

struct arguments {
    enum {
        WATCH_BY_NAME, WATCH_BY_PID
    } mode;
    char *name;
    int pid;
    int time;
    int interval;
};

static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;
    switch (key) {
        case 'i':
            arguments->interval = atoi(arg);
            break;
        case 't':
            arguments->time = atoi(arg);
            break;
        case 'n':
            arguments->mode = WATCH_BY_NAME;
            arguments->name = arg;
            break;
        case 'p':
            arguments->mode = WATCH_BY_PID;
            arguments->pid = atoi(arg);
            break;
        case ARGP_KEY_ARG:
            return 0;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

int watch(int pid, int interval, int time_limit) {
    // function to watch the descriptors of a certain process
    time_t end;
    time_t start = time(NULL);
    int descriptors;
    end = start + time_limit;

    while (start < end) {
        descriptors = count_descriptors_by_pid(pid);
        fprintf(stderr,"process %d: %d\n", pid, descriptors);
        sleep(interval);
        start = time(NULL);
    }
    return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc, 0, 0, 0};

int main(int argc, char *argv[]) {

    // default values for the arguments
    struct arguments arguments;
    arguments.mode = WATCH_BY_NAME;
    arguments.name = (char *) "";  // forced casting to a pointer to avoid the warning: bad
    arguments.pid = -1;
    arguments.time = 60;
    arguments.interval = 1;

    // parse the arguments
    argp_parse(&argp, argc, argv, 0, 0, &arguments);


    // get the pid
    int pid;
    switch (arguments.mode) {
        case WATCH_BY_NAME:
            pid = get_pid_by_name(arguments.name);
            break;
        case WATCH_BY_PID:
            pid = arguments.pid;
            break;
    }

    // watch
    watch(pid, arguments.interval, arguments.time);

    return 0;
};
