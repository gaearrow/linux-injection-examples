/********************************
* Filename	: counter.c
* Author	: Gaearrow
* Version	: 1.0
* Abstract	: The process to be injected
* Platform	: gcc 4.1.2 based on CentOS 5.3 x86_64(20170217)
* Compiling	: gcc -o counter counter.c
* Usage		: ./counter
********************************/

#include <sys/time.h>
#include <stdio.h>

long long timeum(){
    struct timeval tim; 
    gettimeofday (&tim , NULL);
    return (long long)tim.tv_sec*1000000+tim.tv_usec;
}

int main()
{
    int i;
	int pid=getpid();
    long long start,tmp;
    start = timeum();
    for(i = 0; i < 9999; ++i){
        sleep(3);
        tmp = timeum();
        printf("PID: %d     Counter: %04d     ", pid, i);
        printf("Time Interval: %lld\n",tmp-start);
        start = tmp;
    }
    return 0;
}
