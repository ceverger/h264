#include <stdint.h>

#ifndef H264_PACKER_HPP
#define H264_PACKER_HPP

	class H264Packer
	{
		/* Класс предназначен для формирования одного пакета nal unit из потока h264 
			для инкапсуляции его в rtp пакет и дальнейшей транспортировке по сети */

		public:

			enum 
			{
				/* Целочисленные идентификаторы режима упаковки nal unit */ 

					UnknowMode, // Неизвестный режим упаковки
					StapaMode,  // Агрегированный режим упаковки
					SingleMode, // Одиночный режим упаковки
					FuaMode     // Фрагментный режим упаковки
			};

		private:

			int mode;       // Режим упаковки nal unit
			int m_maxSize;  // Максимальная длина пакета с nal unit;

			int naluHeader;   // Заголовок nal unit
			int fuaIndicator; // Индикатор фрагмента nal unit в случае fua упаковки
			int fuaHeader;    // Заголовок фрагмента nal unit в случае fua упаковки

			uint8_t *pos; // Указатель на начальную позицию буфера с nal unit
			uint8_t *cur; // Указатель на текущую позицию буфера с nal unit
			uint8_t *end; // Указатель на конечную позицию буфера с nal unit

		public:

			/* Конструкторы и деструкторы класса */

				H264Packer();
		  		virtual ~H264Packer() {}

			/* Модификаторы класса */

				void getMode() const;
				void getMaxSize() const;
				void getPayloadLen() const;

			/* Модификторы класса */

				void setMaxSize(int maxSize);
				void setNalu(uint8_t *nalu, int size);

			/* Другие методы класса */

				int pack(uint8_t *buf, int size);

				void reset();

	}; // class H264Packer

#endif // H264_PACKER_HPP 
