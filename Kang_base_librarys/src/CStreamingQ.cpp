#include "stdafx.h"
#include "CStreamingQ.h"

namespace My_Library
{
	CStreamingQ::CStreamingQ(unsigned buffer_size)
	{
		m_buffer_size = buffer_size;

		if (m_pBuffer != nullptr)
			throw STREAMINGQ_EXCEPTION::MAIN_QUEUE_BUFFER_WAS_NOT_NULL;

		m_pBuffer = new char[m_buffer_size];
		memset(m_pBuffer, 0, m_buffer_size);
	}

	CStreamingQ::~CStreamingQ()
	{
		if (m_pBuffer != nullptr)
			delete[] m_pBuffer;
	}

	unsigned CStreamingQ::GetBufferSize() const
	{
		return m_buffer_size;
	}

	unsigned CStreamingQ::GetUsableSize() const
	{
		return m_buffer_size - GetUsedSize() - 1;
	}

	unsigned CStreamingQ::GetUsedSize() const
	{
		if (m_rear >= m_front)
			return m_rear - m_front;
		else
			return (m_buffer_size - m_front) + m_rear;
	}

	unsigned CStreamingQ::GetNotBrokenEnQueueSize() const
	{
		if (IsFull())
			return 0;

		if (m_rear <= ((m_front + m_buffer_size - 1) % m_buffer_size))
			return ((m_front + m_buffer_size - 1) % m_buffer_size) - m_rear;

		return m_buffer_size - m_rear;
	}

	unsigned CStreamingQ::GetNotBrokenDeQueueSize() const
	{
		if (m_rear >= m_front)
			return m_rear - m_front;

		return m_buffer_size - m_front;
	}

	char* CStreamingQ::GetBufferPointer() const
	{
		if (m_pBuffer == nullptr)
			throw STREAMINGQ_EXCEPTION::MAIN_QUEUE_BUFFER_WAS_NULL;

		return m_pBuffer;
	}

	char* CStreamingQ::GetFrontPorinter() const
	{
		if (m_pBuffer == nullptr)
			throw STREAMINGQ_EXCEPTION::MAIN_QUEUE_BUFFER_WAS_NULL;

		return &m_pBuffer[m_front];
	}

	char* CStreamingQ::GetRearPointer() const
	{
		if (m_pBuffer == nullptr)
			throw STREAMINGQ_EXCEPTION::MAIN_QUEUE_BUFFER_WAS_NULL;

		return &m_pBuffer[m_rear];
	}

	unsigned CStreamingQ::MoveFront(size_t size)
	{
		m_front = (m_front + size) % m_buffer_size;

		return static_cast<unsigned>(size);
	}

	unsigned CStreamingQ::MoveRear(size_t size)
	{
		m_rear = (m_rear + size) % m_buffer_size;

		return static_cast<unsigned>(size);
	}

	bool CStreamingQ::IsEmpty() const
	{
		if (m_front == m_rear)
			return true;

		return false;
	}

	bool CStreamingQ::IsFull() const
	{
		if ((m_rear + 1) % m_buffer_size == m_front)
			return true;

		return false;
	}

	void CStreamingQ::Clear()
	{
		if (m_pBuffer == nullptr)
			throw STREAMINGQ_EXCEPTION::MAIN_QUEUE_BUFFER_WAS_NULL;

		memset(m_pBuffer, 0, m_buffer_size);

		m_front = 0;
		m_rear = 0;
	}
}