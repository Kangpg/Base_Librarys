#pragma once

#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#include <WS2tcpip.h>
#include <stack>
#include <atomic>
#include <vector>
#include <thread>
#include <chrono>
#include "CStreamingQ.h"
#include "CPacket_serialization.h"
#include "CParsingData.h"

using namespace std;
using namespace chrono;

namespace My_Library
{
	enum class en_netserver_error_code : unsigned
	{
		CANNOT_MAKE_ACCEPT_THREAD,
		CANNOT_MAKE_WORKER_THREAD
	};

	class CNetServer
	{
	private:

		struct st_session
		{
			SOCKET session_socket;
			SOCKADDR_IN session_address;

			atomic<DWORD> session_io_count;
			atomic<bool> session_is_release;

			// unsigned long long union
			// high part = id
			// low part = index
			ULARGE_INTEGER session_unique_id;
			
			OVERLAPPED session_overlapped_send;
			OVERLAPPED session_overlapped_recv;

			CStreamingQ session_recv_queue;

			stack<CPacket_serialization*> session_sent_stack;
			SRWLOCK session_sent_stack_lock;
			unsigned session_sent_count;

			atomic<bool> session_is_sending;

			system_clock::time_point session_heart_beat;
		};

	public:

		CNetServer(const WCHAR* p_ip, WORD port, WORD max_worker_threads, DWORD max_connection, bool is_nagle);
		virtual ~CNetServer();

		auto Start();
		auto Stop();

	protected:

		virtual void OnClientJoin(unsigned session_id) = 0;
		virtual void OnClientLeave() = 0;
		virtual void OnRecv() = 0;
		virtual void OnSend() = 0;
		virtual void OnWorkerThreadBegin() = 0;
		virtual void OnWorkerThreadEnd() = 0;
		virtual void OnError(int error, WCHAR* p_msg) = 0;

		auto SendPacket(unsigned session_id, CPacket_serialization* pPacket);
		auto Disconnect(unsigned session_id);

	private:

		auto InitNetWork();
		auto InitThreads();

		void Accept_thread_function();
		void Worker_thread_function();

		auto GetNewSessionIndex();

		auto ReleaseSession(st_session* pSession);

		auto RecvProcess(st_session* pSession);
		auto SendProcess(st_session* pSession);

	private:

		// Server config
		WCHAR m_ip[16] = { 0 };
		WORD m_port = 0;

		WORD m_max_worker_threads = 0;
		DWORD m_max_connection = 0;

		bool m_is_nagle = true;
		////

		SOCKET m_listen_socket = INVALID_SOCKET;
		SOCKADDR_IN m_address = { 0 };

		vector<st_session> m_session_vector;

		stack<unsigned> m_session_index_stack;
		SRWLOCK m_session_index_stack_lock;

		HANDLE m_iocp = NULL;

		thread m_accept_thread;
		vector<thread> m_worker_thread_vector;

		atomic<unsigned> m_session_id_counter = 10;

	// Monitor member
	public:

		atomic<DWORD> m_session_total_accept_try_count = 0;

		atomic<DWORD> m_session_total_accept_recog_count = 0;
		atomic<DWORD> m_session_total_accept_recog_tps_count = 0;

		atomic<DWORD> m_session_total_connecting_count = 0;
	};
}
