#include <pthread.h>
#include <semaphore.h>
// functions
typedef float (*FN_PT)(void *p);
void pool_init(int);
void pool_submit(FN_PT fn_pt, void *param);
void pool_start_jobs();
void pool_destroy(char *path);
float execute(FN_PT fn_pt, void *param);


// data structures
void *worker(void *param);
struct Task {
    int tid;
    FN_PT fn_pt;
    struct Task *next;
    void *param; 
};

struct ThreadPool {
    struct Task *head;
    struct Task *tail;
    pthread_mutex_t mutex;
    sem_t semaphore;
    int total_threads;
    pthread_t *threads;
    float count;
};