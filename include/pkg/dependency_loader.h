#pragma once

#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <queue>
#include <set>
#include <string>
#include <vector>

#include "boost/filesystem/path.hpp"

#include "pkg/dep.h"

namespace pkg {

struct dependency_loader {
public:
  using iteration_fn_t = std::function<void(dep*, branch_commit const&)>;
  using async_iteration_fn_t = std::function<void(dep*, iteration_fn_t)>;

  explicit dependency_loader(boost::filesystem::path deps_root);
  ~dependency_loader();

  void retrieve(
      boost::filesystem::path const&,
      iteration_fn_t const& = [](dep*, branch_commit const&) {},
      bool recursive = false);

  dep* root() const;
  std::vector<dep*> sorted() const;
  std::vector<dep*> get_all() const;
  auto get_unique_paths() const {
    return get_uniques(
        [](dep* const a, dep* const b) -> bool { return a->path_ < b->path_; });
  }
  std::optional<dep*> resolve(std::string const& url) const;

private:
  void retrieve(dep* pred, iteration_fn_t const&, bool recursive);

  template <typename Fn>
  std::set<dep*, Fn> get_uniques(Fn&& comp) const {
    auto uniques = std::set<dep*, Fn>{comp};
    auto q = std::queue<dep*>{};
    q.push(deps_.at(ROOT));
    while (!q.empty()) {
      auto const d = q.front();
      uniques.insert(d);
      q.pop();
      for (auto const succ : d->succs_) {
        q.push(succ);
      }
    }

    return uniques;
  }

  boost::filesystem::path deps_root_;
  std::map<std::string, dep*> deps_;
  std::vector<std::unique_ptr<dep>> dep_mem_;
};

}  // namespace pkg