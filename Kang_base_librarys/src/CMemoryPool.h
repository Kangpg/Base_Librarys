#pragma once

#include <Windows.h>
#include <atomic>
#include <memory>
#include <new>

using namespace std;

namespace My_Library
{
	template <class T>
	class CMemoryPool
	{
	private:

		struct st_node
		{
			st_node(st_node* pNode = nullptr) : pNext(pNode) {}

			T data;
			st_node* pNext;
		};

		struct st_top
		{
			st_top(st_node* pNode = nullptr, LONG64 src_count = 0) : pTop(pNode), count(src_count) {}

			st_node* pTop;
			LONG64 count;
		};

	public:

		CMemoryPool()
		{
			m_pTop = new st_top();
		}

		virtual ~CMemoryPool()
		{
			if (m_pTop != nullptr)
				delete[] m_pTop;
		}

		T* Try_Alloc(bool is_placement = false)
		{
			m_used_node_count++;

			auto tmp_used_node_count = m_used_node_count.load();
			auto tmp_total_node_count = m_total_node_count.load();

			if (tmp_used_node_count > tmp_total_node_count)
			{
				auto tmp_node = new st_node();

				m_total_node_count++;

				if (is_placement)
					new ((T*)(&tmp_node->data)) T();

				return (T*)&tmp_node->data;
			}
			else
			{
				m_count++;
				auto alloc_count = m_count.load();

				while (true)
				{
					st_top top_compare(m_pTop->pTop, m_pTop->count);

					if (InterlockedCompareExchange128((LONG64*)m_pTop, alloc_count, (LONG64)m_pTop->pTop->pNext, (LONG64*)&top_compare))
					{
						auto tmp_node = top_compare.pTop;
						tmp_node->pNext = nullptr;
						return (T*)&tmp_node->data;
					}
				}
			}

			return nullptr;
		}

		auto Try_Free(T* pData, bool is_deconstructor = false)
		{
			auto tmp_node = reinterpret_cast<st_node*>(pData + sizeof(T));

			m_count++;

			auto alloc_count = m_count.load();

			while (true)
			{
				st_top top_compare(m_pTop->pTop, m_pTop->count);

				tmp_node->pNext = m_pTop->pTop;

				if (InterlockedCompareExchange128((LONG64*)m_pTop, alloc_count, (LONG64)tmp_node, (LONG64*)&top_compare))
				{
					if (is_deconstructor)
						pData->~T();

					m_used_node_count--;
					break;
				}
			}

			return true;
		}

		auto GetUsedNodeCount() const
		{
			return m_used_node_count.load();
		}

		auto GetTotalNodeCount() const
		{
			return m_total_node_count.load();
		}

	private:

		st_top* m_pTop = nullptr;
		atomic<LONG64> m_count = 0;

		atomic<DWORD> m_used_node_count = 0;
		atomic<DWORD> m_total_node_count = 0;
	};
}