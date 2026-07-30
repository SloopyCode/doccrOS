#include <unistd.h>
#include <stdio.h>

int main(void)
{
    printf("[reboot] rebooting now\n");
    reboot(REBOOT_REBOOT);
    printf("[reboot] failed to reboot, shutting down your computer...\n");
    reboot(REBOOT_SHUTDOWN);
    printf("[poweroff] failed cuz idk\n");
    _exit(1);

    // may execute poweroff.elf if those 2 programs get bigger in future
}