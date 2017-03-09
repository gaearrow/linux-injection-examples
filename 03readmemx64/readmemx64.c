/********************************
* Filename	: readmemx64.c
* Author	: Gaearrow
* Version	: 1.0
* Abstract	: Read the specified memory space of the specified process
* Platform	: gcc 4.1.2 based on CentOS 5.3 x86_64(20170308)
* Compiling	: gcc -o readmemx64 readmemx64.c
* Usage		: ./readmemx64 <pid> <start address in hex formats> <length>
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
#define MAX_SIZE_READ 1048576	 		// 1 MB = 1024*1024 bytes
const int LONG_SIZE = sizeof(long);		// i386:4	x86_64:8

ulong readMem(pid_t pid, ulong ulSAddr, char *szMem, ulong ulLen);

int main(int argc, char *argv[])
{ 
	pid_t pid;
	ulong ulSAddr;
	int ulLen;
 
	/* Process Input */
	if(argc != 4) {
		printf("usage: ./readmemx64 <pid> <start address(eg:2ad316875000)> "
					"<length(max:1048576 bytes)> \n");
		return -1;
	}
	pid = atoi(argv[1]);
	sscanf(argv[2],"%lx",&ulSAddr);
	ulLen = atoi(argv[3]);

	/* Reading Length Check */
	if (ulLen > MAX_SIZE_READ){
		printf("ERROR:Length is overflow! \n");
		return -1;
	}

	/* Read memory */	
	uchar *szMem = (uchar*)malloc(ulLen+1);	
	if (szMem == NULL) {
		printf("ERROR:Alloc memory unsuccessfully! \n");
		return -1;
	}
	if (readMem(pid, ulSAddr, szMem, ulLen) < 0){
		printf("ERROR:Read memeory unsuccessfully! \n");
		return -1;
	}

	/* Output memory */
	int i;
	for (i = 0; i < ulLen; i++){
		if (i % 16 == 0) printf("\n");
		printf("%02X ",szMem[i]);
	}
	printf("\n");
	
	free(szMem);
	return 0;
}

ulong readMem(pid_t pid, ulong ulSAddr, char *szMem, ulong ulLen)
{
	int i = 0;
	int j = ulLen / LONG_SIZE;
	char *pMem = szMem;   
	union u{
		long val;
		char chars[LONG_SIZE];
	}data;

	/* Attach to the process */
	ptrace(PTRACE_ATTACH, pid, NULL, NULL);
	if (pid != wait(NULL))
	{
		printf("Attach unsuccessfully!\n");
		return -1;
	}else
	{
		printf("Attach to the specified process pid %u successfully!\n",pid); 	// Info.
	}

	while (i < j)
	{
		data.val = ptrace(PTRACE_PEEKDATA, pid, ulSAddr + i*LONG_SIZE, NULL);
		memcpy(pMem, data.chars, LONG_SIZE);
		++i;
		pMem += LONG_SIZE;
	}
	j = ulLen % LONG_SIZE;
	if (j != 0)
	{
		data.val = ptrace(PTRACE_PEEKDATA, pid, ulSAddr + i*LONG_SIZE, NULL);
		memcpy(pMem, data.chars, j);
		pMem += j;
	}

	/* Recover the Process */
	ptrace(PTRACE_DETACH, pid, NULL, NULL);
	printf("Read %d bytes data at 0x%lx \n",pMem-szMem,ulSAddr);
	return pMem-szMem;
}
