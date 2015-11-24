#define _GNU_SOURCE
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <signal.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include "err.h"
#include <sys/types.h>
#include <sys/wait.h>

const int PAGE_NO = 10;
const int PAGE_SIZE = 4096;

static int print_maps(void);

static void pr_errno(const char *msg)
{
	printf("error: %s, %s", strerror(errno), msg);
}

static void pr_err(const char *msg)
{
	printf("error: %s", msg);
}

static int test_1(void)
{
	int pid = 1; 
	int i = 0;

	printf("Parent pid is %d\n",getpid());
	print_maps();
	pid = fork();

	if (pid == 0) {
		printf("child pid is %d\n",getpid());
		char* p = (char *)mmap(0, PAGE_SIZE * PAGE_NO, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
		printf("%p\n",p);
		if (p == NULL) {
			printf("error: %s\n",strerror(errno));
			exit(1);
		}
		
		
		while (i < 5) {
			p[i * PAGE_SIZE] = '0';
			i++;
		}
		
		print_maps();
		printf("test\n");
		munmap(p, PAGE_SIZE * PAGE_NO);
		exit(0);
	}

	return 0;
}

static int test_2(void)
{
	return 0;
}

static int test_3(void)
{
	return 0;
}

static int test_4(void)
{
	return 0;
}

static int test_5(void)
{
	return 0;
}

static int test_6(void)
{
	return 0;
}

static int test_7(void)
{
	return 0;
}

int main(int argc, char **argv)
{
	/*
	 * Change this main function as you see fit.
	 */
	test_1();
	test_2();
	test_3();
	test_4();
	test_5();
	test_6();
	test_7();
	
	while (wait(0) > 0);
	return 0;
}

static int print_maps(void)
{
	/*
	 * You may not modify print_maps().
	 * Every test should call print_maps() once.
	 */
	char *path;
	char str[25000];
	int fd;
	int r, w;

	path = "/proc/self/maps";
	printf("%s:\n", path);

	fd = open(path, O_RDONLY);

	if (fd < 0)
		pr_errno(path);

	r = read(fd, str, sizeof(str));

	if (r < 0)
		pr_errno("cannot read the mapping");

	if (r == sizeof(str))
		pr_err("mapping too big");

	while (r) {
		w = write(1, str, r);
		if (w < 0)
			pr_errno("cannot write to stdout");
		r -= w;
	}

	return 0;
}
