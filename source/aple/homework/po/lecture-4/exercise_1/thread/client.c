#include<stdio.h>
#include <stdlib.h>
#include<dirent.h>
#include<string.h>
#include <sys/stat.h>  

#include "threadpool.h"

struct Param {
    char *path;
};

float get_dir_size(const char*pathname)
{
  struct stat s_buf;
  stat(pathname,&s_buf);
  float size = 0;
  if(S_ISDIR(s_buf.st_mode)) {
      struct dirent *entry;
      int parent_dir_len = strlen(pathname);
      DIR *dir = opendir(pathname);
    	while((entry = readdir(dir))) {
        if (!(strcmp(entry->d_name,".") & strcmp(entry->d_name,".."))) {
          continue;
        }
        char *fullpath = malloc(parent_dir_len + strlen(entry->d_name) + 2);
        sprintf(fullpath, "%s/%s", pathname, entry->d_name);
        size += get_dir_size(fullpath);
      }
  } else {
    size += (float)s_buf.st_size;
  }
  return size;
}

float worker_fn(void *param) {
    struct Param *p = (struct Param *)param;
    float size = get_dir_size(p->path);
    printf("path: %s, size: %f\n", p->path, size);
    return size;
}

void submit_cal_dir_size_job(char *dirpath) {
    struct dirent *entry;
    DIR *dir = opendir(dirpath);
    if (dir == NULL)
    {
        perror("Directory cannot be opened!");
        exit(0);
    }
    int parent_dir_len = strlen(dirpath);
    int count = 0;
    while ((entry = readdir(dir)) != NULL)
    {
        if (!(strcmp(entry->d_name,".") & strcmp(entry->d_name,".."))) {
          continue;
        }
        char *fullpath = malloc(parent_dir_len + strlen(entry->d_name) + 2);
        sprintf(fullpath, "%s/%s", dirpath, entry->d_name);
        struct Param *p = (struct Param*) malloc(sizeof(struct Param));
        p->path = fullpath;
        // submit job
        pool_submit(&worker_fn, p);
        if (count >= 2) {
          usleep(10);
        }
        count++;
    }
    closedir(dir);
}

int main() {
    int thread_num = 8;
    pool_init(thread_num);
    char dirpath[] = "/root/now/linux-study/";
    submit_cal_dir_size_job(&dirpath);
    pool_destroy(&dirpath);
}