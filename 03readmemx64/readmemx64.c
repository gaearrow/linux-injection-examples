/********************************
**filename:readmemx64.c
**OS:CentOS 5.3 x86_64
**GCC:version 4.1.2 20080704 (Red Hat 4.1.2-44)
**gcc -o readmemx64 readmemx64.c
**ps -aux | grep counter
**./readmemx64 <pid> <start address in hex formats> <length>
********************************/

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <linux/user.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define uchar unsigned char
#define ulong unsigned long
#define MAX_SIZE_READ 1048576	 	// 1 MB = 1024*1024 bytes

const int long_size = sizeof(long);	// i386:4 x86_64:8
int readmem(pid_t child, ulong saddr, char *str, int len);

int main(int argc, char *argv[])
{ 
    pid_t pid;
    ulong saddr;
    int len;
 
    // Process Input
    if(argc != 4) {
        printf("useage: ./readmemx64 <pid> <start address(eg:2ad316875000)> "
					"<length(max:1048576 bytes)> \n");
        return -1;
    }
    pid = atoi(argv[1]);
    sscanf(argv[2],"%lx",&saddr);
    len = atoi(argv[3]);

	// Reading Length Check
	if (len > MAX_SIZE_READ){
		printf("ERROR:Length is overflow! \n");
		return -1;
	}
	uchar *mem = (uchar*)malloc(len+1);	
	if (mem == NULL) {
		printf("ERROR:Alloc memory unsuccessfully! \n");
		return -1;
	}

    // Read&Output memory;	
	int rst = readmem(pid, saddr, mem, len);
	if ( rst < 0){
		printf("ERROR:Read memeory unsuccessfully! \n");
		return -1;
	}
    
    int i;
    for (i = 0; i < len; i++){
        if (i % 16 == 0) printf("\n");
        printf("%02X ",mem[i]);
    }
    printf("\n");
	
	free(mem);
    return 0;
}


int readmem(pid_t pid, ulong saddr, char *mem, int len)
{
    int i = 0;
	int j = len / long_size;
    char *pmem = mem;   
    union u{
        long val;
        char chars[long_size];
    }data;

    // Attach to the process
    ptrace(PTRACE_ATTACH, pid, NULL, NULL);
    if (pid != wait(NULL)){
        printf("Attach unsuccessfully!\n");
		return -1;
	}else{
		printf("Attach to the process specified in pid %u Successfully!\n",pid);
		printf("%d bytes data at 0x%lx is follow.",len,saddr);
	}

    while(i < j){
        data.val = ptrace(PTRACE_PEEKDATA, pid, saddr + i*long_size, NULL);
        memcpy(pmem, data.chars, long_size);
        ++i;
        pmem += long_size;
    }
    j = len % long_size;
    if(j != 0){
        data.val = ptrace(PTRACE_PEEKDATA, pid, saddr + i*long_size, NULL);
        memcpy(pmem, data.chars, j);
		pmem += j;
    }
    pmem[0] = ' ';

    // Recover the Process
    ptrace(PTRACE_DETACH, pid, NULL, NULL);

	return pmem-mem;
}
