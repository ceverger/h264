#include <cstdio>
#include "h264.hpp"

int main(int argc, char *argv[])
{
	int auCount;
	int n = -1, ret;
	int bufsize = 1400;
	uint8_t buf[bufsize];

	H264Parser parser;
	H264Packer packer;

	ret = parser.loadFile("test.264");
	parser.init();

	auCount = parser.getAUCount();

	packer.setBuf(buf, bufsize);
	packer.init();

	while(n < auCount)
	{
		if(packer.getMode() == H264Packer::UnknowMode) n++;
		ret = packer.pack(parser);
		if(ret == 0) break;
		printf("n = %d, data size = %d\n", n, ret);
	}

	return 0;
}
