#pragma once

#include <algorithm>
#include <exception>
#include <iostream>
#include <memory>
#include <unordered_set>

class IdTreeNode {
 public:
  IdTreeNode(int id) : id_(id) {}

  bool add_to(int parrent_id, int new_id) {
    if (id_ == parrent_id) {
      childs_.insert(std::make_shared<IdTreeNode>(new_id));
      return true;
    } else {
      bool is_ok = false;
      for (const auto& ch_ptr : childs_) {
        is_ok = is_ok || ch_ptr->add_to(parrent_id, new_id);
        if (is_ok) {
          break;
        }
      }
      return is_ok;
    }
  }

  bool remove(int id) {
    auto it = std::find_if(childs_.begin(), childs_.end(), [id](const auto& ptr) { return ptr->id_ == id; });
    if (it != childs_.end()) {
      childs_.erase(it);
      return true;
    } else {
      bool is_ok = false;
      for (const auto& ch_ptr : childs_) {
        is_ok = is_ok || ch_ptr->remove(id);
        if (is_ok) {
          break;
        }
      }
      return is_ok;
    }
  }

  bool find(int id) const {
    if (id_ == id) {
      return true;
    } else {
      bool is_ok = false;
      for (const auto& ch_ptr : childs_) {
        is_ok = is_ok || ch_ptr->find(id);
        if (is_ok) {
          break;
        }
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

  int id() const {
    return id_;
  }

 private:
  int id_;
  std::unordered_set<std::shared_ptr<IdTreeNode>> childs_;
};

class IdTree {
 public:
  IdTree() {
    head_ = std::make_shared<IdTreeNode>(0);
  }

  bool add_to(int parrent_id, int new_id) {
    if (find(new_id)) {
      return false;
    }
    return head_->add_to(parrent_id, new_id);
  }

  bool remove(int id) {
    if (id == head_->id()) {
      head_ = std::make_shared<IdTreeNode>(0);
      return true;
    }
    return head_->remove(id);
  }

  bool find(int id) const {
    return head_->find(id);
  }

  void print(std::ostream& out = std::cout) const {
    out << head_->id() << "\n";
    head_->print(out, 1);
  }

 private:
  std::shared_ptr<IdTreeNode> head_;
};
