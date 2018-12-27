#include "stdafx.h"
#include "CCpuUsage.h"

CCpuUsage::CCpuUsage(HANDLE process_handle)
	: m_process(process_handle)
{
	if (m_process == INVALID_HANDLE_VALUE)
		m_process = GetCurrentProcess();

	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);

	m_processor_count = sysinfo.dwNumberOfProcessors;
}

auto CCpuUsage::Update_CPU_time()
{
	UINT64 idle = 0;
	UINT64 kernel = 0;
	UINT64 user = 0;

	if (!GetSystemTimes((PFILETIME)&idle, (PFILETIME)&kernel, (PFILETIME)&user))
		return;

	auto kernel_dift = kernel - m_processor_last_kernel_time;
	auto user_dift = user - m_processor_last_user_time;
	auto idle_dift = idle - m_processor_last_idle_time;

	auto total = kernel_dift + user_dift;
	if (total == 0)
	{
		m_processor_user_time = 0.0f;
		m_processor_kernel_time = 0.0f;
		m_processor_total_time = 0.0f;
	}
	else
	{
		m_processor_user_time = (float)((double)user_dift / total * 100.0f);
		m_processor_kernel_time = (float)((double)(total - idle_dift) / total * 100.0f);;
		m_processor_total_time = (float)((double)(kernel_dift - idle_dift) / total * 100.0f);;
	}

	m_processor_last_user_time = user;
	m_processor_last_kernel_time = kernel;
	m_processor_last_idle_time = idle;

	UINT64 nowtime = 0;
	GetSystemTimeAsFileTime((LPFILETIME)&nowtime);

	UINT64 none = 0;
	GetProcessTimes(m_process, (LPFILETIME)&none, (LPFILETIME)&none, (LPFILETIME)&kernel, (LPFILETIME)&user);

	auto time_dift = nowtime - m_process_last_time;
	user_dift = user - m_process_last_user_time;
	kernel_dift = kernel - m_process_last_kernel_time;

	total = kernel_dift + user_dift;

	m_process_total_time = (float)(total / (double)m_processor_count / (double)time_dift * 100.0f);
	m_process_kernel_time = (float)(kernel_dift / (double)m_processor_count / (double)time_dift * 100.0f);
	m_process_user_time = (float)(user_dift / (double)m_processor_count / (double)time_dift * 100.0f);

	m_process_last_time = nowtime;
	m_process_last_kernel_time = kernel;
	m_process_last_user_time = user;
}