#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

static void delete_sem(void);

/* Create a System V semaphore set. */
    key_t sem_key;
    int semid;
    union semun sunion;

    // Obtain the semophore syncing drawing.
    if ((sem_key = get_my_sem_key ()) == -1) {
        goto error;
    }
    semid = semget (sem_key, _NR_SEM, SEM_PARAM | IPC_CREAT | IPC_EXCL);
    if (semid == -1) {
        goto error;
    }
    atexit (delete_sem);

/* Initialize the semaphores. */
    sunion.val = 1;
    semctl (semid, 0, SETVAL, sunion);
    sunion.val = 1;
    semctl (semid, 1, SETVAL, sunion);
    sunion.val = 0;
    semctl (semid, 2, SETVAL, sunion);

static void delete_sem(void);
{
    union semun ignored;
    if (semctl (SHAREDRES_SEMID, 0, IPC_RMID, ignored) < 0)
        goto error;

    return;

error:
    perror("remove semaphore");
}

void unlock_draw_sem (void)
{
    struct sembuf sb;

again:
    sb.sem_num = 0;
    sb.sem_op = 1;
    sb.sem_flg = SEM_UNDO;

    if (semop (SHAREDRES_SEMID, &sb, 1) == -1) {
        if (errno == EINTR) {
            goto again;
        }
    }
}

void lock_draw_sem (void)
{
    struct sembuf sb;

again:
    sb.sem_num = 0;
    sb.sem_op = -1;
    sb.sem_flg = SEM_UNDO;

    if (semop (SHAREDRES_SEMID, &sb, 1) == -1) {
        if (errno == EINTR) {
            goto again;
        }
    }
}

