#include "stdafx.h"
#include "CPacket_serialization.h"

namespace My_Library
{
	CPacket_serialization::CPacket_serialization(unsigned packet_header_size, unsigned packet_buffer_size)
	{
		m_packet_buffer_size = packet_buffer_size;
		m_packet_header_size = packet_header_size;

		if (m_pBuffer != nullptr)
			throw PACKET_EXCEPTION::PACKET_BUFFER_WAS_NOT_NULL;

		m_pBuffer = new char[packet_buffer_size];
		memset(m_pBuffer, 0, packet_buffer_size);

		m_pHeader = m_pBuffer;
		m_pData = m_pBuffer + m_packet_header_size;
	}

	CPacket_serialization::~CPacket_serialization()
	{
		if (m_pBuffer != nullptr)
			delete[] m_pBuffer;
	}

	unsigned CPacket_serialization::GetDataUsedSize() const
	{
		return m_packet_data_rear - m_packet_data_front;
	}

	unsigned CPacket_serialization::GetDataUsableSize() const
	{
		return m_packet_buffer_size - m_packet_header_size - m_packet_data_rear;
	}

	unsigned CPacket_serialization::GetHeaderUsedSize() const
	{
		return m_packet_header_rear - m_packet_header_front;
	}

	unsigned CPacket_serialization::GetHeaderUsableSize() const
	{
		return m_packet_header_size - m_packet_header_rear;
	}

	unsigned CPacket_serialization::GetTotalPacketUsageSize() const
	{
		return m_packet_total_usage_size;
	}

	CPacket_serialization& CPacket_serialization::operator=(const CPacket_serialization& source_CPacket)
	{
		m_packet_total_usage_size = source_CPacket.m_packet_total_usage_size;

		m_packet_data_front = source_CPacket.m_packet_data_front;
		m_packet_data_rear = source_CPacket.m_packet_data_rear;

		m_packet_header_front = source_CPacket.m_packet_header_front;
		m_packet_header_rear = source_CPacket.m_packet_header_rear;

		memcpy(m_pBuffer, source_CPacket.m_pBuffer, m_packet_buffer_size);

		return *this;
	}
}