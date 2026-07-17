#include <unistd.h>
#include <stdio.h>

int main(void)
{
    printf("[poweroff] shutdown now\n");
    reboot(REBOOT_SHUTDOWN);
    printf("[poweroff] failed cuz idk\n");
    _exit(1);
}