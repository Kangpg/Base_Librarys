#include "stdafx.h"
#include "CParsingData.h"

namespace My_Library
{
	WCHAR CParsingData::g_ip[16] = { 0 };
	WORD CParsingData::g_port = 0;

	WORD CParsingData::g_max_worker_threads = 0;
	DWORD CParsingData::g_max_connection = 0;

	bool CParsingData::g_is_nagle = true;

	BYTE CParsingData::g_packet_unfixed_code_01 = 0;
	BYTE CParsingData::g_packet_unfixed_code_02 = 0;

	CParsingData::CParsingData()
	{
		FILE* pStream = nullptr;
		fopen_s(&pStream, "config.json", "rb");

		char readBuffer[65536] = { 0 };

		FileReadStream rs(pStream, readBuffer, sizeof(readBuffer));

		Document document;
		document.ParseStream(rs);

		fclose(pStream);

		assert(document.HasMember("server_ip"));
		const auto& ip = document["server_ip"];

		memcpy(g_ip, ip.GetString(), ip.GetStringLength());

		assert(document.HasMember("server_port"));
		const auto& port = document["server_port"];

		g_port = static_cast<WORD>(port.GetInt());

		assert(document.HasMember("server_max_worker_threads"));
		const auto& max_worker_threads = document["server_max_worker_threads"];

		g_max_worker_threads = static_cast<WORD>(max_worker_threads.GetInt());

		assert(document.HasMember("server_max_connection"));
		const auto& max_connection = document["server_max_connection"];

		g_max_connection = static_cast<DWORD>(max_connection.GetInt());

		assert(document.HasMember("server_is_nagle"));
		const auto& is_nagle = document["server_is_nagle"];

		g_is_nagle = is_nagle.GetBool();

		assert(document.HasMember("packet_code"));
		const auto& code = document["packet_code"];
		assert(code.IsArray());

		g_packet_unfixed_code_01 = static_cast<BYTE>(code[0].GetInt());
		g_packet_unfixed_code_02 = static_cast<BYTE>(code[1].GetInt());
	}
}