/* In the server processes */

    /* 获取共享内存对象的键值 */
    if ((shm_key = get_shm_key ()) == -1) {
        goto error;
    }

    /* 建立共享内存对象 */
    shmid = shmget (shm_key, mgSizeRes, SHM_PARAM | IPC_CREAT | IPC_EXCL);
    if (shmid == -1) {
        goto error;
    }

    /* 附加到共享内存对象 */
    memptr = shmat (shmid, 0, 0);
    if (memptr == (char*)-1)
        goto error;
    else {
        /* 将内容写入共享内存对象 */
        memcpy (memptr, mgSharedRes, mgSizeRes);
        free (mgSharedRes);
    }

    /* 删除该共享内存对象。系统在附加进程数减到 1 时，才会真正删除该对象 */
    if (shmctl (shmid, IPC_RMID, NULL) < 0)
        goto error;

    /* 将 shmid 写入预先约定好的文件 */
    if ((lockfd = open (LOCKFILE, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
        goto error;

    if (write (lockfd, &shmid, sizeof (shmid)) < sizeof (shmid))
        goto error;

/* In client processes */
void* AttachSharedResource (void)
{
    int shmid;
    int lockfd;
    void* memptr;

    /* 打开约定好的文件，读取 shmid */
    if ((lockfd = open (LOCKFILE, O_RDONLY)) == -1)
        goto error;

    if (read (lockfd, &shmid, sizeof (shmid)) < sizeof (shmid))
        goto error;
    close (lockfd);

    /* 附加到共享内存对象 */
    memptr = shmat (shmid, 0, SHM_RDONLY);
    if (memptr == (char*)-1)
        goto error;
    return memptr;

error:
    perror ("AttachSharedResource");
    return NULL;
}

void UnattachSharedResource (void)
{
    /* 取消到共享内存对象的附加 */
    if (shmdt (mgSharedRes) < 0)
        perror("detaches shared resource");
}
