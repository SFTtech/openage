Synopsis:

    #include "log/log.h"

    log::log(MSG(info) << "test" << 1337);

    auto msg = MSG(warn);
    msg.fmt("%d %8.3f", 10, 2.0) << "lol test";
    msg << "rofl";
    log::log(msg);

    villager.log(MSG(dbg) <<
            "This is how you're supposed to break log lines that are too"
            "long. " << 1337 <<
            "foo");

    throw Error(MSG(err) << "Exceptions use the MSG system as well!");

The logging system consists of the following main components:

 - `log::LogSource`: Objects that have a `.log()` member function and accept messages.
 - `log::log()`: Accepts messages using a "general" LogSource.
 - `MSG`: Macro that creates objects for the `log()` methods.
 - `log::LogSink`: prints/renders/writes log messages.

#### MSG

The `MSG` macro collects all sorts of information, including `__FILE__` and `__LINE__`.
`MSG` evaluates to a `log::MessageBuilder` constructor; the message builder accepts input in two ways:

 - "c++-style" with `operator <<` ("iostreams")
 - "c-style" with `.fmt()` ("printf")

All input is appended to the internal `log::message` object. The `MessageBuilder` is auto-converted to `log::message` if needed.

#### message

Dumb struct that holds the text and metadata.

#### LogSource

Any class that wishes to provide `.log()` may inherit from `log::LogSource`.
It just needs to implement `virtual std::string logsource_name()`.

#### LogSink

A list of all log sinks is maintained (by the `LogSink` constructor/destructor) in `LogSink::log_sink_list`.

Each time a message is passed to `LogSource::log()`, it is forwarded to each `LogSink`, which may proceed as it wishes.

Popular `LogSink` classes include¹:

| Sink         | What does it do to my messages?                             |
|--------------|-------------------------------------------------------------|
| StdOutSink   | Prints them to stdout.                                      |
| FileSink     | Writes them and all their metadata to a file.               |
| InGameSink   | Displays them next to the in-game objects they refer to.    |
| ConsoleSink  | Prints them to a in-game terminal buffer.                   |

¹) Disclaimer: May not actually be popular and/or available.
