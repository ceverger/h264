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

	int temp, ret;
	int size = 1400, i = 0, j = 0;
	uint8_t *bufIN, *bufOUT, packet[size];

	H264::NalUnit nu;
	H264::AccessUnit au;
	H264::Stream in, out;
	H264::Packer packer;
	H264::Unpacker unpacker;

	ret = fileLoad(argv[1], (void **) &bufIN); // Загрузка потока h264 в буфер
	if(ret < 0) return 1;

	bufOUT = new uint8_t[ret];
	if(bufOUT == nullptr) return 1;

	packer.setBuf(packet, size);
	in.setBuf(bufIN, ret);
	out.setBuf(bufOUT, ret);
	unpacker.setBuf(bufOUT, ret);

	in.setBuf(bufIN, ret);

	while(in.nextAccessUnit(au).isInit())
	{
		printf("au_num = %d\n", j);

		do
		{
			ret = packer.pack(au);

			if(ret > 0) 
			{
				printf("   packet_num = %d, pack_size = %d\n", i, ret);
				ret = unpacker.unpack(packet, ret);
				printf("   unpacker_size = %d\n", ret);
			}

			i++;
		}
		while(ret > 0);

		i = 0;
		j++;
	}

	while(out.nextAccessUnit(au).isInit())
	{
		while(au.nextNalUnit(nu).isInit())
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
	}
	
	return 0;
}
