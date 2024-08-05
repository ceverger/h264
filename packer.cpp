#include <cstdio>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include "files.hpp"
#include "h264.hpp"

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		printf("Invalid arguments!\n");
		return 1;
	}

	uint8_t *buf;
	int ret, i = 0, j = 0;

	int cacheSize = 1400;
	uint8_t cache[cacheSize];

	H264::AccessUnit au;
	H264::Stream stream;
	H264::Packer packer;

	ret = fileLoad(argv[1], (void **) &buf); // Загрузка потока h264 в буфер
	if(ret < 0) return 1;

	stream.setBuf(buf, ret);
	packer.setBuf(cache, cacheSize);

	while(stream.nextAccessUnit(au).isInit())
	{
		printf("au_num = %d\n", j);

		do
		{
			ret = packer.pack(au);
			if(ret > 0) printf("   packet_num = %d, pack_size = %d\n", i, ret);
			i++;
		}
		while(ret > 0);

		i = 0;
		j++;
	}

	return 0;
}
