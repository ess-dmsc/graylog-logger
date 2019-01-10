# graylog-logger example code
Instructions on how to install the C++ library *graylog-logger* can be found on the [repository page](https://github.com/ess-dmsc/graylog-logger). The intent of this document is to give you examples on how the library can be used and extended. There is currently no code documentation except for the examples given here. For more information on implementation and interfaces see the header and implementation files. Most of the relevant interface information can be found in the header files `graylog_logger/Log.hpp` and `graylog_logger/LogUtil.hpp`.

## Basic example
By default, the library will log messages to console.

```c++
#include <graylog_logger/Log.hpp>

using Log:Severity;

int main() {
    Log::Msg(Severity::Warning, "Some message.");
    return 0;
}
```
The compiled application will (should) print the following to console:

```
WARNING: Some message.
```

## Write log messages to file
In order to set-up the library to write messages to file, a file writer has to be added.

```c++
#include <thread>
#include <chrono>
#include <graylog_logger/Log.hpp>
#include <graylog_logger/FileInterface.hpp>

using namespace Log;

int main() {
    Log::AddLogHandler(std::make_shared<FileInterface>("new_log_file.log"));
    Log::Msg(Severity::Error, "This is an error.");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return 0;
}
```

The `sleep_for()` function is added in order to give the file writing thread time to actually write the message before the application exits. The resulting file output should be similar to the following:

```
2017-01-02 18:29:20 (CI0011840) ERROR: This is an error.
```

## Send messages to a Graylog server
To use the library for its original purpose, a Graylog server interface has to be added. This can be done as follows:

```c++
#include <thread>
#include <chrono>
#include <graylog_logger/Log.hpp>
#include <graylog_logger/GraylogInterface.hpp>

int main() {
    Log::AddLogHandler(new Log::GraylogInterface("somehost.com", 12201));
    Log::Msg(Log::Severity::Error, "This message will be sent to a Graylog server.");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return 0;
}
```

As the default file handler has not been removed this will send a message to console as well as to the Graylog server on "somehost.com".

## Stop writing to console
In order to prevent the logger from writing messages to (e.g.) console but still write to file (or Graylog server), existing log handlers must be removed using the `Log::RemoveAllHandlers()` function before adding the log handlers you do want to use.

```c++
#include <thread>
#include <chrono>
#include <graylog_logger/Log.hpp>
#include <graylog_logger/FileInterface.hpp>
#include <graylog_logger/GraylogInterface.hpp>

using namespace Log;

int main() {
    RemoveAllHandlers();
    AddLogHandler(new FileInterface("new_log_file.log"));
    AddLogHandler(new GraylogInterface("somehost.com", 12201));
    Msg(Severity::Error, "This is an error.");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return 0;
}
```

## Set global severity limit
This library currently uses a global severity limit setting. The following code snippet illustrates how it can be used.

```c++
#include <graylog_logger/Log.hpp>

using Log::Severity;

int main() {
    Log::Msg(Severity::Debug, "This debug message will not be shown.");
    Log::SetMinimumSeverity(Severity::Debug);
    Log::Msg(Severity::Debug, "This debug message will be shown.");
    return 0;
}
```

The resulting output to console is the line:

```
Debug: This debug message will be shown.
```

## Message string formatting
It is possible to supply your own string formatting function as shown below.

```c++
#include <graylog_logger/Log.hpp>
#include <graylog_logger/ConsoleInterface.hpp>

using namespace Log;

std::string MyFormatter(LogMessage &msg) {
    std::time_t cTime = std::chrono::system_clock::to_time_t(msg.timestamp);
    char timeBuffer[50];
    size_t bytes = std::strftime(timeBuffer, 50, "%T", std::localtime(&cTime));
    return std::string(timeBuffer, bytes) + " (host:" + msg.host + ", sev:" + std::to_string(int(msg.severity)) + "): " + msg.message;
}

int main() {
    RemoveAllHandlers();
    auto ci = new ConsoleInterface();
    ci->SetMessageStringCreatorFunction(MyFormatter);
    AddLogHandler(ci);
    Msg(Severity::Warning, "A warning with a custom format.");
    return 0;
}
```

The definition of `struct LogMessage` can be found in the file `graylog_logger/LogUtil.hpp`. The output to console produced by this code is:

```
20:10:07 (host:Hecate, sev:4): A warning with a custom format.
```

## Retrieve message handler status
Pointers to used logging interfaces can be retrieved from the logging system. This is illustrated here where the Graylog interface is singled out and its connection status is examined.

```c++
#include <iostream>
#include <vector>
#include <graylog_logger/Log.hpp>
#include <graylog_logger/GraylogInterface.hpp>

int main() {
    Log::AddLogHandler(new Log::GraylogInterface("somehost.com", 12201));
    std::vector<Log::LogHandler_P> interfaces = Log::GetHandlers();
    auto checkFunc = [&](){for (auto h : interfaces) {
        auto casted = dynamic_cast<Log::GraylogInterface*>(h.get());
        if (casted != nullptr) {
            std::cout << "Queued messages (true/false): "<< casted->MessagesQueued() << std::endl;
        }
    }};
    checkFunc();
    Log::Msg(Log::Severity::Error, "An error message");
    checkFunc();
    return 0;
}
```

The resulting output is as follows:

```
Queued messages (true/false): 0
ERROR: An error message
Queued messages (true/false): 1
```

Although the library can print log messages to console very quickly, there is a slight delay when sending messages over the network. Thus in the second call to the GraylogInterface instance, the message is still queued up.

## Additional fields
The standard fields provided with every log message sent to the Graylog server are the following:

* Timestamp
* Host name
* Process id
* Process name
* Thread id
* Log message
* Severity level

It is possible to add more fields if so required and this can be done globally or on a message by message basis. Only three types of fields are currently supported:

* `std::int64_t`
* `double`
* `std::string`

### Additional global fields
Additional global fields are added using the `Log::AddField()` function. It can be used as follows:

```c++
int main() {
    Log::AddField("some_key", "some_value");
    Log::AddLogHandler(new Log::GraylogInterface("somehost.com", 12201));
    Log::Msg(Log::Severity::Error, "This message will be sent to a Graylog servers.");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return 0;
}
```

The output to console will not change (unless you write a replacement string formatter) but every log message sent to the Graylog server will contain an extra field with the name `some_key` containing the value `some_value`. It is possible to change this value with consecutive calls to `Log::AddField()` using the same key but different value.

### Additional fields on a per message basis
Adding fields on a per message basis is done when calling the `Log::Msg()`-function. An example follows.

```c++
int main() {
    Log::AddLogHandler(new Log::GraylogInterface("somehost.com", 12201));
    Log::Msg(Log::Severity::Error, "A message.", {"another_key", 3.14});
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return 0;
}
```

It is possible to add several new fields as well.

```c++
int main() {
    using Log::GraylogInterface;
    Log::AddLogHandler(new GraylogInterface("somehost.com", 12201));
    Log::Msg(Severity::Error, "A message.", {{"another_key", 3.14}, {"a_third_key", "some_other_value"}});
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return 0;
}
```

Finally, an extra field added to a message will change the value (including the type) of a field set using `Log::AddField()`.

```c++
int main() {
    using Log::Severity;
    Log::AddField("some_key", "some_value");
    Log::AddLogHandler(new Log::GraylogInterface("somehost.com", 12201));
    Log::Msg(Severity::Error, "Yet another message", {"some_key", 42});
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return 0;
}
```

This last piece of code will send a log message to the Graylog server containing only a single extra field with the key `some_key` and the integer value `42`.