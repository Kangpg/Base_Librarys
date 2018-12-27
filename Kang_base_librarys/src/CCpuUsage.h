#pragma once

#include <Windows.h>

class CCpuUsage
{
public:

	CCpuUsage(HANDLE process_handle = INVALID_HANDLE_VALUE);
	virtual ~CCpuUsage() = default;

	auto Update_CPU_time();

	auto Get_process_total_time() const { return m_process_total_time; }
	auto Get_process_user_time() const { return m_process_user_time; }
	auto Get_process_kernel_time() const { return m_process_kernel_time; }

	auto Get_processor_total_time() const { return m_processor_total_time; }
	auto Get_processor_user_time() const { return m_processor_user_time; }
	auto Get_processor_kernel_time() const { return m_processor_kernel_time; }

private:

	HANDLE m_process = INVALID_HANDLE_VALUE;

	unsigned m_processor_count = 0;

	float m_processor_total_time = 0;
	float m_processor_user_time = 0;
	float m_processor_kernel_time = 0;

	UINT64 m_processor_last_kernel_time = 0;
	UINT64 m_processor_last_user_time = 0;
	UINT64 m_processor_last_idle_time = 0;

	float m_process_total_time = 0;
	float m_process_user_time = 0;
	float m_process_kernel_time = 0;

	UINT64 m_process_last_kernel_time = 0;
	UINT64 m_process_last_user_time = 0;
	UINT64 m_process_last_time = 0;
};