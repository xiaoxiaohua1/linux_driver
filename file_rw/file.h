
#ifndef __FILE_H__
#define __FILE_H__

typedef struct file_operation{
	int fileSize;
	char *buf;
}myfile;


#define FILE_R      _IOR('a',0,myfile)	
#define FILE_W      _IOW('a',1,myfile)	

char *buffer;


#endif











