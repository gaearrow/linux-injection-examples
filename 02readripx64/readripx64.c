/********************************
* Filename	: readripx64.c
* Author	: Gaearrow
* Version	: 1.0
* Abstract	: Read the rip of the specified process
* Platform	: gcc 4.1.2 based on CentOS 5.3 x86_64(20170217)
* Compiling	: gcc -o readripx64 readripx64.c
* Usage		: ./readripx64 <pid>
********************************/

#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <linux/user.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{ 
	pid_t pid;
	struct user_regs_struct regs;
	
	/* Process Input */
	if(argc != 2) {
		printf("usage: ./readripx64 <pid> \n");
		return 1;
	}	
	pid = atoi(argv[1]);

	/* Attach to the process */
	ptrace(PTRACE_ATTACH, pid, NULL, NULL);
	if (pid != wait(NULL)){
		printf("Attach unsuccessfully!\n");
		return 1;
	}else
	{
		printf("Attach to the specified process pid %d successfully!\n",pid);
	}

	/* Read REGS & Output RIP */
	ptrace(PTRACE_GETREGS, pid, NULL, &regs);
	printf("Instruction pointer(rip): %lx \n", regs.rip);
	
	/* Recover the Process */
	ptrace(PTRACE_DETACH, pid, NULL, NULL);

	return 0;
}
