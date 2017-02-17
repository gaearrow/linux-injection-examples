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

#define uchar unsigned char
#define ulong unsigned long
#define MAX_SIZE_READ 1024000	// Bytes

//i386:	  long_size=4
//x86_64: long_size=8
const int long_size = sizeof(long);
void readmem(pid_t child, ulong saddr, char *str, int len);


int main(int argc, char *argv[])
{ 
    pid_t pid;
    ulong saddr;
    int len;
    
    uchar mem[MAX_SIZE_READ+1];
    
    // Process Input
    if(argc != 4) {
        printf("useage: ./readmemx64 <pid> <start address(eg:2ad316875000)> <length(max:1024000 Bytes)> \n");
        return 1;
    }
    pid = atoi(argv[1]);
    sscanf(argv[2],"%lx",&saddr);
    len = atoi(argv[3]);
	
	// Reading Length Check
	if (len > MAX_SIZE_READ){
		printf("ERROR:Length is overflow! \n");
		return 1;
	}

    // Attach to the process
    ptrace(PTRACE_ATTACH, pid, NULL, NULL);
    if (pid != wait(NULL)){
        printf("Attach unsuccessfully!\n");
		return 1;
	}else{
		printf("Attach to the process specified in pid %u Successfully!\n",pid);
		printf("The %d Bytes data started with 0x%lx is follow.",len,saddr);
	}
    

    // Read memory
    readmem(pid, saddr, mem, len);

    // Print memory
    int i;
    for (i = 0;i < len;i++){
        if (i%16==0) printf("\n");
        printf("%02X ",mem[i]);
    }
    printf("\n");

    // Recover the Process
    ptrace(PTRACE_DETACH, pid, NULL, NULL);

    return 0;
}

void readmem(pid_t pid, ulong saddr, char *mem, int len)
{
    int i = 0;
	int j = len / long_size;
    char *pmem = mem;   
    union u{
        long val;
        char chars[long_size];
    }data;

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
    }
    mem[len] = ' ';
	return;
}

