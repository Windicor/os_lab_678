#include "tree.h"

#include <algorithm>
#include <exception>

IdTreeNode::IdTreeNode(std::pair<int, pid_t> id) : id_(id) {}

bool IdTreeNode::add_to(int parrent_id, std::pair<int, pid_t> new_id) {
  if (id_.first == parrent_id) {
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

bool IdTreeNode::remove(int id) {
  auto it = std::find_if(childs_.begin(), childs_.end(), [id](const auto& ptr) { return ptr->id_.first == id; });
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

std::pair<int, pid_t> IdTreeNode::find(int id) const {
  if (id_.first == id) {
    return id_;
  } else {
    for (const auto& ch_ptr : childs_) {
      std::pair<int, pid_t> res = ch_ptr->find(id);
      if (res.first != BAD_RES.first || res.second != BAD_RES.second) {
        return res;
      }
    }
    return BAD_RES;
  }
}

void IdTreeNode::print(std::ostream& out, int depth) const {
  for (const auto& ptr : childs_) {
    out << std::string(depth - 1, ' ') << "-(" << ptr->id_.first << " " << ptr->id_.second << ")\n";
    ptr->print(out, depth + 1);
  }
}

std::pair<int, pid_t> IdTreeNode::id() const {
  return id_;
}

IdTree::IdTree() {
  head_ = std::make_shared<IdTreeNode>(std::pair<int, pid_t>{0, 0});
}

bool IdTree::add_to(int parrent_id, std::pair<int, pid_t> new_id) {
  if (find(new_id.first) != BAD_RES) {
    return false;
  }
  return head_->add_to(parrent_id, new_id);
}

bool IdTree::remove(int id) {
  if (id == head_->id().first) {
    head_ = std::make_shared<IdTreeNode>();
    return true;
  }
  return head_->remove(id);
}

std::pair<int, pid_t> IdTree::find(int id) const {
  return head_->find(id);
}

void IdTree::print(std::ostream& out) const {
  out << head_->id().first << " " << head_->id().second << "\n";
  head_->print(out, 1);
}
