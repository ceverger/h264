#include "h264packer.hpp"

		uint8_t GetForbiddenBit(uint8_t header)
		{	
			/* Получение значения защищённого бита */

				return header >> 7;
		}

		uint8_t GetReferenceIDC(uint8_t header)
		{
			/* Получение значения nri */

				return (header >> 5) & 0x03;
		}

		uint8_t GetPayloadType(uint8_t header)
		{
			/* Получение типа полезной нагрузки */

				return header & 0x1F;
		}

		uint8_t SetForbiddenBit(uint8_t forbiddenBit, uint8_t header)
		{
			/* Установка значения защищённого бита */

				header = (forbiddenBit << 7) | (header & 0x7F);

				return header;
		}

		uint8_t SetReferenceIDC(uint8_t referenceIdc, uint8_t header)
		{
			/* Установка значения nri */

				header = ((referenceIdc << 5) & 0x60) | (header & 0x9F);

				return header;
		}

		uint8_t SetPayloadType(uint8_t payloadType, uint8_t header)
		{
			/* Установка типа полезной нагрузки */

				header = (payloadType & 0x1F) | (header & 0xE0);

				return header;
		}

		uint8_t GetStapaHeader(uint8_t header)
		{
			header = (header & 0xE0) | 24;

			return header;
		}

		uint8_t GetFuaIndicator(uint8_t header)
		{
			header = (header & 0xE0) | 28;

			return header;
		}

		uint8_t GetFuaHeader(uint8_t header)
		{
			header = header & 0x1F;

			return header;
		}

		uint8_t GetFuaStartBit(uint8_t fuaHeader)
		{
			uint8_t startBit = (fuaHeader & 0x80) >> 7;

			return startBit;
		}

		uint8_t GetFuaEndBit(uint8_t fuaHeader)
		{
			uint8_t endBit = (fuaHeader & 0x40) >> 6;

			return endBit;
		}

		uint8_t SetFuaStartBit(uint8_t startBit, uint8_t fuaHeader)
		{
			fuaHeader = (fuaHeader & 0x1F) | (startBit << 7);
			
			return fuaHeader;
		}

		uint8_t SetFuaEndBit(uint8_t endtBit, uint8_t fuaHeader)
		{
			fuaHeader = (fuaHeader & 0x1F) | (endtBit << 6);

			return fuaHeader;
		}

		uint8_t ResetFuaBits(uint8_t fuaHeader)
		{
			return fuaHeader & 0x1F;
		}

	H264Packer::H264Packer()
	{
		mode = UnknowMode;

		m_maxSize = 0;

		naluHeader   = 0;
		fuaIndicator = 0;
		fuaHeader    = 0;

		pos = nullptr;
		cur = nullptr;
		end = nullptr;		
	}

	void H264Packer::getMode() const
	{
		return mode;
	}

	void H264Packer::getMaxSize() const
	{
		return m_maxSize;
	}

	void H264Packer::getPayloadLen() const
	{
		return end - pos;
	}

	void H264Packer::setMaxSize(int maxSize)
	{
		m_maxSize = maxSize;
	}

	void H264Packer::setNalu(uint8_t *nalu, int size)
	{
		pos = nalu;
		cur = pos;
		end = pos + size;
	}

	int H264Packer::pack(uint8_t *buf, int size)
	{
		if(m_maxSize <= 0) return -1;
		if(buf == nullptr || size <= 0) return -1;
		if(pos == nullptr || end == nullptr) return -1;

		int buf_pos = buf;
		int buf_end = buf_pos + size;

		if(cur == end)
		{
			reset();
			return 0;
		}

		if(mode == UnknowMode)
		{
			mode = (getPayloadLen() < maxSize) ? SingleMode : FuaMode;
			naluHeader = *cur;
		}

		if(mode == SingleMode)
		{
			while(cur < end && buf_pos < buf_end)
			{
				*buf_pos++ = *cur++;
			}

			return cur - pos;
		}

		if(mode == FuaMode)
		{
			if(cur == pos)
			{
				fuaIndicator = GetFuaIndicator(naluHeader);
				   fuaHeader = GetFuaHeader(naluHeader);
				   fuaHeader = SetFuaStartBit(1, fuaHeader);
			}

			if(cur > pos && cur < end)
			{
				fuaHeader = ResetFuaBits(fuaHeader);
			}

			if(end - cur == buf_end - buf_pos)
			{
				fuaHeader = SetFuaEndBit(1, fuaHeader);
			}

			if(end - cur < buf_end - buf_pos)
			{
				fuaHeader = SetFuaEndBit(1, fuaHeader);
			}

			*buf_pos++ = fuaIndicator;
			*buf_pos++ = fuaHeader;

			while(cur < end && buf_pos < buf_end)
			{
				*buf_pos++ = *cur++;
			}

			return cur - pos;
		}
	}

	void H264Packer::reset()
	{
		mode = UnknowMode;

		m_maxSize = 0;

		naluHeader   = 0;
		fuaIndicator = 0;
		fuaHeader    = 0;

		pos = nullptr;
		cur = nullptr;
		end = nullptr;	
	}

	

