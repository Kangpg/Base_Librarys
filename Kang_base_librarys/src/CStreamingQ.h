#pragma once

#include <Windows.h>
#include <iostream>
#include <memory>

using namespace std;

namespace My_Library
{
	typedef enum class streamingQ_en_exception : unsigned
	{
		// STREAMING QUEUE
		MAIN_QUEUE_BUFFER_WAS_NULL,
		MAIN_QUEUE_BUFFER_WAS_NOT_NULL

	}STREAMINGQ_EXCEPTION;

	class CStreamingQ
	{
	protected:

		enum class en_config : unsigned
		{
			DEFAULT_BUFFER_SIZE = 8096
		};

	public:

		CStreamingQ(unsigned buffer_size = static_cast<unsigned>(en_config::DEFAULT_BUFFER_SIZE));
		virtual ~CStreamingQ();

		unsigned GetBufferSize() const;

		unsigned GetUsableSize() const;
		unsigned GetUsedSize() const;

		unsigned GetNotBrokenEnQueueSize() const;
		unsigned GetNotBrokenDeQueueSize() const;

		char* GetBufferPointer() const;
		char* GetFrontPorinter() const;
		char* GetRearPointer() const;

		unsigned MoveFront(size_t size);
		unsigned MoveRear(size_t size);

		bool IsEmpty() const;
		bool IsFull() const;

		void Clear();

	private:

		char* m_pBuffer = nullptr;

		unsigned m_front = 0;
		unsigned m_rear = 0;

		unsigned m_buffer_size = 0;

	public:

		template <class T>
		auto EnQueue(T* source, unsigned size)
		{
			if (m_pBuffer == nullptr)
				throw STREAMINGQ_EXCEPTION::MAIN_QUEUE_BUFFER_WAS_NULL;

			if (IsFull())
				return static_cast<unsigned>(0);

			if (GetUsableSize() <= size)
				size = GetUsableSize();

			if (GetNotBrokenEnQueueSize() >= size)
				memcpy(m_pBuffer + m_rear, reinterpret_cast<char*>(source), static_cast<size_t>(size));
			else
			{
				auto resize = GetNotBrokenEnQueueSize();
				memcpy(m_pBuffer + m_rear, reinterpret_cast<char*>(source), static_cast<size_t>(resize));
				memcpy(m_pBuffer, source + resize, static_cast<size_t>(size - resize));
			}

			m_rear = (m_rear + size) % m_buffer_size;

			return size;
		}

		template <class T>
		auto DeQueue(T* pDest, unsigned size)
		{
			if (m_pBuffer == nullptr)
				throw STREAMINGQ_EXCEPTION::MAIN_QUEUE_BUFFER_WAS_NULL;

			if (IsEmpty())
				return static_cast<unsigned>(0);

			if (GetUsedSize() <= size)
				size = GetUsedSize();

			if (GetNotBrokenDeQueueSize() >= size)
				memcpy(reinterpret_cast<char*>(pDest), m_pBuffer + m_front, static_cast<size_t>(size));
			else
			{
				auto resize = GetNotBrokenDeQueueSize();
				memcpy(reinterpret_cast<char*>(pDest), m_pBuffer + m_front, static_cast<size_t>(resize));
				memcpy(reinterpret_cast<char*>(pDest + resize), m_pBuffer, static_cast<size_t>(size - resize));
			}

			m_front = (m_front + size) % m_buffer_size;

			return size;
		}

		template <class T>
		auto Peek(T* pDest, unsigned size)
		{
			if (m_pBuffer == nullptr)
				throw STREAMINGQ_EXCEPTION::MAIN_QUEUE_BUFFER_WAS_NULL;

			if (IsEmpty())
				return static_cast<unsigned>(0);

			if (GetUsedSize() <= size)
				size = GetUsedSize();

			if (GetNotBrokenDeQueueSize() >= size)
				memcpy(reinterpret_cast<char*>(pDest), m_pBuffer + m_front, static_cast<size_t>(size));
			else
			{
				auto resize = GetNotBrokenDeQueueSize();
				memcpy(reinterpret_cast<char*>(pDest), m_pBuffer + m_front, static_cast<size_t>(resize));
				memcpy(reinterpret_cast<char*>(pDest + resize), m_pBuffer, static_cast<size_t>(size - resize));
			}

			return size;
		}
	};
}