#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include "file.h"

int GetFileSize(FILE *fd);
int main(int argc, const char *argv[])
{
	int fd;
	FILE *fd_r, *fd_w;
	char *read_file_path_name = "/home/read_file.txt", *write_file_path_name = "/home/write_file.txt", *buffer;
	myfile file;
	
	if((fd = open("/dev/mycdev",O_RDWR)) < 0){
		perror("open error");
		exit(EXIT_FAILURE);
	}

	fd_r = fopen(read_file_path_name, "r");
	if(fd_r == NULL){
		printf(" Err: Open %s failed.", read_file_path_name);
		return;
	}
	
	fd_w = fopen(write_file_path_name, "w");
	if(fd_w == NULL){
		printf(" Err: Open %s failed.", write_file_path_name);
		return;
	}

	file.fileSize = GetFileSize(fd_w);
	file.buf = (char*)malloc(file.fileSize);
	if (file.buf == NULL)
	{
        printf(" %s:%d Malloc fail!\n", __func__, __LINE__);
        fclose(fd_w);
        return -1;
    }
	fread(file.buf, 1, file.fileSize, fd_w);
	ioctl(fd, FILE_W, &file);

	ioctl(fd, FILE_R, buffer);
	fwrite(buffer, 1, file.fileSize, fd_r);

	/*while(1){
		sleep(1);
	}*/

	free(file.buf);
	close(fd);
	fclose(fd_r);
	fclose(fd_w);
	return 0;
}

int GetFileSize(FILE *fd)
{
	int length;
	// Move to End
	fseek(fd, 0, SEEK_END);
	length = ftell(fd);
	// Move to first
	fseek(fd, 0, SEEK_SET);
	return length;
}
