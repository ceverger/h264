#include <stdio.h>
#include "files.hpp"
#include "byte.hpp"
#include "h264.hpp"
#include <iostream>

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		std::cout << "Invalid arguments!" << std::endl;
		return 1;
	}

	uint8_t *buf;
	int ret, i = 0, j = 0;

	H264::NalUnit nu;
	H264::AccessUnit au;
	H264::Stream stream;

	ret = fileLoad(argv[1], (void **) &buf); // Загрузка потока h264 в буфер
	if(ret < 0) return 1;

	stream.setBuf(buf, ret);
	std::cout << stream.getSize() <<  std::endl;

	while(stream.nextAccessUnit(au).isInit()) // Парсинг всех access unit в потоке
	{
		while(au.nextNalUnit(nu).isInit()) // Парсинг всех nal unit
		{
			printf(
						"nalu_number = %d, startCode_len = %d, forbiddenBit = %d, referenceIDC = %d, payloadType = %d, payloadLen = %d\n",
						 i,
						 nu.getStartCodeLen(),
						 nu.getForbiddenBit(),
						 nu.getReferenceIDC(),
						 nu.getPayloadType(),
						 nu.getSize()
					);
			i++;
		}

		j++;
	}

	return 0;
}
