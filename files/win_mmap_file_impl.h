#include <cstdint>

namespace oops_compiler {
    namespace files {
        struct mmap_file_impl {
            void* file_handle;
            void* file_mapping_handle;
            void* file_view;
            std::uintptr_t file_size;
        };
    }
}