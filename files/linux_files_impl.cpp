#include "platform_selector.h"
#ifdef OOPS_COMPILE_FOR_LINUX

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <string>

#include "files.h"
using namespace oops_compiler::files;

std::optional<oops_compiler::files::mmap_file> mmap_file::create(
    const char *filename, int length) {
  mmap_file_impl impl;
  std::string file;
  file.resize(length + 2);
  file[0] = '.';
  file[1] = '/';
  int last = -1;
  for (int i = 0; i < length; i++) {
    last = filename[i] == '.' ? i : last;
    file[i + 2] = filename[i] == '.' ? '/' : filename[i];
  }
  if (last == -1) {
    return {};
  }
  file[last + 2] = '.';
  std::cout << "Original: " << filename << "\nFile: " << file
            << "\nLength: " << length << "\n";
  struct stat st;
  if (stat(file.c_str(), &st) == 0) {
    impl.file_size = st.st_size;
    std::cout << "File size of " << st.st_size << std::endl;
    impl.fd = open(file.c_str(), O_RDONLY);
    if (impl.fd != -1) {
      std::cout << "Got file descriptor" << std::endl;
      impl.file_view =
          mmap(nullptr, impl.file_size, PROT_READ, MAP_PRIVATE, impl.fd, 0);
      if (impl.file_view != MAP_FAILED) {
        return mmap_file(impl);
      }
      close(impl.fd);
    }
  }
  return {};
}
const void *mmap_file::operator*() const { return this->impl.file_view; }
std::uintptr_t mmap_file::file_size() const { return this->impl.file_size; }
mmap_file::~mmap_file() {
  if (this->impl.file_size) {
    munmap(impl.file_view, impl.file_size);
    close(impl.fd);
    this->impl.file_size = 0;
  }
}

mmap_file::mmap_file(mmap_file &&file) {
  std::swap(this->impl, file.impl);
  file.impl.file_size = 0;
}
mmap_file &mmap_file::operator=(mmap_file &&file) {
  std::swap(this->impl, file.impl);
  return *this;
}

#endif