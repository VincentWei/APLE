#include <sys/types.h>
#include <sys/ipc.h>

key_t get_my_shm_key(void)
{
    return ftok("/etc/MiniGUI.cfg", 'a');
}

key_t get_my_sem_key (void)
{
    return ftok("/etc/MiniGUI.cfg", 'b');
}

