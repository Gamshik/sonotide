#include "sonotide/runtime.h"
#include "sonotide/version.h"

int main() {
    [[maybe_unused]] constexpr auto version = sonotide::version_string;
    [[maybe_unused]] sonotide::runtime_options options{};
    return 0;
}
