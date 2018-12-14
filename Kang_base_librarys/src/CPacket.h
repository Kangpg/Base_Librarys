/////
// Only for my server library packet using my server's protocol.
// inherit CPacket_serialization class.
/////

#pragma once

#include "CPacket_serialization.h"
#include "CParsingData.h"

namespace My_Library
{
	class CPacket : public CPacket_serialization
	{
	private:

		enum class en_config : unsigned
		{
			DEFAULT_PACKET_SIZE = 1024,
			DEFAULT_HEADER_SIZE = 0
		};

	public:

		CPacket(unsigned packet_header_size = static_cast<unsigned>(en_config::DEFAULT_HEADER_SIZE),
			unsigned packet_buffer_size = static_cast<unsigned>(en_config::DEFAULT_PACKET_SIZE));
		~CPacket() = default;

	private:

		BYTE m_packet_fixed_code = 119;
		BYTE m_packet_unfixed_code_01 = CParsingData::g_packet_unfixed_code_01;
		BYTE m_packet_unfixed_code_02 = CParsingData::g_packet_unfixed_code_02;
	};
}