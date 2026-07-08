#include "doctest.h"

#include <boost/filesystem/directory.hpp>
#include <system_error>

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>

#include "pkg/exec.h"
#include "pkg/git.h"

#include "test_dir.h"

namespace fs = boost::filesystem;
using pkg::exec;
using pkg::get_remote;

TEST_CASE("get_remote") {
  auto const test_repo =
      fs::path{TEST_EXECUTION_DIR} / "test" / "git_test_repo";

  if (fs::exists(test_repo)) {
    fs::remove_all(test_repo);
  }
  fs::create_directory(test_repo);
  exec(test_repo, "git init");
  exec(test_repo, "git remote add origin git@github.com:foo/foo.git");
  exec(test_repo, "git remote add remoteA git@github.com:bar/bar.git");
  exec(test_repo, "git remote add remoteB ssh://git@github.com:bar/bar.git");
  exec(test_repo, "git remote add remoteC https://git@github.com:bar/bar.git");
  exec(test_repo, "git remote add remoteD ssh://git@github.com:baz/baz.git");
  exec(test_repo, "git remote add remoteE git@github.com:zab/zab.git");
  exec(test_repo, "git remote set-url --push remoteE read-only");

  CHECK("remoteA" == get_remote(test_repo, "git@github.com:bar/bar.git"));
  CHECK("remoteD" == get_remote(test_repo, "ssh://git@github.com:baz/baz.git"));
  CHECK("origin" == get_remote(test_repo, "git@github.com:oof/oof.git"));
  CHECK("remoteE" == get_remote(test_repo, "git@github.com:zab/zab.git"));
  CHECK("origin" == get_remote(test_repo, "read-only"));

  fs::remove_all(test_repo);
}