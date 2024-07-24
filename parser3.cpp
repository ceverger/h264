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
	int naluCount = 0, auCount; // Счётчики nal unit и access unit

	H264::NalUnit nu;
	H264::AccessUnit au;
	H264::Stream stream;

	ret = fileLoad(argv[1], (void **) &buf); // Загрузка потока h264 в буфер
	if(ret < 0) return 1;

	stream.setBuf(buf, ret); // Инициализация буфера с потоком h264
	std::cout << stream.getSize() <<  std::endl;

	auCount = stream.accessUnitCount(); // Получение количества access unit в потоке h264

	while(j < auCount) // Парсинг всех access unit в потоке и вывод информации он их
	{
		stream.nextAccessUnit(au);      // Парсинг access unit
		naluCount += au.nalUnitCount(); // Увеличение сётчика nalu unit

		printf("\nau_number = %d, au_size = %d\n", j, au.getSize());

		while(i < naluCount) // Парсинг всех nal unit и вывод информации о нем
		{
			au.nextNalUnit(nu); // Парсинг очередного nal unit
			printf(
						"   nalu_number = %d, startCode_len = %d, forbiddenBit = %d, referenceIDC = %d, payloadType = %d, payloadLen = %d\n",
						 i,
						 nu.getStartCodeLen(),
						 nu.getForbiddenBit(),
						 nu.getReferenceIDC(),
						 nu.getPayloadType(),
						 nu.getSize()
					);

			nu.clear();
			i++;
		}

		au.clear();
		j++;
	}

	return 0;
}
