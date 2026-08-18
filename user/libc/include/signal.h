#pragma once

typedef void (*sighandler_t)(int);

#define SIG_DFL ((sighandler_t)0)
#define SIG_IGN ((sighandler_t)1)

#define SIGINT  2
#define SIGTERM 15

sighandler_t signal(int signum, sighandler_t handler);
int kill(long pid, int sig);
