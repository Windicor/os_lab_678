#pragma once

#include <exception>
#include <memory>
#include <unordered_set>
#include <algorithm>
#include <iostream>

template <class T>
class IdTreeNode {
public:
	IdTreeNode(T id) : id_(id) {}

	bool add_to(T parrent_id, T new_id) {
		if (id_ == parrent_id) {
			childs_.insert(std::make_shared<IdTreeNode<T>>(new_id));
			return true;
		}
		else {
			bool is_ok = false;
			for (const auto& ch_ptr : childs_) {
				is_ok = is_ok || ch_ptr->add_to(parrent_id, new_id);
				if (is_ok) { break; }
			}
			return is_ok;
		}
	}

	bool remove(T id) {
		auto it = std::find_if(childs_.begin(), childs_.end(), [id](const auto& ptr) { return ptr->id_ == id; });
		if (it != childs_.end()) {
			childs_.erase(it);
			return true;
		}
		else {
			bool is_ok = false;
			for (const auto& ch_ptr : childs_) {
				is_ok = is_ok || ch_ptr->remove(id);
				if (is_ok) { break; }
			}
			return is_ok;
		}
	}

	bool find(T id) const {
		if (id_ == id) {
			return true;
		}
		else {
			bool is_ok = false;
			for (const auto& ch_ptr : childs_) {
				is_ok = is_ok || ch_ptr->find(id);
				if (is_ok) { break; }
			}
			return is_ok;
		}
	}

	void print(std::ostream& out, int depth) const {
		for (const auto& ptr : childs_) {
			out << std::string(depth - 1, ' ') << "-" << ptr->id_ << "\n";
			ptr->print(out, depth + 1);
		}
	}

	T id() const {
		return id_;
	}

private:
	T id_;
	std::unordered_set<std::shared_ptr<IdTreeNode<T>>> childs_;
};

template <class T>
class IdTree {
public:
	IdTree() {
		head_ = std::make_shared<IdTreeNode<T>>(T{});
	}

	bool add_to(T parrent_id, T new_id) {
		if (find(new_id)) {
			return false;
		}
		return head_->add_to(parrent_id, new_id);
	}

	bool remove(T id) {
		if (id == head_->id()) {
			head_ = std::make_shared<IdTreeNode<T>>(0);
			return true;
		}
		return head_->remove(id);
	}

	bool find(T id) const {
		return head_->find(id);
	}

	void print(std::ostream& out = std::cout) const {
		out << head_->id() << "\n";
		head_->print(out, 1);
	}

private:
	std::shared_ptr<IdTreeNode<T>> head_;
};
