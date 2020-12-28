#pragma once

#include <iostream>
#include <memory>
#include <unordered_set>
#include <utility>

using pid_t = int;

const std::pair<int, pid_t> BAD_RES = {-1, -1};

class IdTreeNode {
 public:
  IdTreeNode(std::pair<int, pid_t> id = BAD_RES);

  bool add_to(int parrent_id, std::pair<int, pid_t> new_id);
  bool remove(int id);
  std::pair<int, pid_t> find(int id) const;
  void print(std::ostream& out, int depth) const;

  std::pair<int, pid_t> id() const;

 private:
  std::pair<int, pid_t> id_ = BAD_RES;
  std::unordered_set<std::shared_ptr<IdTreeNode>> childs_;
};

class IdTree {
 public:
  IdTree();

  bool add_to(int parrent_id, std::pair<int, pid_t> new_id);
  bool remove(int id);
  std::pair<int, pid_t> find(int id) const;
  void print(std::ostream& out = std::cout) const;

 private:
  std::shared_ptr<IdTreeNode> head_;
};
