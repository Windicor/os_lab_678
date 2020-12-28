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

bool IdTreeNode::find(int id) const {
  if (id_.first == id) {
    return true;
  } else {
    bool is_ok = false;
    for (const auto& ch_ptr : childs_) {
      is_ok = is_ok || ch_ptr->find(id);
      if (is_ok) {
        return true;
      }
    }
    return is_ok;
  }
}

std::pair<int, pid_t> IdTreeNode::get(int id) const {
  if (id_.first == id) {
    return id_;
  } else {
    for (const auto& ch_ptr : childs_) {
      std::pair<int, pid_t> res = ch_ptr->get(id);
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

std::unordered_set<pid_t> IdTreeNode::get_all() const {
  std::unordered_set<pid_t> res;
  for (const auto& ptr : childs_) {
    res.insert(ptr->id().second);
    res.merge(ptr->get_all());
  }
  return res;
}

std::pair<int, pid_t> IdTreeNode::id() const {
  return id_;
}

bool IdTree::add_to(int parrent_id, std::pair<int, pid_t> new_id) {
  if (!head_) {
    head_ = std::make_shared<IdTreeNode>(new_id);
    return true;
  }
  if (find(new_id.first)) {
    return false;
  }
  return head_->add_to(parrent_id, new_id);
}

bool IdTree::remove(int id) {
  if (head_) {
    if (id == head_->id().first) {
      head_ = nullptr;
      return true;
    }
    return head_->remove(id);
  }
  return false;
}

bool IdTree::find(int id) const {
  if (head_) {
    return head_->find(id);
  }
  return false;
}

std::pair<int, pid_t> IdTree::get(int id) const {
  if (head_) {
    return head_->get(id);
  }
  return BAD_RES;
}

void IdTree::print(std::ostream& out) const {
  if (head_) {
    out << "(" << head_->id().first << " " << head_->id().second << ")\n";
    head_->print(out, 1);
  }
}

std::unordered_set<pid_t> IdTree::get_all() const {
  if (head_) {
    std::unordered_set<pid_t> res;
    res.insert(head_->id().second);
    res.merge(head_->get_all());
    return res;
  }
  return {};
}
