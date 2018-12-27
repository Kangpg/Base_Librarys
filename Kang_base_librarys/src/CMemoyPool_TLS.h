#pragma once

#include <Windows.h>
#include <atomic>
#include "CMemoryPool.h"

namespace My_Library
{
	typedef enum class memorypool_tls_en_exception : unsigned
	{
		// STREAMING QUEUE
		NOT_EFFECTIVE_TLS_INDEX

	}MEMORYPOOL_TLS_EXCEPTION;

	template <class T>
	class CMemoryPool_TLS
	{
	private:

		// CChunk class define
		class CChunk
		{
		private:

			enum en_chunk_size : unsigned
			{
				CHUNK_SIZE_MAX = 500
			};

			struct st_node
			{
				st_node(CChunk* ptr = nullptr) : pChunk(ptr) {}

				CChunk* pChunk;
				T data;
			};

		public:

			typedef en_chunk_size chunk_max_size;

			CChunk() = default;
			virtual ~CChunk() = default;

			auto Alloc();
			auto Free();

		private:

			auto SetPoolPtr(CMemoryPool_TLS<T>& pMemoryPool);
			auto Clear();

		private:

			st_node* m_pNode = nullptr;

			atomic<unsigned> m_allocable_index = 0;
			atomic<unsigned> m_free_count = 0;

			CMemoryPool_TLS<T>* m_pMemoryPool = nullptr;
		};

	public:

		CMemoryPool_TLS();
		virtual ~CMemoryPool_TLS();

		auto Alloc();
		auto Free(T* pData);

		auto GetPoolUsage() const { return m_pool_usage.load(); }

	private:

		auto AllocChunk();

	private:

		int m_tls_index = -1;
		atomic<unsigned> m_pool_usage = 0;

		CMemoryPool<CChunk> m_chunk_memory_pool;
	};

	template <class T>
	CMemoryPool_TLS<T>::CMemoryPool_TLS()
	{
		m_tls_index = TlsAlloc();
		if (m_tls_index == TLS_OUT_OF_INDEXES)
			throw memorypool_tls_en_exception::NOT_EFFECTIVE_TLS_INDEX;
	}

	template <class T>
	CMemoryPool_TLS<T>::~CMemoryPool_TLS()
	{
		TlsFree(m_tls_index);
	}

	template <class T>
	auto CMemoryPool_TLS<T>::Alloc()
	{
		auto pChunk = (CChunk*)TlsGetValue(m_tls_index);
		if (pChunk == nullptr)
			pChunk = AllocChunk();

		m_pool_usage++;

		return pChunk->Alloc();
	}

	template <class T>
	auto CMemoryPool_TLS<T>::Free(T* pData)
	{
		auto node = (typename CChunk::st_node*)(reinterpret_cast<char*>(pData) - sizeof(CChunk*));
		node->pChunk->Free();

		m_pool_usage--;
	}

	template <class T>
	auto CMemoryPool_TLS<T>::AllocChunk()
	{
		auto pChunk = m_chunk_memory_pool.Try_Alloc();
		if (pChunk->m_pNode == nullptr)
			pChunk->SetPoolPtr(*this);
		else
			pChunk->Clear();

		TlsSetValue(m_tls_index, pChunk);

		return pChunk;
	}

	template <class T>
	auto CMemoryPool_TLS<T>::CChunk::SetPoolPtr(CMemoryPool_TLS<T>& pMemoryPool)
	{
		m_pMemoryPool = &pMemoryPool;
		m_free_count = 0;

		m_pNode = new st_node[en_chunk_size::CHUNK_SIZE_MAX];
		for (auto i = 0; i < en_chunk_size::CHUNK_SIZE_MAX; i++)
		{
			m_pNode[i].pChunk = this;
		}
	}

	template <class T>
	auto CMemoryPool_TLS<T>::CChunk::Clear()
	{
		m_allocable_index.exchange(0);
		m_free_count = 0;
	}

	template <class T>
	auto CMemoryPool_TLS<T>::CChunk::Alloc()
	{
		auto pData = &m_pNode[m_allocable_index.load()].data;

		auto temp_allocable_index = m_allocable_index.fetch_add(1) + 1;

		if (temp_allocable_index == 500)
			m_pMemoryPool->AllocChunk();

		return pData;
	}

	template <class T>
	auto CMemoryPool_TLS<T>::CChunk::Free()
	{
		auto temp_free_count = m_free_count.fetch_add(1) + 1;

		if (temp_free_count == en_chunk_size::CHUNK_SIZE_MAX)
			m_pMemoryPool->m_chunk_memory_pool.Try_Free(m_pNode->pChunk);
	}
}