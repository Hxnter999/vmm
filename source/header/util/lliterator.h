#pragma once
#include <commons.h>

template <typename T, typename U>
static constexpr size_t OffsetOf(U T::* member)
{
	return reinterpret_cast<size_t>(&(static_cast<T*>(nullptr)->*member));
}


template <typename T>
class LinkedList
{

	struct NODE
	{
		NODE(PLIST_ENTRY node, size_t offset) : node(node), offset(offset)
		{}

		size_t offset;
		PLIST_ENTRY node;

		T* operator->()
		{
			return reinterpret_cast<T*>(reinterpret_cast<size_t>(node) - offset);
		}
		T* operator*()
		{
			return reinterpret_cast<T*>(reinterpret_cast<size_t>(node) - offset);
		}
		void operator++()
		{
			node = node->Flink;
		}
		bool operator!=(NODE other)
		{
			return node != other.node;
		}
	};

	size_t offset;
	NODE store;
	typedef NODE iterator;

public:
	template <typename U>
	LinkedList(PLIST_ENTRY store, U T::* member) : offset(OffsetOf(member)), store(store, offset) {}

	LinkedList(T* store, size_t offset = 0) : offset(offset), store((PLIST_ENTRY)(store), offset) {}

	iterator begin() { return NODE{ (store.node->Flink), offset }; }
	iterator end() { return store; }

};