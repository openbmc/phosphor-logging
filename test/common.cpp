#include "config.h"

#include <unistd.h>

#include <string>

namespace
{
// Use a separate persistence root per test process. meson runs test
// executables in parallel and several of them create entries with the same
// ID, e.g. log_manager_rbmc_sync_test and serialization_test_json both use
// entry 77, so a shared root makes them overwrite or delete each other's
// files.
const std::string persistPathRoot =
    "/tmp/phosphor-logging-" + std::to_string(getpid());
} // namespace

const char* PERSIST_PATH_ROOT = persistPathRoot.c_str();
const bool IS_UNIT_TEST = true;
