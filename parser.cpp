#include <cstdio>
#include "h264.hpp"

int main(int argc, char *argv[])
{
	int n = 0, ret;
	int i = 0, j = 0;
	int auCount, naluCount;

	int bufsize = 1400;
	uint8_t buf[bufsize];

	H264Parser parser;
	H264Packer packer;

	ret = parser.loadFile("test.264");
	parser.init();

	auCount = parser.getAUCount();

	while(j < auCount)
	{
		H264AccessUnit au;
		parser.parseAccessUnit(au);
		naluCount = au.getNaluCount();

		printf(
				 "\n\naccess_unit_num = %d; access_unit_size = %d; nalu_count = %d\n\n",
				  j,
				  au.getSize(),
				  naluCount		    
			   );

		while(i < naluCount)
		{
			H264NalUnit nu;
			au.parseNalUnit(nu);
			printf(
					 "   startcode_len = %d   nalu_size = %-5d   forbidden_bit = %d   reference_idc = %d   payload_type = %d\n",
						  nu.getStartCodeLen(),
						  nu.getSize(),
						  nu.getForbiddenBit(),
						  nu.getReferenceIDC(),
						  nu.getPayloadType()
					);
			i++;
		}

		i = 0;
		j++;
	}

	return 0;
}
