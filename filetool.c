#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "md5.h"

#define	SingleFileSize	(1024*128)
#define MaxFileNum		(1024)


const char opts[][16] = {"merge","split","remove","md5"};

void merge(char * fileName);
void split(char * fileName);
void md5sum(char * fileName);
void deleteFile(void);

int main(int argc, char* argv[])
{
	if(argc < 3)
	{
		printf("./program  %s|%s|%s file\n",opts[0],opts[1],opts[3]);
		printf("./program  %s\n",opts[2]);
		exit(0);
	}
	if(0 == strcmp(argv[1],opts[0]))
	{
		merge(argv[2]);
	}
	else if(0 == strcmp(argv[1],opts[1]))
	{
		split(argv[2]);
	}
	else if(0 == strcmp(argv[1],opts[2]))
	{
		deleteFile();
	}
	else if(0 == strcmp(argv[1],opts[3]))
	{
		md5sum(argv[2]);
	}
	return 0;
}

void merge(char * fileName)
{
	int iErr = 0, i = 0;
	char name[32] = {0}, buff[SingleFileSize + 128] = {0};
	FILE *fp_o = NULL, *fp_i = NULL;

	fp_o = fopen(fileName,"wb");
	if(!fp_o)
	{
		iErr = 1;
		printf("fopen failed, fileName:%s\n",fileName);
		goto ret;
	}
	for(i = 0; i < MaxFileNum; i++)
	{
		sprintf(name,"%03i.dat",i);
		fp_i = fopen(name,"rb");
		if(!fp_i)
		{
			iErr = 1;
			printf("fopen failed, name:%s\n",name);
			goto ret;
		}
		if(SingleFileSize != fread(buff,1,SingleFileSize,fp_i))
		{
			iErr = 1;
			printf("fread failed not enough data, name:%s\n",name);
			goto ret;
		}
		if(SingleFileSize != fwrite(buff,1,SingleFileSize,fp_o))
		{
			iErr = 1;
			printf("fwrite failed when deal name:%s\n",name);
			goto ret;
		}
		fclose(fp_i);
		fp_i = NULL;
	}

ret:
	printf("%s %s\n",__FUNCTION__,(iErr ? "failed":"success"));
	if(fp_o)
		fclose(fp_o);
	if(fp_i)
		fclose(fp_i);
	return;
}
void split(char * fileName)
{
	int iErr = 0, i = 0;
	char name[32] = {0}, buff[SingleFileSize + 128] = {0};
	FILE *fp_o = NULL, *fp_i = NULL;

	fp_i = fopen(fileName,"rb");
	if(!fp_i)
	{
		iErr = 1;
		printf("fopen failed, fileName:%s\n",fileName);
		goto ret;
	}
	for(i = 0; i < MaxFileNum; i++)
	{
		sprintf(name,"%03i.dat",i);
		fp_o = fopen(name,"wb");
		if(!fp_o)
		{
			iErr = 1;
			printf("fopen failed, name:%s\n",name);
			goto ret;
		}
		if(SingleFileSize != fread(buff,1,SingleFileSize,fp_i))
		{
			iErr = 1;
			printf("fread failed not enough data, name:%s\n",name);
			goto ret;
		}
		if(SingleFileSize != fwrite(buff,1,SingleFileSize,fp_o))
		{
			iErr = 1;
			printf("fwrite failed when deal name:%s\n",name);
			goto ret;
		}
		fclose(fp_o);
		fp_o = NULL;
	}

ret:
	printf("%s %s\n",__FUNCTION__,(iErr ? "failed":"success"));
	if(fp_o)
		fclose(fp_o);
	if(fp_i)
		fclose(fp_i);
	return;
}

void deleteFile(void)
{
	int iRet = 0, iErr = 0, i = 0;
	char name[32] = {0};
	
	for(i = 0; i < MaxFileNum; i++)
	{
		sprintf(name,"%03i.dat",i);
		if(0 != remove(name) && ENOENT != errno)
		{
			printf("remove failed, errno:0x%0x\n",errno);
		}
	}

ret:
	printf("%s %s\n",__FUNCTION__,(iErr ? "failed":"success"));
	
	return;
}

void md5sum(char * fileName)
{
	int iErr = 0, dataSize = 0, i = 0;
	unsigned int tmp = 0;
	unsigned char buff[1024] = {0}, md[32] = {0};
	unsigned int buffLen = 0;
	MD5_CTX ctx = {0};
	FILE *fp = NULL;

	fp = fopen(fileName,"rb");
	if(!fp)
	{
		iErr = 1;
		printf("fopen failed, fileName:%s\n",fileName);
		goto ret;
	}
	MD5_Init(&ctx);
	while(1)
	{
		buffLen = fread(buff,1,sizeof(buff),fp);
		if(buffLen > 0)
			MD5_Update(&ctx,buff,buffLen);
		else
			break;
		dataSize += buffLen;
	}
	if(dataSize <= 0)
	{
		iErr = 1;
		goto ret;
	}
	MD5_Final(md,&ctx);
	for(i = 0; i < 16; i++)
	{
		tmp = md[i];
		printf("%02x",tmp);
	}
	printf("\t\t%s\n",fileName);

ret:
	printf("%s %s\n",__FUNCTION__,(iErr ? "failed":"success"));
	if(fp)
		fclose(fp);
	return;

}