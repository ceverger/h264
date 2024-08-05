#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "byte.hpp"
#include "h264.hpp"

	namespace H264 
	{
		static bool startcode3(uint8_t *buf)
		{
			/* Поиск в видео-файле кодека h264 стартового кода длиной 3 байта */
			
				return (buf[0] == 0 && buf[1] == 0 && buf[2] == 1) ? true : false;
		}

		static bool startcode4(uint8_t *buf)
		{
			/* Поиск в видео-файле кодека h264 стартового кода длиной 4 байта */

				return (buf[0] == 0 && buf[1] == 0 && buf[2] == 0 && buf[3] == 1) ? true : false;
		}

		int Stream::accessUnitCount()
		{
			/* Подсчёт количества access unit в потоке h264 */

				if(!isInit()) return -1;

				int count = 0;
				uint8_t header, *pos = m_pos;
				
				while(pos < m_end)
				{
					if(startcode3(pos) || startcode4(pos))
					{
						while(*pos++ != 1);
						header = *pos;
						if(GetPayloadType(header) == 9 || GetPayloadType(header) == 10) count++;
					}

					pos++;
				}
				
				return count;
		}

		AccessUnit & Stream::nextAccessUnit(AccessUnit & au)
		{
			/* Парсинг очередного access unit из потока h264
				и сохранение информации о нём в объекте au    */


			// Проверка выполнения предусловий.
			// Если предусловия не выполняется,
			// то метод завершает свою работу

				if(!isInit()) return au;
				if(au.isInit()) au.clear();
				if(cur == m_end) return au;

			/* Начало выполнения основного блока метода */

				uint8_t header;
				uint8_t *pos = nullptr, *end = nullptr;

				while(cur < m_end)
				{
					if(startcode3(cur) || startcode4(cur))
					{
						if(pos == nullptr)
						{
							pos = cur;
							while(*cur++ != 1);
							header = *cur;

							if(GetPayloadType(header) == 9 || GetPayloadType(header) == 10)
								au.setPos(pos);
						}
						else
						{
							end = cur;
							while(*cur++ != 1);
							header = *cur;

							if(GetPayloadType(header) == 9 || GetPayloadType(header) == 10)
							{
								au.setEnd(end);
								cur = end;
								break;
							}
						}
					}

					cur++;
				}

				if(cur == m_end) au.setEnd(m_end);

				return au;					
		}

		AccessUnit & Stream::currentAccessUnit(AccessUnit & au) 
		{
			/* Парсинг текущего access unit из потока h264
				и сохранение информации о нём в объекте au    */


			// Проверка выполнения предусловий.
			// Если предусловия не выполняется,
			// то метод завершает свою работу

				if(!isInit()) return au;
				if(au.isInit()) au.clear();

			/* Начало выполнения основного блока метода */

				uint8_t header, *temp, *pos = cur;

				if(pos == m_pos)
				{
					if(!startcode3(pos) || !startcode4(pos)) return au;

					temp = pos;
					while(*pos++ != 1);
					header = *pos;

					if(GetPayloadType(header) != 9) return au;
					au.setPos(temp);

					while(pos < m_end)
					{
						if(startcode3(pos) || startcode4(pos))
						{
							temp = pos;
							while(*pos++ != 1);
							header = *pos;

							if(GetPayloadType(header) == 9 || GetPayloadType(header) == 10)
							{
								au.setEnd(temp);
								break;
							}
							
							pos++;
						}
					}
				}
				else
				{
					au.setEnd(pos);
					pos = (pos == m_end) ? pos - 4 : pos;

					while(pos > m_pos)
					{
						if(startcode3(pos))
						{
							if(pos - 1 >= m_pos && startcode4(pos - 1)) pos--;

							temp = pos;
							while(*pos++ != 1);
							header = *pos;

							if(GetPayloadType(header) == 9 || GetPayloadType(header) == 10)
							{
								au.setPos(temp);
								break;
							}
							else pos = temp;
						}

						pos--;
					}
				}
				
				return au;
			}

			int AccessUnit::nalUnitCount()
			{
				/* Метод подсчитывает количество nalu в потоке h264 */

					if(!isInit()) return -1;

					int count = 0;
					uint8_t *pos = cur;
					
					while(pos < m_end)
					{
						if(startcode3(pos) || startcode4(pos))
						{
							while(*pos++ != 1);
							count++;
						}

						pos++;
					}
					
					return count;
		}

		NalUnit & AccessUnit::nextNalUnit(NalUnit & nu)
		{
			/* Метод парсит очередной nal unit из потока h264
				и сохраняет данные о нём в объекте nu          */

			// Проверка выполнения предусловий.
			// Если предусловия не выполняется,
			// то метод завершает свою работу

				if(!isInit()) return nu;
				if(nu.isInit()) nu.clear();
				if(cur == m_end) return nu;


			/* Начало выполнения основного блока метода */

				uint8_t *pos = nullptr;

				while(cur < m_end)
				{
					if(startcode3(cur) || startcode4(cur))
					{
						if(pos == nullptr)
						{
							pos = cur;
							nu.setPos(pos);
							while(*cur++ != 1);
						}
						else
						{
							nu.setEnd(cur);
							break;					
						}
					}

					cur++;
				}

				if(cur == m_end) nu.setEnd(cur);

				nu.init();

				return nu;
		}

		NalUnit & AccessUnit::currentNalUnit(NalUnit & nu)
		{
			/* Метод парсит текущий nal unit из потока h264
				и сохраняет данные о нём в объекта nu        */

				if(!isInit()) return nu;
				if(nu.isInit()) nu.clear();

			/* Начало выполнения основного блока метода */

				uint8_t *pos = cur;

				if(pos == m_pos)
				{
					if(!startcode3(pos) || startcode4(pos)) return nu;

					nu.setPos(pos);
					while(*pos++ != 1);

					while(pos < m_end)
					{
						if(startcode3(pos) || startcode4(pos))
						{	
							nu.setEnd(pos);
							break;
						}

						pos++;
					}
				}
				else
				{
					nu.setEnd(pos);
					pos = (pos == m_end) ? pos - 4 : pos;

					while(pos > m_pos)
					{
						if(startcode3(pos))
						{
							if(pos - 1 != m_pos && startcode4(pos - 1)) pos--;
							nu.setPos(pos);
							break;
						}

						pos--;
					}
				}

				nu.init();

				return nu;
		}


		NalUnit::NalUnit() : Byte::Array()
		{
			/* Реализация конструктора класса по умолчанию */

			header = 0;
			startCode = nullptr;
		}

		uint8_t NalUnit::getHeader() const
		{
			/* Получение заголовка nal unit */

			return *m_pos;
		}

		uint8_t *NalUnit::getStartCode() const 
		{
			/* Получение указателя на позиция стартового кода */

			return startCode;
		}

		int NalUnit::getStartCodeLen() const 
		{ 
			/* Получение длины стартового кода  */

			return m_pos - startCode;
		}

		uint8_t NalUnit::getForbiddenBit() const
		{
			/* Получение значения защищённого бита */

			return header >> 7;
		}

		uint8_t NalUnit::getReferenceIDC() const
		{
			/* Получение згачени nri */

			return (header >> 5) & 0x03;
		}

		uint8_t NalUnit::getPayloadType() const
		{
			/* Получение значения типа полезной нагрузки */

			return header & 0x1F;
		}

		void NalUnit::setPos(uint8_t *pos)
		{
			/* Установка начальной позиции nal unit
				в потоке h264                        */

			if(isInit()) return;

			m_pos = pos;
		}

		void NalUnit::setEnd(uint8_t *end)
		{
			/* Установка конечной позиции nal unit
				в потоке h264                        */

			if(isInit()) return;

			m_end = end;
		}

		int NalUnit::payload(uint8_t *buf, int len)
		{
			/* Метод записывает полезную нагрузку nal unit
				в buf и возвращает количетво записанных байт */

				if(!isInit()) return -1;

				int i = 0;
				
				while(i < len && cur < m_end)
				{
					*buf++ = *cur++;
					 i++;
				}

				return i;
		}

		void NalUnit::init()
		{
			/* Инициализация всех параметров класса */


			/* Проверка выполнения предусловий.
			   Если предусловия не выполняется,
			   то метод завершает свою работу   */

				if(isInit()) return;
				if(startcode3(m_pos) == false && startcode4(m_pos) == false) return;

			/* Начало выполнения основного блока метода */
			
				startCode = m_pos;

				cur = m_pos;
				
				while(*cur++ != 1);

				m_pos = cur;

				header = *m_pos;

				cur = m_pos;
		}

		void NalUnit::clear()
		{
			/* Сброс всех значений параметра класса */

				header = 0;
				startCode = nullptr;
				m_pos = nullptr;
				m_end = nullptr;
		}

		bool NalUnit::isInit() const
		{
			/* Проверка корректной инициализации параметров класса */

				if(!Byte::Array::isInit()) return false;
				if(startCode == nullptr) return false;

				return true;
		}

		Packer::Packer() : Byte::Buffer()
		{
			/* Реализация конструктор класса по умолчанию */

				mode = UnknowMode;
		}

		int Packer::pack(AccessUnit & au)
		{
			/* Метод упаковывает все nal unit 
				которые содержит один access unit 
				в буфер */


			/* Проверка выполнения предусловий.
			   Если предусловия не выполняется,
			   то метод завершает свою работу */

				if(!isInit()) return -1;
				if(cur == m_end) return 0;

			/* Начало выполнения основного блока метода */

				uint32_t payloadSize = 0;
				uint8_t *naluPos, header, nri = 0;
				uint8_t  temp, fuaIndicator, fuaHeader, stapaHeader = 0;
				int ret, len, packetNumber, otherSize, n = 0, naluCount = 0;

				mode = (mode == UnknowMode) ? StapaMode : mode;

				if(mode == FuaMode)
				{
					/* Упаковка фрагмента nal unit */

						*cur++ = GetFuaIndicator(nu.getHeader());
						*cur++ = GetFuaHeader(nu.getHeader());

						if(nu.isReset())
						{
							*(cur - 1) = SetFuaStartBit(1, *(cur - 1));
							 ret = nu.payload(&header, 1);
						}

						ret = nu.payload(cur, getFree());

						if(nu.isComplete())
						{
							*(m_pos + 1) = SetFuaEndBit(1, *(m_pos + 1));
							 mode = UnknowMode;	
						}

						cur += ret;
						len = getLen();
						cur = m_pos;
						return len;
				}

				if(mode == SingleMode)
				{
					/* Упаковка одиночного nal unit */

						ret = nu.payload(m_pos, nu.getSize());
						cur += ret;
						mode = UnknowMode;
						len = getLen();
						cur = m_pos;
						return len;
				}

				if(mode == StapaMode)
				{
					/* Агрегированная упаковка acess unit типа STAP-A */

					if(au.nalUnitCount() == 0) return 0;

					while(au.nextNalUnit(nu).isInit())
					{
						// Обработка случая, когда nal unit является концом потока

							if(nu.getPayloadType() == 10)
							{
								*cur++ = nu.getHeader();
								 return 1;			
							}

						/* Обработка случая, когда размер nal unit
							либо больше размера самого буфера, либо
							больше, чем размер свободного места в
							буфере */

							if(nu.getSize() > getSize() || nu.getSize() > getFree())
							{
								mode = nu.getSize() > getSize() ? FuaMode : SingleMode;
							  *m_pos = SetReferenceIDC(nri, *m_pos);
								len = getLen();
								cur = m_pos;
								return len;
							}

						// Упаковка очередного nal unit

							payloadSize = nu.getSize();
							nri = nu.getReferenceIDC() > nri ? nu.getReferenceIDC() : nri;
							
							if(cur == m_pos)
							{
								/* Если отпарсенный nal unit является самым первым,
									то в начало буфера добавляется специальный заголовок */

									stapaHeader = GetStapaHeader(nu.getHeader());
									*cur++ = stapaHeader;
							}

						// Запись в буфер размера полезной нагрузки nal unit

							*cur++ = (uint8_t)((payloadSize >> 8) & 0x000000FF);
							*cur++ = (uint8_t)(payloadSize & 0x000000FF);

							ret = nu.payload(cur, getFree());
							cur += ret;
					}

					/* Обработка случая, когда все nal unit помещаются в буфер */

						*m_pos = SetReferenceIDC(nri, *m_pos);
						 len = getLen();
						 cur = m_pos;
						 mode = UnknowMode;
						 return len;
				}

			return 0;
		}

		int Unpacker::unpack(uint8_t *buf, int len)
		{
			/* Метод распаковывает упакованные nal unit в буфер
				и добавляет стартовые коды */

			if(!isInit()) return -1;
			if(buf == nullptr || len == 0) return -1;

			uint32_t payloadSize;
			uint8_t naluHeader = 0, payloadType;
			uint8_t  *temp, *last = cur, *end = buf + len;

			if(GetPayloadType(*buf) == 24)
			{
				buf++;
				while(buf < end)
				{
					payloadSize = 0;
					payloadSize = (payloadSize | *buf) << 8;
					buf++;
					payloadSize |= *buf;
					buf++;

					payloadType = GetPayloadType(*buf);
					if(payloadType == 9 || payloadType == 8 || payloadType == 7)
					{
						if(getFree() < payloadSize + 4) return 0;

						*cur++ = 0;
						*cur++ = 0;
						*cur++ = 0;
						*cur++ = 1;
					}
					else
					{
						if(getFree() < payloadSize + 3) return 0;

						*cur++ = 0;
						*cur++ = 0;
						*cur++ = 1;	
					}

					temp = buf + payloadSize;

					while(buf < temp) *cur++ = *buf++;
				}

				return cur - last;
			}

			if(GetPayloadType(*buf) == 28)
			{	
				naluHeader = SetForbiddenBit(GetForbiddenBit(*buf), naluHeader);
				naluHeader = SetReferenceIDC(GetReferenceIDC(*buf), naluHeader);

				buf++;

				if(GetFuaStartBit(*buf))
				{
					naluHeader = SetPayloadType(GetPayloadType(*buf), naluHeader);

					if(getFree() < end - (buf + 3)) return 0;

					*cur++ = 0;
					*cur++ = 0;
					*cur++ = 1;
					*cur++ = naluHeader;
				}

				buf++;

				if(getFree() < end - buf) return 0;

				while(buf < end) *cur++ = *buf++;

				return cur - last;
			}

			if(GetPayloadType(*buf) == 10)
			{
				if(getFree() < 5) return 0;

				*cur++ = 0;
				*cur++ = 0;
				*cur++ = 0;
				*cur++ = 1;
				*cur++ = *buf;

				return cur - last;					
			}

			if(getFree() < end - buf) return 0;

			while(buf < end) *cur++ = *buf++;

			return cur - last;
		}


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

	} // namespace H264
