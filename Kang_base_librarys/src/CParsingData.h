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

		static WCHAR g_ip[16];
		static WORD g_port;

		static WORD g_max_worker_threads;
		static DWORD g_max_connection;

		static bool g_is_nagle;

		static BYTE g_packet_unfixed_code_01;
		static BYTE g_packet_unfixed_code_02;
	};
}