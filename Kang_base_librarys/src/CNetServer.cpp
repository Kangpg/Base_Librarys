#include "stdafx.h"
#include "CNetServer.h"

namespace My_Library
{
	CNetServer::CNetServer(const WCHAR* p_ip, WORD port, WORD max_worker_threads, DWORD max_connection, bool is_nagle)
		: m_port(port), m_max_worker_threads(max_worker_threads),m_max_connection(max_connection), m_is_nagle(is_nagle),
		m_session_vector(max_connection), m_worker_thread_vector(max_worker_threads)
	{
		memcpy(m_ip, p_ip, 16);

		InitializeSRWLock(&m_session_index_stack_lock);

		for (auto n = m_max_connection - 1; n >= 0; n--)
		{
			m_session_vector[n].session_socket = INVALID_SOCKET;
			memset(&m_session_vector[n].session_address, 0, sizeof(SOCKADDR_IN));

			m_session_vector[n].session_io_count = 0;
			m_session_vector[n].session_is_release = false;

			m_session_vector[n].session_unique_id = { 0, 0 };

			InitializeSRWLock(&m_session_vector[n].session_sent_stack_lock);
			m_session_vector[n].session_sent_count = 0;

			m_session_vector[n].session_is_sending = false;

			m_session_index_stack.push(n);
		}
	}

	CNetServer::~CNetServer()
	{

	}

	auto CNetServer::GetNewSessionIndex()
	{
		AcquireSRWLockExclusive(&m_session_index_stack_lock);

		auto index = m_session_index_stack.top();
		m_session_index_stack.pop();

		ReleaseSRWLockExclusive(&m_session_index_stack_lock);

		return index;
	}

	auto CNetServer::InitNetWork()
	{
		WSADATA wsa;
		if (WSAStartup(MAKEWORD(2, 2), &wsa))
		{
			auto error = WSAGetLastError();
			throw error;
		}

		m_listen_socket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
		if (m_listen_socket == INVALID_SOCKET)
		{
			auto error = WSAGetLastError();
			throw error;
		}

		m_address.sin_family = AF_INET;
		m_address.sin_port = htons(m_port);
		InetPton(AF_INET, m_ip, &m_address.sin_addr);

		if (::bind(m_listen_socket, (SOCKADDR*)&m_address, sizeof(m_address)) == SOCKET_ERROR)
		{
			auto error = WSAGetLastError();
			throw error;
		}

		m_iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, 0);
		if (m_iocp == NULL)
		{
			auto error = WSAGetLastError();
			throw error;
		}

		if (listen(m_listen_socket, SOMAXCONN) == SOCKET_ERROR)
		{
			auto error = WSAGetLastError();
			throw error;
		}
	}

	auto CNetServer::InitThreads()
	{
		m_accept_thread = thread(&CNetServer::Accept_thread_function, this);
		if (m_accept_thread.native_handle() == NULL)
			throw en_netserver_error_code::CANNOT_MAKE_ACCEPT_THREAD;

		for(auto& n : m_worker_thread_vector)
		{
			n = thread(&CNetServer::Worker_thread_function, this);
			if(n.native_handle() == NULL)
				throw en_netserver_error_code::CANNOT_MAKE_WORKER_THREAD;
		}
	}

	auto CNetServer::SendPacket(unsigned session_id, CPacket_serialization* pPacket)
	{

	}

	auto CNetServer::Disconnect(unsigned session_id)
	{

	}

	auto CNetServer::ReleaseSession(st_session* pSession)
	{

	}

	auto CNetServer::RecvProcess(st_session* pSession)
	{

	}

	auto CNetServer::SendProcess(st_session* pSession)
	{

	}

	void CNetServer::Accept_thread_function()
	{
		SOCKET session_socket = INVALID_SOCKET;
		SOCKADDR_IN session_address;
		auto len = sizeof(session_address);

		while (true)
		{
			memset(&session_address, 0, sizeof(session_address));

			session_socket = WSAAccept(m_listen_socket, (SOCKADDR*)&session_address, reinterpret_cast<int*>(&len), NULL, NULL);

			// total accept tried count
			m_session_total_accept_try_count++;

			if (session_socket == INVALID_SOCKET)
			{
				auto error = WSAGetLastError();
				OnError(error, L"Accept Error\n");
				continue;
			}

			// total accept recognized tps count
			// add tps count to total recognized count
			m_session_total_accept_recog_tps_count++;

			// over flow max session
			if (m_session_total_connecting_count >= m_max_connection)
			{
				closesocket(session_socket);
				continue;
			}

			// now connecting session count
			m_session_total_connecting_count++;

			auto index = GetNewSessionIndex();
			m_session_vector[index].session_heart_beat = system_clock::now();

			if (m_session_id_counter >= 0 && m_session_id_counter < 10)
				m_session_id_counter = 0;

			m_session_vector[index].session_socket = session_socket;
			memcpy(&m_session_vector[index].session_address, &session_address, sizeof(session_address));

			m_session_vector[index].session_unique_id.HighPart = m_session_id_counter++;
			m_session_vector[index].session_unique_id.LowPart = index;
			m_session_vector[index].session_io_count = 1;

			if (!CreateIoCompletionPort(reinterpret_cast<HANDLE>(session_socket), m_iocp, reinterpret_cast<ULONG_PTR>(&m_session_vector[index]), 0))
			{
				closesocket(session_socket);

				auto error = WSAGetLastError();

				OnError(-99, L"CreateIoCompletionPort Error\n");
				ReleaseSession(&m_session_vector[index]);
				continue;
			}

			OnClientJoin(m_session_vector[index].session_unique_id.HighPart);

			RecvProcess(&m_session_vector[index]);

			// confirm session's io count.
			// if io count have zero count, release session.
			if (m_session_vector[index].session_io_count.fetch_sub(1) - 1 == 0)
			{
				Disconnect(m_session_vector[index].session_unique_id.HighPart);
				ReleaseSession(&m_session_vector[index]);
			}
		}
	}

	void CNetServer::Worker_thread_function()
	{

	}

	auto CNetServer::Start()
	{
		try
		{
			InitNetWork();

			InitThreads();
		}
		catch (const int& exception)
		{
			OnError(exception, L"Init network error\n");
			return false;
		}
		catch (const en_netserver_error_code& exception)
		{
			OnError(static_cast<int>(exception), L"System error\n");
			return false;
		}

		return true;
	}

	auto CNetServer::Stop()
	{
		if (m_accept_thread.joinable())
			m_accept_thread.join();
		else
		{
			m_accept_thread.detach();
			m_accept_thread.join();
		}

		for (auto& n : m_worker_thread_vector)
		{
			if (n.joinable())
				n.join();
			else
			{
				n.detach();
				n.join();
			}
		}
	}
}