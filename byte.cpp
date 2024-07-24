#include "byte.hpp"

	namespace Byte 
	{
		/* Реализация методов классов модуля BYTE */

			/* Реализация методов класса Array */
			
				Array::Array()
				{
					/* Конструктор по умолчанию */

					m_pos = nullptr;
					  cur = nullptr;
					m_end = nullptr;
				}

				Array::Array(uint8_t *buf, int bufsize)
				{
					/* Пользовательский конструктор класса */

					m_pos = buf;
					  cur = m_pos;
					m_end = m_pos + bufsize;				
				}

				Array::Array(const Array & item)
				{
					/* Конструктор копирования */
					
					m_pos = item.m_pos;
					  cur = item.cur;
					m_end = item.m_end;
				}

				Array & Array::operator=(const Array & item)
				{
					/* Конструтор присваивания */

					if(isInit()) return *this;

					m_pos = item.m_pos;
					  cur = item.cur;
					m_end = item.m_end;

					return *this;
				}

				int Array::getSize() const
				{
					/* Получение размера массива байтов */

					if(!isInit()) return 0;

					return m_end - m_pos;
				}

				Array & Array::getItem(Array & item) const
				{
					/* Копирование данных в другой объект */

					item.m_pos = m_pos;
					item.cur = cur;
					item.m_end = m_end;

					return item;
				}

				void Array::setPos(uint8_t *pos)
				{
					/* Установка начальной и текущей позиции массива байт */

					if(isInit()) return;

					m_pos = pos;
					cur = m_pos;
				}

				void Array::setEnd(uint8_t *end)
				{
					/* Установка конечной позиции массива байт */

					if(isInit()) return;

					m_end = end;					
				}

				void Array::setBuf(uint8_t *buf, int bufsize)
				{
					/* Установка начальной и конечной позиции массива байт */

					if(isInit()) return;

					m_pos = buf;
					  cur = m_pos;
					m_end = m_pos + bufsize;	
				}

				void Array::setItem(const Array & item)
				{
					/* Копирование данных из другого объекта */

					if(isInit()) return;

					m_pos = item.m_pos;
					  cur = item.cur;
					m_end = item.m_end;
				}

				void Array::reset()
				{
					/* Сброс текущей позиции в массиве байт */
				
					cur = m_pos;
				}

				void Array::clear()
				{
					/* Очистка данных объекта */

					m_pos = nullptr;
					  cur = nullptr;
					m_end = nullptr;
				}

				bool Array::isInit() const
				{
					return m_pos == nullptr || m_end == nullptr ? false : true;
				}


			/* Реализация методов класса Buffer */

				int Buffer::getLen() const
				{
					/* Метод возвращает количество записанных
						в буфер данных (валидных байт)         */

					if(!isInit()) return -1;

					return cur - m_pos;
				}

				int Buffer::getFree() const
				{
					/* Метод возвращает количество свободного
						места в буфере                         */

					if(!isInit()) return -1;

					return m_end - cur;
				}

				int Buffer::getValue(int index) const
				{
					/* Метод возвращает значение элемента
						буфера по его индексу              */

					if(!isInit()) return -1;

					return index < getLen() ? m_pos[index] : -1;	
				}

				int Buffer::getData(uint8_t *buf, int bufsize) const
				{
					/* Метод копирует данные в другой буфер 
						и возвращет количество скопированных байт */

					if(!isInit()) return -1;

					int i = 0;

					while(i < bufsize && i < getSize())
					{
						buf[i] = m_pos[i];
						i++;
					}

					return i;
				}

				Buffer & Buffer::getData(Buffer & item) const
				{
					/* Метод инициализирует объект item
						значениями данных текущего объекта */

					if(item.isInit()) return item;

					item.m_pos = m_pos;
					item.cur = cur;
					item.m_end = m_end;

					return item;
				}

				void Buffer::setValue(int index, uint8_t value)
				{
					/* Метод инициализирует один из элементов буфера */

					if(!isInit()) return;
					if(index > getLen()) return;

					m_pos[index] = value;			
				}

				void Buffer::setData(uint8_t *buf, int bufsize)
				{
					/* Метод копирует данные в буфер текущего объекта */

					if(!isInit()) return;

					cur = m_pos;

					int i = 0;

					while(i < bufsize && cur < m_end)
					{
						*cur++ = *buf++;
						 i++;
					}
				}

				void Buffer::setData(const Buffer & item)
				{
					/* Метод копирует данные в буфер из другого объекта */

					if(isInit()) return;

					m_pos = item.m_pos;
					cur = item.cur;
					m_end = item.m_end;
				}

				int Buffer::addData(uint8_t value)
				{
					/* Метод добавляет новый валидный элемент
						в конец буфера                         */

					if(!isInit()) return -1;
					if(getFree() <= 0) return 0;

					*cur++ = value;

					return 1;
				}

			   int Buffer::addData(uint8_t *buf, int len)
				{
					/* Добавление новых данных в конец буфера */

					if(!isInit()) return -1;
					if(getFree() <= 0) return 0;

					int i = 0;

					while(i < len && cur < m_end)
					{
						*cur++ = *buf++;
						 i++;						
					}
					
					return i;
				}

				int Buffer::addData(const Buffer & item)
				{
					/* Копирование новых данных в конец буфера 
						из другого объекта                      */
	
					if(!isInit()) return -1;
					if(!item.isInit()) return -1;
					if(getFree() <= 0) return 0;

					int i = 0;
					uint8_t *buf = item.m_pos;

					while(i < item.getLen() && cur < m_end)
					{
						*cur++ = *buf++;
						 i++;
					}
					
					return i;												
				}

	}; // namespace Byte 
