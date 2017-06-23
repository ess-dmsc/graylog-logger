#include <iostream>
#include <graylog_logger/Log.hpp>

int main() {
    Log::Msg(Severity::Warning, "Danger!");
}
