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
#include <limits.h>

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

	pid = fork();
	if (pid == 0) {
		char *p;

		printf("\n........START TEST CASE 1..........\n");
		p = (char *)mmap(0, PAGE_SIZE * PAGE_NO, PROT_READ,
			MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
		printf("%p\n", p);
		if (p == NULL) {
			printf("error: %s\n", strerror(errno));
			exit(1);
		}

		print_maps();
		munmap(p, PAGE_SIZE * PAGE_NO);
		exit(0);
	}

	wait(0);
	printf("........END TEST CASE 1..........\n");
	return 0;
}

static int test_2(void)
{
	int pid = 1;
	int i = 0;

	pid = fork();
	if (pid == 0) {
		char *p;

		printf("\n........START TEST CASE 2..........\n");
		p = (char *)mmap(0, PAGE_SIZE * PAGE_NO, PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
		if (p == NULL) {
			printf("error: %s\n", strerror(errno));
			exit(1);
		}

		while (i < PAGE_NO) {
			p[i * PAGE_SIZE] = 'a';
			i++;
		}

		sleep(1);
		print_maps();
		munmap(p, PAGE_SIZE * PAGE_NO);
		exit(0);
	}

	wait(0);
	printf("........END TEST CASE 2..........\n");
	return 0;
}

static int test_3(void)
{
	int pid = 1;
	int i = 1;
	char *p;

	pid = fork();

	if (pid == 0) {
		printf("\n........START TEST CASE 3..........\n");
		p = (char *)mmap(0, PAGE_SIZE * PAGE_NO, PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
		printf("%p\n", p);
		if (p == NULL) {
			printf("error: %s\n", strerror(errno));
			exit(1);
		}

		while (i < PAGE_NO) {
			p[i * PAGE_SIZE] = 'a';
			i += 2;
		}

		sleep(1);
		print_maps();
		munmap(p, PAGE_SIZE * PAGE_NO);
		exit(0);
	}

	wait(0);
	printf("........END TEST CASE 3..........\n");
	return 0;
}

static int test_4(void)
{
	int pid = 1;
	int i = 0;
	char *p, *tmp;

	printf("\n........START TEST CASE 4..........\n");

	tmp = (char *)mmap(0, PAGE_SIZE , PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	p = (char *)mmap(tmp + 1, PAGE_SIZE * PAGE_NO, PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	munmap(tmp, PAGE_SIZE);
	if (p == NULL) {
		printf("error: %s\n", strerror(errno));
		exit(1);
	}

	while (i < PAGE_NO/2) {
		p[i * PAGE_SIZE] = 'a';
		i++;
	}

	pid = fork();

	if (pid == 0) {
		printf("child pid is %d\n", getpid());
		sleep(1);
		print_maps();
		exit(0);
	}
	wait(0);
	munmap(p, PAGE_SIZE * PAGE_NO);
	printf("\n........END TEST CASE 4..........\n");

	return 0;
}

static int test_5(void)
{
	int pid = 1;
	int i = PAGE_NO - 1;
	char *p, *tmp;

	printf("\n........START TEST CASE 5..........\n");
	tmp = (char *)mmap(0, PAGE_SIZE , PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	p = (char *)mmap(tmp + 1, PAGE_SIZE * PAGE_NO, PROT_READ | PROT_WRITE,
		MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
	munmap(tmp, PAGE_SIZE);

	if (p == NULL) {
		printf("error: %s\n", strerror(errno));
		exit(1);
	}

	while (i > PAGE_NO/2) {
		p[i * PAGE_SIZE] = 'a';
		i--;
	}

	pid = fork();

	if (pid == 0) {
		i = 0;
		while (i < PAGE_NO/2 - 1) {
			p[i * PAGE_SIZE] = 'a';
			i += 1;
		}

		sleep(1);
		print_maps();
		exit(0);
	}

	wait(0);
	munmap(p, PAGE_SIZE * PAGE_NO);
	printf("........END TEST CASE 5..........\n");

	return 0;
}

static int test_6(void)
{
	int pid = 1;
	int MAX_PAGE = 2000;
	char *p;

	printf("\n........START TEST CASE 6..........\n");
	pid = fork();
	if (pid == 0) {
		p = (char *)mmap(0, PAGE_SIZE * MAX_PAGE, PROT_READ,
			MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
		printf("%p\n", p);
		if (p == NULL) {
			printf("error: %s\n", strerror(errno));
			exit(1);
		}

		print_maps();
		munmap(p, PAGE_SIZE * PAGE_NO);
		exit(0);
	}

	wait(0);
	printf("........END TEST CASE 6..........\n");

	return 0;
}

static int test_7(void)
{
	int i = 0;
	int j = 0;
	int  MAX_ITER = USHRT_MAX - 1;
	int MAX_NO = 2000;
	char *p[MAX_ITER];

	printf("\n........START TEST CASE 7..........\n");
	while (j < MAX_ITER - 1) {
		i = 0;
		p[j] = (char *)mmap(0, PAGE_SIZE * MAX_NO,
			PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
		if (p[j] == NULL) {
			printf("error: %s\n", strerror(errno));
			exit(1);
		}

		while (i < PAGE_SIZE * MAX_NO) {
			p[j][i] = 'a';
			i++;
		}

		j++;
	}

	j = 0;
	while (j < MAX_ITER - 1) {
		munmap(p[j], PAGE_SIZE * MAX_NO);
		j++;
	}
	printf("........END TEST CASE 7..........\n");

	return 0;
}

int main(int argc, char **argv)
{
	test_1();
	test_2();
	test_3();
	test_4();
	test_5();
	test_6();
	test_7();

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
