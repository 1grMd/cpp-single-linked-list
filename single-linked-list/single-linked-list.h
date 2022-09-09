#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <iterator>
#include <string>
#include <utility>

template <typename Type>
class SingleLinkedList {
	// Узел списка
	struct Node {
		Node() = default;
		Node(const Type& value, Node* next)
			: value(value)
			, next_node(next) {
		}
		Type value;
		Node* next_node = nullptr;
	};

	template <typename ValueType>
	class BasicIterator {

		friend class SingleLinkedList;

		explicit BasicIterator(Node* node)
			:node_(node) {
		}

	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = Type;
		using difference_type = std::ptrdiff_t;
		using pointer = ValueType*;
		using reference = ValueType&;

		BasicIterator() = default;

		BasicIterator(const BasicIterator<Type>& other) noexcept
			:node_(other.node_){
		}

		BasicIterator& operator=(const BasicIterator& rhs) = default;

		[[nodiscard]] bool operator==(const BasicIterator<const Type>& rhs) const noexcept {
			return this->node_ == rhs.node_;
		}

		[[nodiscard]] bool operator!=(const BasicIterator<const Type>& rhs) const noexcept {
			return this->node_ != rhs.node_;
		}

		[[nodiscard]] bool operator==(const BasicIterator<Type>& rhs) const noexcept {
			return this->node_ == rhs.node_;
		}

		[[nodiscard]] bool operator!=(const BasicIterator<Type>& rhs) const noexcept {
			return this->node_ != rhs.node_;
		}

		BasicIterator& operator++() noexcept {
			assert(this->node_ != nullptr);
			node_ = node_->next_node ? node_->next_node : nullptr;
			return *this;
		}

		BasicIterator operator++(int) noexcept {
			auto old_value(*this); 
			++(*this);
			return old_value;
		}
		BasicIterator& operator+=(size_t count) noexcept {
			for (size_t i = 0; i < count; ++i) {
				++(*this);
			}
			return *this;
		}

		BasicIterator& operator+(size_t count) noexcept {
			auto it(*this);
			return it+=count;
		}

		[[nodiscard]] reference operator*() const noexcept {
			assert(this->node_ != nullptr);
			ValueType& result = node_->value;
			return result;
		}

		[[nodiscard]] pointer operator->() const noexcept {
			assert(this->node_ != nullptr);
			ValueType& result = node_->value;
			return &result;
		}

	private:
		Node* node_ = nullptr;
	};

public:
	using value_type = Type;
	using reference = value_type&;
	using const_reference = const value_type&;

	using Iterator = BasicIterator<Type>;

	using ConstIterator = BasicIterator<const Type>;

	SingleLinkedList()
		: head_(), size_(0) {
	}

	SingleLinkedList(const SingleLinkedList& other)
		: head_(), size_(0) {
		SingleLinkedList<Type> tmp;
		auto pos_it = tmp.before_begin();
		for(auto value_it = other.begin(); value_it != other.end(); ++value_it) {
			tmp.InsertAfter(pos_it,*value_it);
			++pos_it;
		}
		swap(tmp);
	}

	SingleLinkedList(std::initializer_list<Type> values)
		: head_(), size_(0) {
		SingleLinkedList<Type> tmp;
		for (auto it = rbegin(values); it != rend(values); ++it) {
			tmp.PushFront(*it);
		}
		swap(tmp);
	}

	SingleLinkedList& operator=(const SingleLinkedList& rhs) {
		if (this->begin() == rhs.begin()) {
			return *this;
		}
		SingleLinkedList<Type> tmp(rhs);
		if (this->size_ != 0) {
			this->Clear();
		}
		this->swap(tmp);
		return *this;
	}

	void Clear() {
		while (head_.next_node) {
			PopFront();
		}
	}

	Iterator EraseAfter(ConstIterator pos) noexcept {
		if(pos == end() || !pos.node_->next_node) {
			return Iterator(nullptr);
		}
		auto erase_node = pos.node_->next_node;
		pos.node_->next_node = erase_node->next_node;
		delete erase_node;
		--size_;
		return Iterator(pos.node_->next_node);
	}

	size_t GetSize() const noexcept {
		return size_;
	}

	Iterator InsertAfter(ConstIterator pos, const Type& value) {
		if (pos == end()) {
			return Iterator(nullptr);
		}
		Node* insert_element = new Node(value, pos.node_->next_node);
		pos.node_->next_node = insert_element;
		++size_;
		return Iterator(insert_element);
	}

	bool IsEmpty() const noexcept {
		return size_ == 0;
	}

	void PopFront () {
		if(!head_.next_node) {
			return;
		}
		Node* front_node = head_.next_node;
		head_.next_node = head_.next_node->next_node;
		delete front_node;
		--size_;
	}

	void PushFront(const Type& value) {
		Node* front_element = new Node(value, head_.next_node);
		head_.next_node = front_element;
		++size_;
	}

	void swap(SingleLinkedList& other) noexcept {
		std::swap(size_, other.size_);
		std::swap(head_.next_node, other.head_.next_node);
	}

	[[nodiscard]] Iterator begin() noexcept {
		return head_.next_node ? BasicIterator<Type>(head_.next_node) : BasicIterator<Type>();
	}

	[[nodiscard]] Iterator end() noexcept {
		return BasicIterator<Type>();;
	}

	[[nodiscard]] ConstIterator begin() const noexcept {
		return head_.next_node ? BasicIterator<Type>(head_.next_node) : BasicIterator<Type>();
	}

	[[nodiscard]] ConstIterator end() const noexcept {
		return BasicIterator<Type>();
	}

	[[nodiscard]] ConstIterator cbegin() const noexcept {
		return head_.next_node ? BasicIterator<Type>(head_.next_node) : BasicIterator<Type>();
	}

	[[nodiscard]] ConstIterator cend() const noexcept {
		return BasicIterator<Type>();
	}

	[[nodiscard]] Iterator before_begin() noexcept {
		return BasicIterator<Type>(&head_);
	}

	[[nodiscard]] ConstIterator cbefore_begin() const noexcept {
		return BasicIterator<Type>(const_cast<Node*>(&head_));

	}

	[[nodiscard]] ConstIterator before_begin() const noexcept {
		return cbefore_begin();
	}

	~SingleLinkedList(){
		Clear();
	}
private:
	// Фиктивный узел, используется для вставки "перед первым элементом"
	Node head_;
	size_t size_;
};

template <typename Type>
void swap(SingleLinkedList<Type>& lhs, SingleLinkedList<Type>& rhs) noexcept {
	lhs.swap(rhs);
}

template <typename Type>
bool operator==(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return lhs.GetSize() == rhs.GetSize() ?
			std::equal(lhs.begin(), lhs.end(), rhs.begin())
			: false;
}

template <typename Type>
bool operator!=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return !(lhs == rhs);
}

template <typename Type>
bool operator<(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename Type>
bool operator<=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return (lhs == rhs) || (lhs < rhs);
}

template <typename Type>
bool operator>(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return rhs < lhs;
}

template <typename Type>
bool operator>=(const SingleLinkedList<Type>& lhs, const SingleLinkedList<Type>& rhs) {
	return (lhs == rhs) || (lhs > rhs);
}
