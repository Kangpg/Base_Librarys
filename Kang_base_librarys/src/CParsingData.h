#pragma once

#include <Windows.h>
#include "document.h"
#include "filereadstream.h"
#include "CommonProtocol.h"

using namespace rapidjson;

namespace My_Library
{
	class CParsingData
	{
	public:

		CParsingData();
		virtual ~CParsingData() = default;

	public:

		static BYTE g_packet_unfixed_code_01;
		static BYTE g_packet_unfixed_code_02;
	};
}