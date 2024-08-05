#include "byte.hpp"

#ifndef H264_HPP
#define H264_HPP

	namespace H264 
	{
		/* Пространство имён модуля для работы с видео-потоком H264  */

		class NalUnit;
		class AccessUnit;

		class Stream : public Byte::Array
		{		
			/* Данный класс управляет доступом к информации 
				об access unit потока h264                   */

			public:

				/* Констркуторы и деструкторы класса */

		     		Stream() : Byte::Array() {}
					virtual ~Stream() {}

				/* Другие методы класса */

					int accessUnitCount();
					AccessUnit & nextAccessUnit(AccessUnit & au);
					AccessUnit & currentAccessUnit(AccessUnit & au);

		}; // class Stream


		class AccessUnit : public Byte::Array
		{
			public:

				/* Конструкторы и деструкторы класса */

					AccessUnit() : Byte::Array() {}
					virtual ~AccessUnit() {}

				/* Другие методы класса */

					int nalUnitCount();
					NalUnit & nextNalUnit(NalUnit & nu);
					NalUnit & currentNalUnit(NalUnit & nu);
			
		}; // class AccessUnit


		class NalUnit : public Byte::Array
		{
			private:

				uint8_t header;     // Заголовок nal unit
				uint8_t *startCode; // Указатель на начальную позиция стартового кода

			public:

			/* Конструкторы и деструкторы класса */

				NalUnit();
				virtual ~NalUnit() {}

			/* Селекторы класса */		

				uint8_t  getHeader() const;
				uint8_t *getStartCode() const;
				    int  getStartCodeLen() const;

				uint8_t getForbiddenBit() const;
				uint8_t getReferenceIDC() const;
				uint8_t getPayloadType()  const;

			/* Модификаторы класса */

				virtual void setPos(uint8_t *pos);
				virtual void setEnd(uint8_t *end);

			/* Другие методы класса */

				int payload(uint8_t *buf, int len);
				virtual void clear();

				void init();

			/* Чекеры класса */
	
				virtual bool isInit() const;
				
		}; // class NalUnit


		class Packer : public Byte::Buffer
		{
			/* Данный класс упаковывает nal unit потока h264
				в буфер используя один из режимов упаковки    */

			public:
				
				// Целочисленные идентификаторы режима упаковки 

					enum { UnknowMode, StapaMode, FuaMode, SingleMode };

			private:

				int mode;   // Текущий режим упаковки
				NalUnit nu; // Текущий обрабатываемый nal unit

			public:

				/* Конструкторы и деструкторы класса */

					Packer();
					virtual ~Packer() {}

				/* Другие методы класса */

					int pack(AccessUnit & au);

		}; // class Packer

		class Unpacker : public Byte::Buffer
		{
			public:

				/* Конструкторы и деструкторы класса */
			
					Unpacker() : Byte::Buffer() {}
					virtual ~Unpacker() {}

				/* Другие методы классса */

					int unpack(uint8_t *buf, int bufsize);
				
		}; // class Unpacker


	/* Внеклассовые функции для работы с заголовком nal unit */

		uint8_t GetForbiddenBit(uint8_t header);
		uint8_t GetReferenceIDC(uint8_t header);
		uint8_t GetPayloadType(uint8_t header);

		uint8_t SetForbiddenBit(uint8_t forbiddenBit, uint8_t header);
		uint8_t SetReferenceIDC(uint8_t referenceIDC, uint8_t header);
		uint8_t SetPayloadType(uint8_t payloadType, uint8_t header);

		uint8_t GetStapaHeader(uint8_t header);

		uint8_t GetFuaIndicator(uint8_t header);
		uint8_t GetFuaHeader(uint8_t header);
		uint8_t GetFuaStartBit(uint8_t fuaHeader);
		uint8_t GetFuaEndBit(uint8_t fuaHeader);

		uint8_t SetFuaStartBit(uint8_t startBit, uint8_t fuaHeader);
		uint8_t SetFuaEndBit(uint8_t endtBit, uint8_t fuaHeader);
		uint8_t ResetFuaBits(uint8_t fuaHeader);
		
	}; // namespace H264

#endif // H264_HPP
