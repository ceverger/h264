#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

	int fileLoad(const char *filename, void **buf)
	{
		/* Загрузка содержимого файла в буфер */
		
		if(filename == nullptr || buf == nullptr)
		{
			printf("fileLoad(): invalid argument");
			return -1;
		}
		
		int fd;
		int size;
		uint8_t *store = (uint8_t *) *buf;
		struct stat file_info;

		fd = open(filename, O_RDONLY);
		
		if(fd < 0)
		{
			perror("open()");
			return -1;
		}
		
		if(fstat(fd, &file_info) < 0)
		{
			perror("fstat()");
			return -1;
		}
		
		size = (int) file_info.st_size;	
		store = (uint8_t *) calloc(size, sizeof(uint8_t));

		if(store == nullptr)
		{
			perror("calloc");
			return -1;
		}

		if(read(fd, store, size) < 0)
		{
			perror("read");
			return -1;
		}

		*buf = store;

		return size;
	}
