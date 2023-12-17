#include <stdio.h>
#include <stdlib.h>

// 测试 aligned_alloc

int
main(void)
{
    char  *p         = NULL;
    size_t alignment = 4;

    srandom(42);
    for (alignment = 4; alignment < 512; alignment = alignment << 1) {
        printf("address aligned to %ld:\n", alignment);
        for (int i = 0; i < 10; i++) {
            // 申请分配 10 个随机大小小于 2048 的内存空间
            p = aligned_alloc(alignment, (size_t)random() / 2048);
            printf(" >> %p\n", p);
            free(p);
        }
    }
    return 0;
}
