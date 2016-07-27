#include "test7.h"

void test7001() {

	int fd = open("abc.sss", O_CREAT | O_RDWR, 0777);

	int file_length = -1;
	struct stat fileStat;
	if (-1 == fstat(fd, &fileStat)) {
		file_length = 0;
	} else {
		file_length = fileStat.st_size;
	}

	ftruncate(fd, 666888);

	int i = 1;
	i = i + 1;
}

void test7002() {
	char * dir = "./";
	DIR *dp;
	struct dirent *entry;
	struct stat statbuf;
	if ((dp = opendir(dir)) == NULL) {
		fprintf(stderr, "cannot open directory: %s\n", dir);
		return;
	}
//	chdir(dir);
	while ((entry = readdir(dp)) != NULL) {
		lstat(entry->d_name, &statbuf);
		if (S_ISDIR(statbuf.st_mode)) {
			printf("dir %s/\n", entry->d_name);
		} else {
			printf("file %s\n", entry->d_name);
		}
	}
//	chdir("..");
	closedir(dp);
}

void test7003() {
	char a[128] = { 1, 2, 3, 4, 5, 6, 7, 8 };
	float * b;
	b = (float*) a;
	float b1 =b[0];
	float b2=b[1];
	int i = 1;
	i = i + 1;
}

void test7000() {
	test7003();
}
