#include <cstdint>

#ifndef H264_HPP
#define H264_HPP

	uint8_t H264GetForbiddenBit(uint8_t header);
	uint8_t H264GetReferenceIDC(uint8_t header);
	uint8_t H264GetPayloadType(uint8_t header);

	uint8_t H264SetForbiddenBit(uint8_t forbiddenBit, uint8_t header);
	uint8_t H264SetReferenceIDC(uint8_t referenceIDC, uint8_t header);
	uint8_t H264SetPayloadType(uint8_t payloadType, uint8_t header);

	uint8_t H264GetStapaHeader(uint8_t header);

	uint8_t H264GetFuaIndicator(uint8_t header);
	uint8_t H264GetFuaHeader(uint8_t header);
	uint8_t H264GetFuaStartBit(uint8_t fuaHeader);
	uint8_t H264GetFuaEndBit(uint8_t fuaHeader);

	uint8_t H264SetFuaStartBit(uint8_t startBit, uint8_t fuaHeader);
	uint8_t H264SetFuaEndBit(uint8_t endtBit, uint8_t fuaHeader);
	uint8_t H264ResetFuaBits(uint8_t fuaHeader);

	class H264NalUnit
	{
		private:

			uint8_t header;       // Заголовок nal unit
			uint8_t *m_pos; 		 // Начальная позиция буфера с nal unit, в котором хранится поток h264
			uint8_t *m_end; 		 // Конечная позиция буфера с nal unit, в котром хранится поток h264
			uint8_t *m_startCode; // Указатель на начальную позиция стартового кода

		public:

			/* Конструкторы и деструкторы класса */

				H264NalUnit();
				virtual ~H264NalUnit() {}

			/* Селекторы класса */

				    int  getSize()         const { return m_end - m_pos; 		}
				uint8_t *getPos()          const { return m_pos;         		}
				uint8_t *getEnd()          const { return m_end;         	   }
				uint8_t *getStartCode()    const { return m_startCode;   		}
				    int  getStartCodeLen() const { return m_pos - m_startCode; }
				uint8_t  getHeader()       const { return *m_pos;              }

				uint8_t getForbiddenBit() const;
				uint8_t getReferenceIDC() const;
				uint8_t getPayloadType()  const;


			/* Модификаторы класса */

				void setPos(uint8_t *pos)             { m_pos = pos;             }
				void setEnd(uint8_t *end)             { m_end = end;             }
				void setStartCode(uint8_t *startCode) { m_startCode = startCode; }

			/* Другие методы класса */

				int payload(char *buf, int bufsize);

				void init();
				void clear();
	
	}; // class H264NalUnit


	class H264AccessUnit
	{
		private:

			uint8_t *m_pos; // Начальная позиция буфера с access unit, в котором хранится поток h264
			uint8_t *cur;   // Текущая позиция буфера с access unit, в котором хранится поток h264
			uint8_t *m_end; // Конечная позиция буфера с access unit, в котром хранится поток h264

			int naluCount; // Количество nal unit в одном access unit
		
		public:

			/* Конструкторы и деструкторы класса */

				H264AccessUnit();
				virtual ~H264AccessUnit() {}
		
			/* Селекторы класса */

				uint8_t *getPos()       const { return m_pos;         }
				uint8_t *getEnd()       const { return m_end;         }
				    int  getSize()      const { return m_end - m_pos; }
				    int  getNaluCount() const { return naluCount;     }

			/* Модификаторы класса */

				void setPos(uint8_t *pos) { m_pos = pos; }
				void setEnd(uint8_t *end) { m_end = end; }

			/* Другие методы класса */

				void parseNalUnit(H264NalUnit & nu);

				void init();
				void reset();
				void clear();

	}; // class H264AccessUnit


	class H264Parser
	{
		private:

			uint8_t *cur; // Текущая позиция буфера, в котором хранится поток H264

			int auCount; // Количество access unit в буфере

		public:

			/* Конструкторы и деструкторы класса */

				H264Parser();
				virtual ~H264Parser() {}

			/* Селекторы класса */

				int getAUCount() const { return auCount; }

			/* Другие методы класса */

				int loadFile(const char *filename);
				void parseAccessUnit(H264AccessUnit & au);

				void init();
				void reset();
				void clear();
		
	}; // class H264Parser


	class H264Packer
	{
		public:
			
			enum { UnknowMode, StapaMode, FuaMode, SingleMode };

		private:

			int mode; // Текущий режим упаковки

			uint8_t *m_pos; // Начальная позиция буфера для упаковки nal unit
			uint8_t *m_cur; // Текущая позиция буфера для упаковки nal unit
			uint8_t *m_end; // Конечная позиция буфера для упаковки nal unit

			uint8_t *au_pos; // Начальная позиция буфера с access unit
			uint8_t *au_cur; // Текущая позиция буфера с access unit
			uint8_t *au_end; // Конечная позиция буфера с access unit

		public:

			// Констуркторы  и деструкторы класса

				H264Packer();
				virtual ~H264Packer() {}

			// Селекторы класса
 
				int getBufLen()  const { return m_cur - m_pos; }
				int getBufSize() const { return m_end - m_pos; }
				int getMode()    const { return mode;          }

			// Модификаторы класса

				void setPos(uint8_t *pos);
				void setEnd(uint8_t *end);
				void setBuf(uint8_t *buf, int bufsize);

			// Другие методы класса

				int pack(H264Parser & parser);

				void init();
				void reset();
				void clear();

	}; // class H264Packer


	class H264Unpacker
	{
		private:

			uint8_t *m_pos; // Начальная позиция буфера для сохранения распакованного nal unit
			uint8_t *m_cur; // Текущая позиция буфера для сохранения распакованного nal unit
			uint8_t *m_end; // Конечная позиция буфера для сохранения распакованного nal unit

		public:

			// Констуркторы  и деструкторы класса

				H264Packer();
				virtual ~H264Packer() {}
			
			// Селекторы класса
			
				

			// Модификаторы класса

				void setPos(uint8_t *pos);
				void setEnd(uint8_t *end);
				void setBuf(uint8_t *buf, int bufsize);

			// Другие методы класса

				int unpack(uint8_t *buf, int bufsize);

				void init();
				void reset();
				void clear();

	}; // class H264Unpacker

#endif // H264_HPP
