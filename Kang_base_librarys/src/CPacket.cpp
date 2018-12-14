#include "stdafx.h"
#include "CPacket.h"

namespace My_Library
{
	CPacket::CPacket(unsigned packet_header_size, unsigned packet_buffer_size)
		: CPacket_serialization(packet_header_size, packet_buffer_size)
	{

	}
}