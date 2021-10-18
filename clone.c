// Author:   Adrien Mahieux
// Twitter:  @saruspete
// License:  "THE BEER-WARE LICENSE" (Revision 42):
//   As long as you retain this notice you can do whatever you want with this stuff.
//   If we meet some day, and you think this stuff is worth it, you can buy me a beer in return


#define _GNU_SOURCE
#include <sched.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/unistd.h>
#include <linux/kernel.h>
#include <linux/futex.h>
#include <syscall.h>
#include <sys/sysinfo.h>
#include <sys/mman.h>


#define FSIZE  1024
// Just enough to use a var
#define STACK_SIZE 64



int doContinue = 1, futexStart;
unsigned int tMax = 0, loadExit = 0;

int futex_wake(void* addr, int n){
	return syscall(SYS_futex, addr, FUTEX_WAKE, n, NULL, NULL, 0);
}

static int doNothing(void* arg) {
	futexStart = 0;
	syscall(SYS_futex, &futexStart, 0, NULL, NULL, 0);

	int i = 0;
	while (doContinue) {
		i++;
	}

	return 0;
}


int main(int argc, char* argv[]) {

	if (argc < 2) {
		printf("Usage: %s <load_to_generate> [load_to_exit_at]\n", argv[0]);
		printf("\n");
		printf("Load can be 'max' to spawn as many as possible\n");
		return EXIT_FAILURE;
	}

	// parse input
	if (argc >= 2) {
		if (strcmp(argv[1], "max") == 0) {
			tMax = -1;
		}
		else if (sscanf(argv[1], "%u", &tMax) != 1) {
			printf("Error: load_to_generate must be a full number");
			return EXIT_FAILURE;
		}
	}
	if (argc >= 3) {
		if (sscanf(argv[2], "%u", &loadExit) != 1) {
			printf("Error: load_to_exit_at must be a full number");
			return EXIT_FAILURE;
		}
	}
	else {
		loadExit = tMax;
	}

	printf("Check consumption with 'grep kB /proc/%u/status' (don't use ps/top)\n", getpid());
	printf("Will try to run %u threads and stop at %u load\n", tMax, loadExit);

	int cloneFlags = CLONE_VM | CLONE_THREAD | CLONE_SIGHAND;
				//| CLONE_FS | CLONE_FILES  | CLONE_SYSVSEM
				//	| CLONE_SETTLS | CLONE_PARENT_SETTID
				//	| CLONE_CHILD_CLEARTID;
	int tCnt;

	// Create all our threads
	for (tCnt = 0; tCnt < tMax; tCnt++) {
		void *cloneStack = mmap( 0, STACK_SIZE, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
		// The glibc clone wrapper wants a stack...
		if (clone(&doNothing, cloneStack + STACK_SIZE, cloneFlags, NULL) == -1) {
			if (tMax == -1) {
				tMax = tCnt;
				break;
			}
			else {
				perror("clone");
				return EXIT_FAILURE;
			}
		}
	}

	// Moving myself out of loaded cpus
	cpu_set_t  cpuMask;
	CPU_ZERO(&cpuMask);
	CPU_SET(0, &cpuMask);
	sched_setaffinity(0, sizeof(cpuMask), &cpuMask);

	// Unleash the beast
	syscall(SYS_futex, &futexStart, FUTEX_WAKE, tMax, NULL, NULL, 0);
	printf("Started %u threads\n", tMax);

	// And a bit of info
	unsigned long load1=0, loadLast=0, timeGross=0;
	float loadFactor = 1.l / (1 << SI_LOAD_SHIFT);
	struct sysinfo sysinf;

	while (load1 < loadExit) {

		if (sysinfo(&sysinf)) {
			perror("sysinfo");
		}
		else {
			load1 = (unsigned long) sysinf.loads[0] * loadFactor;
			printf("%5lu Load: %lu (%lu)\n", timeGross, load1, load1 - loadLast);
			loadLast = load1;
		}

		// loadavg is computed every 5 sec
		sleep(5);
		timeGross += 5;
	}
	doContinue = 0;


	return EXIT_SUCCESS;
}
