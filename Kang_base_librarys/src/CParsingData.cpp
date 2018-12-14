#include "stdafx.h"
#include "CParsingData.h"

namespace My_Library
{
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

		assert(document.HasMember("packet_code"));

		const auto& code = document["packet_code"];
		assert(code.IsArray());

		g_packet_unfixed_code_01 = static_cast<BYTE>(code[0].GetInt());
		g_packet_unfixed_code_02 = static_cast<BYTE>(code[1].GetInt());
	}
}