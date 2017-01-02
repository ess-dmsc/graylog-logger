# *dm-graylog-logger* examples
Instructions on how to install the C++ library dm-graylog-logger can be found on the [repository page](https://bitbucket.org/europeanspallationsource/dm-graylog-logger). The intent of this page is to give you examples on how the library can be used and extended. Other than the examples shown here, the code should be used as a reference.

## Basic example
By default, the library will log messages to console, file and Graylog server on localhost port 12201.

```c++
#include <thread>
#include <graylog_logger/Log.hpp>

int main() {
    Log::Msg(Severity::Warning, "Some message.");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return 0;
}
```
The 100 *ms* delay has been added in order for the logging system to have time to actually write something before the application exists. The compiled application will print the following to console:

```
WARNING: Some message.
```

The following line will be written to the file `messages.log`:

```
2017-01-02 18:12:04 (CI0011840) WARNING: Some message.
```

Finally, a message will also be sent to the Graylog server listening on localhost port 12201.

## Printing only to console
In order to set-up the library to only print to console, all other interfaces have to be removed and a new console interface has to be added:

```c++
#include <thread>
#include <graylog_logger/Log.hpp>
#include <graylog_logger/ConsoleInterface.hpp>

int main() {
    Log::RemoveAllHandlers();
    Log::AddLogHandler(new ConsoleInterface());
    Log::Msg(Severity::Error, "This is an error.");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return 0;
}
```

This, of course, prints the following to console:

```
ERROR: This is an error.
```

## Use another file name
As the code is currently set-up to immediatly create a file logging interface on run, the file `messages.log` will be created regardless. However in order to use another file name when writing the log messages, use the following code:

```c++
#include <thread>
#include <graylog_logger/Log.hpp>
#include <graylog_logger/FileInterface.hpp>

int main() {
    Log::RemoveAllHandlers();
    Log::AddLogHandler(new FileInterface("new_log_file.log"));
    Log::Msg(Severity::Warning, "New file warning.");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return 0;
}
```

This will output the following line to the file `new_log_file.log`.

```
2017-01-02 18:29:20 (CI0011840) WARNING: New file warning.
```

## Send messages to another Graylog server
It is possible to send log messages to multiple Graylog servers (or files for that matter) at the same time:

```c++
int main() {
    Log::AddLogHandler(new GraylogInterface("somehost.com", 12201));
    Log::AddLogHandler(new GraylogInterface("anotherhost.com", 12202));
    Log::Msg(Severity::Error, "This message will be sent to multiple Graylog servers.");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return 0;
}
```

## Set global severity limit
This library currently uses a single severity limit. The following code snippet illustrates how it can be used.

```c++
#include <thread>
#include <graylog_logger/Log.hpp>

int main() {
    Log::Msg(Severity::Debug, "This debug message will not be shown.");
    Log::SetMinimumSeverity(Severity::Debug);
    Log::Msg(Severity::Debug, "This debug message will be shown.");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return 0;
}
```

The resulting output to console is a single line:

```
Debug: This debug message will be shown.
```

## Message string formating
It is possible to supply your own string formating function as shown below.

```c++
#include <thread>
#include <graylog_logger/Log.hpp>
#include <graylog_logger/ConsoleInterface.hpp>

std::string MyFormater(LogMessage &msg) {
    std::time_t cTime = std::chrono::system_clock::to_time_t(msg.timestamp);
    char timeBuffer[50];
    size_t bytes = std::strftime(timeBuffer, 50, "%T", std::localtime(&cTime));
    return std::string(timeBuffer, bytes) + " (host:" + msg.host + ", sev:" + std::to_string(int(msg.severity)) + "): " + msg.message;
}

int main() {
    Log::RemoveAllHandlers();
    auto ci = new ConsoleInterface();
    ci->SetMessageStringCreatorFunction(MyFormater);
    Log::AddLogHandler(ci);
    Log::Msg(Severity::Warning, "A warning with a custom format.");
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    return 0;
}
```

The output to console produced by this code is:

```
20:10:07 (host:Hecate, sev:4): A warning with a custom format.
```

## Message string formating
Pointers to used logging interfaces can be retrieved from the logging system. This is illustrated here where the Graylog interface is singled out and its connection status is examined.

```c++
#include <iostream>
#include <vector>
#include <graylog_logger/Log.hpp>
#include <graylog_logger/GraylogInterface.hpp>

int main() {
    std::vector<LogHandler_P> interfaces = Log::GetHandlers();
    auto checkFunc = [&](){for (auto h : interfaces) {
        auto casted = dynamic_cast<GraylogInterface*>(h.get());
        if (casted != nullptr) {
            std::cout << "Queued messages (true/false): "<< casted->MessagesQueued() << std::endl;
        }
    }};
    checkFunc();
    Log::Msg(Severity::Error, "An error message");
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