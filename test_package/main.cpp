#include <graylog_logger/Log.hpp>

using Log::Severity;

int main() {
    Log::Msg(Severity::Warning, "All good - it's just a test. Hello from the test_package.");
    return 0;
}
