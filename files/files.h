#ifndef OOPS_COMPILER_FILES_H
#define OOPS_COMPILER_FILES_H

#include "win_mmap_file_impl.h"

#include <cstdint>
#include <optional>

namespace oops_compiler {
namespace files {
class mmap_file {
 private:
  mmap_file_impl impl;
  mmap_file(mmap_file_impl impl) : impl(impl) {}

 public:
  mmap_file(const mmap_file &) = delete;
  mmap_file &operator=(const mmap_file &) = delete;
  mmap_file(mmap_file &&file);
  mmap_file &operator=(mmap_file &&file);
  static std::optional<mmap_file> create(const char *filename, int length);
  const void* operator*() const;
  std::uintptr_t file_size() const;
  ~mmap_file();
};
}  // namespace platform
}  // namespace oops

#endif