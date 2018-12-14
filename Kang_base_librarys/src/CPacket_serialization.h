#pragma once

#include <Windows.h>
#include <iostream>

using namespace std;

namespace My_Library
{
	typedef enum class packet_en_exception : unsigned
	{
		// STREAMING QUEUE
		PACKET_BUFFER_WAS_NULL,
		PACKET_BUFFER_WAS_NOT_NULL

	}PACKET_EXCEPTION;

	class CPacket_serialization
	{
	public:

		CPacket_serialization(unsigned packet_header_size, unsigned packet_buffer_size);
		virtual ~CPacket_serialization();

		// 사용하고 있는 데이터 공간
		unsigned GetDataUsedSize() const;
		// 사용할 수 있는 데이터 공간
		unsigned GetDataUsableSize() const;
		// 사용하고 있는 헤더 공간
		unsigned GetHeaderUsedSize() const;
		// 사용할 수 있는 헤더 공간
		unsigned GetHeaderUsableSize() const;
		// 패킷의 총 사용 공간
		unsigned GetTotalPacketUsageSize() const;

	private:

		char* m_pBuffer = nullptr;

		char* m_pHeader = nullptr;
		char* m_pData = nullptr;

		// 패킷 Header의 데이터 인덱스
		unsigned m_packet_header_front = 0;
		unsigned m_packet_header_rear = 0;

		// 패킷 Body의 데이터 인덱스
		unsigned m_packet_data_front = 0;
		unsigned m_packet_data_rear = 0;

		// 패킷의 총 사용 사이즈
		unsigned m_packet_total_usage_size = 0;

		// 패킷의 총 공간
		unsigned m_packet_header_size = 0;
		unsigned m_packet_buffer_size = 0;

	public:

		template <class T>
		unsigned PushData(T& pResource, unsigned resource_size)
		{
			if (m_pBuffer == nullptr)
				throw PACKET_EXCEPTION::PACKET_BUFFER_WAS_NULL;

			if (GetDataUsableSize() < resource_size)
				return false;

			memcpy(m_pData + m_packet_data_rear, (char*)&pResource, static_cast<size_t>(resource_size));

			m_packet_data_rear += resource_size;
			m_packet_total_usage_size += resource_size;

			return resource_size;
		}

		template <class T>
		unsigned PopData(T& pDest, unsigned resource_size)
		{
			if (m_pBuffer == nullptr)
				throw PACKET_EXCEPTION::PACKET_BUFFER_WAS_NULL;

			if (GetDataUsedSize() < resource_size)
				return false;

			memcpy((char*)&pDest, m_pData + m_packet_data_front, static_cast<size_t>(resource_size));

			m_packet_data_front += resource_size;
			m_packet_total_usage_size -= resource_size;

			return resource_size;
		}

		template <class T>
		unsigned PushHeader(T& pResource, unsigned resource_size)
		{
			if (m_pBuffer == nullptr)
				throw PACKET_EXCEPTION::PACKET_BUFFER_WAS_NULL;

			if (GetHeaderUsableSize() < resource_size)
				return false;

			memcpy(m_pHeader + m_packet_header_rear, (char*)&pResource, static_cast<size_t>(resource_size));

			m_packet_header_rear += resource_size;
			m_packet_total_usage_size += resource_size;

			return resource_size;
		}

		template <class T>
		unsigned PopHeader(T& pDest, unsigned resource_size)
		{
			if (m_pBuffer == nullptr)
				throw PACKET_EXCEPTION::PACKET_BUFFER_WAS_NULL;

			if (GetHeaderUsedSize() < resource_size)
				return false;

			memcpy((char*)&pDest, m_pHeader + m_packet_header_front, static_cast<size_t>(resource_size));

			m_packet_header_front += resource_size;
			m_packet_total_usage_size -= resource_size;

			return resource_size;
		}

		// 대입 operator
		CPacket_serialization& operator = (const CPacket_serialization& source_CPacket);

		// Push Data Operator
		CPacket_serialization& operator << (const char& value) { PushData(value, sizeof(char)); return *this; }
		CPacket_serialization& operator << (const int& value) { PushData(value, sizeof(int)); return *this; }
		CPacket_serialization& operator << (const double& value) { PushData(value, sizeof(double)); return *this; }
		CPacket_serialization& operator << (const float& value) { PushData(value, sizeof(float)); return *this; }
		CPacket_serialization& operator << (const short& value) { PushData(value, sizeof(short)); return *this; }

		CPacket_serialization& operator << (const BYTE& value) { PushData(value, sizeof(BYTE)); return *this; }
		CPacket_serialization& operator << (const WORD& value) { PushData(value, sizeof(WORD)); return *this; }
		CPacket_serialization& operator << (const DWORD& value) { PushData(value, sizeof(DWORD)); return *this; }
		CPacket_serialization& operator << (const UINT& value) { PushData(value, sizeof(UINT)); return *this; }
		CPacket_serialization& operator << (const UINT64& value) { PushData(value, sizeof(UINT64)); return *this; }
		CPacket_serialization& operator << (const INT64& value) { PushData(value, sizeof(INT64)); return *this; }

		// Pop Data Operator
		CPacket_serialization& operator >> (const char& value) { PopData(value, sizeof(char)); return *this; }
		CPacket_serialization& operator >> (const int& value) { PopData(value, sizeof(int)); return *this; }
		CPacket_serialization& operator >> (const double& value) { PopData(value, sizeof(double)); return *this; }
		CPacket_serialization& operator >> (const float& value) { PopData(value, sizeof(float)); return *this; }
		CPacket_serialization& operator >> (const short& value) { PopData(value, sizeof(short)); return *this; }

		CPacket_serialization& operator >> (const BYTE& value) { PopData(value, sizeof(BYTE)); return *this; }
		CPacket_serialization& operator >> (const WORD& value) { PopData(value, sizeof(WORD)); return *this; }
		CPacket_serialization& operator >> (const DWORD& value) { PopData(value, sizeof(DWORD)); return *this; }
		CPacket_serialization& operator >> (const UINT& value) { PopData(value, sizeof(UINT)); return *this; }
		CPacket_serialization& operator >> (const UINT64& value) { PopData(value, sizeof(UINT64)); return *this; }
		CPacket_serialization& operator >> (const INT64& value) { PopData(value, sizeof(INT64)); return *this; }
	};
}