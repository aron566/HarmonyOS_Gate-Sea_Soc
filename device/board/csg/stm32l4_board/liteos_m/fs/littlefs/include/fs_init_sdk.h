#include "stdio.h"
#include "stdint.h"
#include "w25qxx.h"

#define READ_SIZE 1024
#define PROG_SIZE 1024
#define CACHE_SIZE 2048
#define LOOKAHEAD_SIZE 2048
#define BLOCK_CYCLES 500
extern int32_t FsInit(void);

