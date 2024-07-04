#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "h264.hpp"

	static uint8_t *buf_pos;
	static uint8_t *buf_end;

	bool startcode3(uint8_t *buf)
	{
		/* Поиск в видео-файле кодека h264 стартового кода длиной 3 байта */

		if(buf_end == nullptr || buf == nullptr) return false;
		if(buf_end - buf <= 3) return false;
		
		return (buf[0] == 0 && buf[1] == 0 && buf[2] == 1) ? true : false;
	}

	bool startcode4(uint8_t *buf)
	{
		/* Поиск в видео-файле кодека h264 стартового кода длиной 4 байта */

		if(buf_end == nullptr || buf == nullptr) return false;
		if(buf_end - buf <= 4) return false;

		return (buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 1) ? true : false;
	}

	uint8_t H264GetForbiddenBit(uint8_t header)
	{	
		return header >> 7;	
	}

	uint8_t H264GetReferenceIdc(uint8_t header)
	{
		return (header >> 5) & 0x03;
	}

	uint8_t H264GetPayloadType(uint8_t header)
	{
		return header & 0x1F;
	}

	uint8_t H264SetForbiddenBit(uint8_t forbiddenBit, uint8_t header)
	{
		header = (forbiddenBit << 7) | (header & 0x7F);

		return header;
	}

	uint8_t H264SetReferenceIDC(uint8_t referenceIdc, uint8_t header)
	{
		header = ((referenceIdc << 5) & 0x60) | (header & 0x9F);

		return header;
	}

	uint8_t H264SetPayloadType(uint8_t payloadType, uint8_t header)
	{
		header = (payloadType & 0x1F) | (header & 0xE0);

		return header;
	}

	uint8_t H264GetStapaHeader(uint8_t header)
	{
		header = (header & 0xE0) | 24;

		return header;
	}

	uint8_t H264GetFuaIndicator(uint8_t header)
	{
		header = (header & 0xE0) | 28;

		return header;
	}

	uint8_t H264GetFuaHeader(uint8_t header)
	{
		header = header & 0x1F;

		return header;
	}

	uint8_t H264GetFuaStartBit(uint8_t fuaHeader)
	{
		uint8_t startBit = (fuaHeader & 0x80) >> 7;

		return startBit;
	}

	uint8_t H264GetFuaEndBit(uint8_t fuaHeader)
	{
		uint8_t endBit = (fuaHeader & 0x40) >> 6;

		return endBit;
	}

	uint8_t H264SetFuaStartBit(uint8_t startBit, uint8_t fuaHeader)
	{
		fuaHeader = (fuaHeader & 0x1F) | (startBit << 7);
		
		return fuaHeader;
	}

	uint8_t H264SetFuaEndBit(uint8_t endtBit, uint8_t fuaHeader)
	{
		fuaHeader = (fuaHeader & 0x1F) | (endtBit << 6);

		return fuaHeader;
	}

	uint8_t H264ResetFuaBits(uint8_t fuaHeader)
	{
		return fuaHeader & 0x1F;
	}

	H264NalUnit::H264NalUnit()
	{
		header = 0;

		m_pos       = nullptr;
		m_end       = nullptr;
		m_startCode = nullptr;
	}

	uint8_t H264NalUnit::getForbiddenBit() const
	{
		return header >> 7;
	}

	uint8_t H264NalUnit::getReferenceIDC() const
	{
		return (header >> 5) & 0x03;
	}

	uint8_t H264NalUnit::getPayloadType() const
	{
		return header & 0x1F;
	}

	int H264NalUnit::payload(char *buf, int bufsize)
	{
		if(m_pos == nullptr || m_end == nullptr) return -1;

		int i = 0;
		uint8_t *cur = m_pos;
		
		while(i < bufsize && cur != m_end)
		{
			*buf++ = *cur++;
			 i++;
		}

		return i;
	}

	void H264NalUnit::init()
	{
		if(m_startCode == nullptr) return;
		if(m_pos == nullptr || m_end == nullptr) return;
		if(startcode3(m_startCode) == false && startcode4(m_startCode) == false) return;

		header = *m_pos;
	}

	void H264NalUnit::clear()
	{
		m_pos       = nullptr;
		m_end       = nullptr;
		m_startCode = nullptr;		
	}

	H264AccessUnit::H264AccessUnit()
	{
		m_pos     = nullptr;
		cur       = nullptr;
		m_end     = nullptr;
		naluCount = 0;
	}

	void H264AccessUnit::parseNalUnit(H264NalUnit & nu)
	{
		if(cur == nullptr) return;
	 	if(m_pos == nullptr || m_end == nullptr) return;
		if(startcode3(cur) == false && startcode4(cur) == false) return;
		
		nu.setStartCode(cur);
		while(*cur++ != 1);
		nu.setPos(cur);

		while(cur != m_end)
		{
			if(startcode3(cur) || startcode4(cur)) break;
			cur++;
		}

		nu.setEnd(cur);
		nu.init();
	}

	void H264AccessUnit::init()
	{
		if(buf_pos == nullptr) return;
	 	if(m_pos == nullptr || m_end == nullptr) return;

		cur = m_pos;
		naluCount = 0;

		while(cur != m_end)
		{
			if(startcode3(cur) || startcode4(cur)) 
			{
				while(*cur++ != 1);
				naluCount++;
			}
			else cur++;
		}

		cur = m_pos;
	}

	void H264AccessUnit::reset()
	{
		cur = m_pos;
		naluCount = 0;		
	}

	void H264AccessUnit::clear()
	{
		cur   = nullptr;
		naluCount = 0;
	}

	H264Parser::H264Parser()
	{
		/* Конструктор класса по умолчанию */

		cur = nullptr;
		auCount = 0;
	}


	int H264Parser::loadFile(const char *filename)
	{
		/* Загрузка потока видео-файла h264 в буфер */
		
		if(filename == nullptr)
		{
			printf("H264loadFile(): invalid argument");
			return -1;
		}
		
		int fd, bufsize;
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
		
		bufsize = (int) file_info.st_size + 1;	
		buf_pos = (uint8_t *) calloc(bufsize, sizeof(uint8_t));

		if(buf_pos == nullptr)
		{
			printf("H264::loadFile(): allocation error");
			return -1;
		}

		if(read(fd, buf_pos, bufsize) < 0)
		{
			perror("read");
			return -1;
		}

		buf_pos[bufsize] = '\0';
		buf_end = buf_pos + bufsize;

		return bufsize;
	}

	void H264Parser::parseAccessUnit(H264AccessUnit & au)
	{
		if(cur == nullptr) return;
		if(buf_pos == nullptr || buf_end == nullptr) return;
		if(startcode3(cur) == false && startcode4(cur) == false) return;

		uint8_t header;
		uint8_t *temp = cur;

		while(cur != buf_end && *cur != 1) cur++;
		if(cur == buf_end) return;
		cur++;

		header = *cur;
		if(H264GetPayloadType(header) != 9 && H264GetPayloadType(header) != 10) return;
		au.setPos(temp);

		while(cur != buf_end)
		{
			if(startcode3(cur) || startcode4(cur))
			{
				temp = cur;
				while(*cur++ != 1);

				header = *cur;
				if(H264GetPayloadType(header) == 9 || H264GetPayloadType(header) == 10)
				{
					au.setEnd(temp);	
					cur = temp;				
					break;
				}
			}

			cur++;
		}

		if(cur == buf_end) au.setEnd(buf_end);
		au.init();
	}

	void H264Parser::init()
	{
		if(buf_pos == nullptr) return;

		cur = buf_pos;

		if(startcode3(cur) == false && startcode4(cur) == false) return;

		uint8_t header;
		
		while(cur != buf_end)
		{
			if(startcode3(cur) || startcode4(cur))
			{
				while(*cur++ != 1);

				header = *cur;
				if(H264GetPayloadType(header) == 9 || H264GetPayloadType(header) == 10) auCount++;
			}

			cur++;
		}

		cur = buf_pos;
	}

	void H264Parser::reset()
	{
		cur = buf_pos;
		auCount = 0;
	}

	void H264Parser::clear()
	{
		cur = nullptr;
		auCount = 0;
	}

	H264Packer::H264Packer()
	{
		mode = UnknowMode;

		m_pos = nullptr;
		m_cur = nullptr;
		m_end = nullptr;

		au_pos = nullptr;
		au_cur = nullptr;
		au_end = nullptr;
	}

	void H264Packer::setPos(uint8_t *pos)
	{
		if(m_cur != nullptr) return;

		m_pos = pos;
	}

	void H264Packer::setEnd(uint8_t *end)
	{
		if(m_cur != nullptr) return;

		m_end = end;
	}

	void  H264Packer::setBuf(uint8_t *buf, int bufsize)
	{
		if(m_cur != nullptr) return;

		m_pos = buf;
		m_end = m_pos + bufsize;		
	}

	int H264Packer::pack(H264Parser & parser)
	{
		if(m_cur == nullptr) return -1;

		H264NalUnit nu;
		H264AccessUnit au;

		uint32_t payloadSize = 0;
		uint8_t *naluPos, nri = 0;
		uint8_t  fuaIndicator, fuaHeader, stapaHeader = 0;

		int len, packetNumber, otherSize;

		if(m_cur < m_end)
		{
			if(mode == UnknowMode)
			{
				mode = StapaMode;
				parser.parseAccessUnit(au);
				if(au.getSize() == 0) return 0;
				m_cur = m_pos;
				au_end = au.getEnd();
			}

			if(mode == FuaMode)
			{
				if(au_cur != au_end)
				{
					*m_cur++ = H264GetFuaIndicator(*au_pos);
					*m_cur++ = H264GetFuaHeader(*au_pos);

					if(au_cur == au_pos)
					{
						*(m_cur - 1) = H264SetFuaStartBit(1, *(m_cur - 1));
					}

					while(m_cur != m_end && au_cur != au_end) 
						*m_cur++ = *au_cur++;

					if(au_cur == au_end)
					{
						*(m_pos + 1) = H264SetFuaEndBit(1, *(m_pos + 1));

						mode = UnknowMode;
					}
					
					len = getBufLen();
					m_cur = m_pos;
					return len;
				}
			}

			if(mode == SingleMode)
			{
				while(au_cur != au_end) *m_cur++ = *au_cur++;
				mode = UnknowMode;
				len = getBufLen();
				m_cur = m_pos;
				return len;	
			}
			
			if(mode == StapaMode)
			{
				while(au_cur != au_end)
				{
					au.parseNalUnit(nu);
					au_pos = nu.getPos();
					au_cur = au_pos;

					// Обработка случая, когда размер nal unit
					// либо больше размера самого буфера, либо
					// больше, чем размер свободного места
					// в буфере

					if(nu.getSize() > getBufSize() - getBufLen() - 2)
					{
						mode = nu.getSize() > getBufSize() ? FuaMode : SingleMode;
					  *m_pos = H264SetReferenceIDC(nri, *m_pos);
						len = getBufLen();
						m_cur = m_pos;
						return len;
					}

					payloadSize = nu.getSize();
					nri = nu.getReferenceIDC() > nri ? nu.getReferenceIDC() : nri;
					
					if(m_cur == m_pos)
					{
						stapaHeader = H264GetStapaHeader(nu.getHeader());
						*m_cur++ = stapaHeader;
					}

					*m_cur++ = (uint8_t)((payloadSize >> 8) & 0x000000FF);
					*m_cur++ = (uint8_t)(payloadSize & 0x000000FF);

					while(au_cur != nu.getEnd()) *m_cur++ = *au_cur++;
				}

				// Обработка случая, когда все nal unit, которые 
				// содержатся в access unit, помещаются в буфер

				*m_pos = H264SetReferenceIDC(nri, *m_pos);
				 len = getBufLen();
				 m_cur = m_pos;
				 mode = UnknowMode;
				 return len;
			}
		}

		return 0;
	}

	void H264Packer::init()
	{
		if(m_pos == nullptr || m_end == nullptr) return;

		m_cur = m_pos;
	}

	void H264Packer::reset()
	{
		mode = UnknowMode;
		m_cur = m_pos;

		au_pos = nullptr;
		au_cur = nullptr;
		au_end = nullptr;
	}

	void H264Packer::clear()
	{
		mode = UnknowMode;

		m_pos = nullptr;
		m_cur = nullptr;
		m_end = nullptr;

		au_pos = nullptr;
		au_cur = nullptr;
		au_end = nullptr;
	}

	H264Unpacker::H264Unpacker()
	{
		m_pos = nullptr;
		m_cur = nullptr;
		m_end = nullptr;		
	}

	void H264Unpacker::clear()
	{
		m_pos = nullptr;
		m_cur = nullptr;
		m_end = nullptr;		
	}
