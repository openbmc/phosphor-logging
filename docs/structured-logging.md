# Structured Logging

There are currently two APIs for structured logging:
[log](../lib/include/phosphor-logging/log.hpp) and
[lg2](../lib/include/phosphor-logging/lg2.hpp). If your code is C++20 (or later)
it is preferred to use `lg2`.

## Why structured logging?

Structured logging is a method of logging where every variable piece of data is
tagged with some identifier for the data. This is opposite of unstructured
logging where logged events are free-form strings.

The principal logging daemon in OpenBMC (systemd-journald) natively supports
structured logging. As a result, there are some designs in place where specific
structured events are added to the journal and downstream these events can be
consumed. For example, one implementation of the IPMI SEL utilizes specific
journal structured data to stored and later retrieve SEL events.

Even if an argument might be made against the merits of using the journal as a
form of IPC, the value of structured logging persists. It is very common as part
of various failure-analysis operations, either on the part of a system
manufacturer or an end-user, to need to interrogate the system logs to determine
when/where/why a situation degraded. With unstructured logging, the
implementation is left chasing message format and data changes, where as with
structured logging the format is somewhat static and easily parsed.

A specific example of where structured logging is beneficial is a service which
gathers `error` or higher log reports and creates issues when a new or unknown
signature is discovered. If the only information available is an unstructured
string, any kind of signature identification requires creating a regular
expression (likely). With structured log, specific identifiers can be used as
the error signature while others are ignored. For instance, maybe a specific
`ERROR_RC` is critical to identifying the scenario but `FILE_PATH` is variable
and ignored.

For deeper understanding of the OpenBMC logging subsystem, it may be useful to
read the manpages for `man 1 journalctl` and `man 3 sd_journal_send`. Generally
accepted log-levels and their definition is historically documented in
`man 3 syslog`.

## log

The pre-C++20 logging APIs presented by phosphor-logging are
`phosphor::logging::log`. The basic format of a log call is:

```cpp
    log<level>("A message", entry("TAG0=%s", "value"), entry("TAG1=%x", 2));
```

Each log call has a level or priority, which corresponds to syslog priorities,
such as 'debug', 'info', 'emergency', a free-form message string, and any number
of entries, which are key-value pairs of data.

The 'key' of an entry is an upper-case tag for the data along with a
printf-style format string for the data. Journald imposes some restrictions on
the tag: it must be all capital letters, numbers, or underscores and must not
start with an underscore. Unfortunately, if these restrictions are not followed
or the printf string is invalid for the data, the code will compile but journald
may silently drop the log request (or pieces of it).

It is highly discouraged to dynamically create the free-form message string
because the contents are then, effectively, unstructured.

## lg2

The post-C++20 logging APIs presented by phosphor-logging are `lg2::log`. The
basic format of a log call is:

```cpp
    lg2::level("A {TAG0} occurred.", "TAG0", "foo"s, "TAG1", lg2::hex, 2);
```

Each log call has a level or priority, but the level is indicated by the
function call name (such as `lg2::debug(...)`). The log call also has a
free-form message string and any number of entries indicated by 2 or 3 argument
sets:

- key name (with the same `[_A-Z0-9]` requirements imposed by journald).
- [optional] set of format flags
- data value

The free-form message may also optionally contain brace-wrapped key names, for
which the message will be dynamically modified to contain the formatted value in
place of the `{KEY}`. This enables human-friendly message strings to be formed
without relying on verbose journald output modes.

Note: Since a free-form message with data can be created using the `{KEY}`
mechanism, no other string formatting libraries are necessary or should be used.
Avoiding the `{KEY}` feature causes the journal messages to become unstructured.
Do not use `sstream` or `{fmt}` to format the message!

The supported format flags are:

- `bin`, `dec`, `hex`
  - The [integer] data should be formatted in the requested manner.
  - Decimal is the default.
  - Examples:
    - `bin, 0xabcd` -> `0b1010101111001101`
    - `hex, 1234` -> `0x4d2`
- `field8`, `field16`, `field32`, `field64`
  - The [integer] data should be padded as if it were a field of specified
    bit-length (useful only for `bin` or `hex` data).
  - Examples:
    - `(bin | field8), 0xff` -> `0b11111111`
    - `(hex | field16), 10` -> `0x000a`

Format flags can be OR'd together as necessary: `hex | field32`.

The APIs can handle (and format appropriately) any data of the following types:
signed or unsigned integers, floating point numbers, booleans, strings
(C-strings, std::strings, or `std::string_views`), `std::filesystem::paths`,
sdbusplus enums and `object_paths`, generic enumerations (as numbers),
exceptions, and pointers. The APIs will also allow arbitrary type formatting for
any type which has a `to_string` function defined.

The APIs also perform compile-time analysis of the arguments to give descriptive
error messages for incorrect parameters or format flags. Some examples are:

- `(dec | hex)` yields:
  - `error: static assertion failed: Conflicting flags found for value type.`
- `dec` applied to a string yields:
  - `error: static assertion failed: Prohibited flag found for value type.`
- Missing a key yields:
  - `error: static assertion failed: Found value without expected header field.`
- Missing data yields:
  - `error: static assertion failed: Found header field without expected data.`
- Missing a message yields:
  - `error: use of deleted function ‘lg2::debug<>::debug()’`

Debug-level messages are only sent to journald if the `DEBUG_INVOCATION`
environment variable is set, as suggested by the `systemd.exec` manpage.

### LOG2_FMTMSG key

The API adds an extra journald key to represent the original message prior to
`{KEY}` replacement, which is saved with the `LOG2_FMTMSG` key. This is done to
facilitate searching the journal with a known fixed version of the message
(prior to the dynamic replacement).

### Key format checking

The journald APIs require that keys (also called data 'headers') may only
contain underscore, uppercase letters, or numbers (`[_A-Z0-9]`) and may not
start with underscores. If these requirements are ignored, the journal API
silently drops journal requests. In order to prevent silent bugs, the code
performs compile-time checking of these requirements.

The code that enables compile-time header checking imposes two constraints:

1. Keys / headers must be passed as constant C-string values.
   - `"KEY"` is valid; `"KEY"s` or `variable_key` is not.
2. Any constant C-string may be interpreted as a key and give non-obvious
   compile warnings about format violations.
   - Constant C-strings (`"a string"`) should be passed as a C++ literal
     (`"a string"s`) instead.

### stderr output

When running an application or daemon on a console or SSH session, it might not
be obvious that the application is writing to the journal. The `lg2` APIs detect
if the application is running on a TTY and additionally log to the TTY.

Output to stderr can also be forced by setting the `LG2_FORCE_STDERR`
environment variable to any value. This is especially useful to see log output
in OpenBMC CI test verification.

The verbosity of this output can be filtered by setting `LG2_LOG_LEVEL`
environment variable to the desired maximum log level (0-7). Messages with a
priority value greater than this setting will be suppressed. If `LG2_LOG_LEVEL`
is not set, it defaults to 7, showing all messages.

The format of information sent to the TTY can be adjusted by setting the desired
format string in the `LG2_FORMAT` environment variable. Supported fields are:

- `%%` : a `'%'` literal
- `%f` : the logging function's name
- `%F` : the logging function's file
- `%l` : the log level as an integer
- `%L` : the logging function's line number
- `%m` : the lg2 message

The default format is `"<%l> %m"`.

### Why a new API?

There were a number of issues raised by `logging::log` which are not easily
fixed with the existing API.

1. The mixture of template and temporary-constructed `entry` parameters is
   verbose and clumsy. `lg2` is far more succinct in this regard.
2. The printf format-strings were error prone and potentially missed in code
   reviews. `lg2` eliminates the need for printf strings by generating the
   formatting internally.
3. `logging::log` generates incorrect code location information (see
   openbmc/openbmc#2297). `lg2` uses C++20's `source_location` to, by default,
   generate correct code location info and can optionally be passed a
   non-defaulted `source_location` for rare but appropriate cases.
4. The previous APIs had no mechanism to generate dynamic user-friendly strings
   which caused some developers to rely on external formatting libraries such as
   `{fmt}`. `{KEY}` replacement is now a core feature of the new API.
5. When running on a TTY, `logging::log` sent data to journal and the TTY was
   silent. This resulted in some applications creating custom code to write some
   data to `stdout` and some to `logging::log` APIs. `lg2` automatically detects
   if it is running on a TTY and duplicates logging data to the console and the
   journal.

It is possible #3 and #5 could be fixed with the existing APIs, but the
remainder are only possible to be resolved with changes to the API syntax.

### Why C++20?

Solving issue openbmc/openbmc#2297 requires C++20's `source_location` feature.
It is possible that this could be solved in the `logging::log` APIs by utilizing
`#ifdef` detection of the `source_location` feature so that C++20 applications
gain this support.

Implementing much of the syntactic improvements of the `lg2` API is made
possible by leveraging C++20's Concepts feature. Experts in C++ may argue that
this could be implemented in earlier revisions of C++ using complex SFINAE
techniques with templated-class partial-specialization and overloading. Those
experts are more than welcome to implement the `lg2` API in C++17 on their own.

### Why didn't you do ...?

> Why didn't you just use `{fmt}`?

`{fmt}` is a great API for creating unstructured logging strings, but we are
trying to create structured logging. `{fmt}` doesn't address that problem
domain.

> Why invent your own formatting and not use `{fmt}`?

The formatting performed by this API is purposefully minimal. `{fmt}` is very
capable and especially apt for human-facing string formatting. That is not the
typical use-case for our logging. Instead we prioritized the following:

1. Reasonable syntactic ergonomics so that the API can easily be adopted.
2. Simple, consistent, machine parse-able data contents.
3. Sufficient human-facing messages for developer-level debug.
4. Very tight code generation at logging call sites and reasonably performant
   code.

(1) The lg2 API is roughly equivalent to `printf`, `{fmt}`, `cout` in terms of
ergonomics, but significantly better compile-time error proofing than the others
(except on par with `{fmt}` for errors).

(2) Adding robust formatting would lead to less consistent structured data with
essentially no additional benefit. Detailed field specifiers like `{.4f}` do not
serve any purpose when the consumer is another computer program, and only
minimal enhancement for developers. The typical utility formatting for
hardware-facing usage is implemented (hex, binary, field-size).

(3) The `{HEADER}` placeholders allow data to be placed in a human-friendly
manner on par with `{fmt}`.

(4) The call-site code generated by this API is almost identical to a `printf`
and the journal-facing code is on similar performance footing to the
journal_send APIs. We save some processing by using `iovec` interfaces and
providing the source-code information, compared to the older `logging` APIs and
have similar formatting performance to the printf-style formatting that
journal_send used. The only difference is that this is done in our library
rather than in `libsystemd`.

Utilizing `{fmt}` for each structured data element would impose much greater
overheads. Either we insert the `{fmt}` calls at the call-site (N calls plus N
string objects for N structured data elements), or we do them in the library
side where we lose the compile-time format checking. Also, the performance of
the more robust formatting would almost certainly be worse, especially if we do
the formatting library-side.

Logging is done often. Shifting a few values onto the stack for a printf-type
call compared to a kilobyte+ of generated code for inline `{fmt}` calls is a
significant trade-off. And one with the only major advantage being more
universally standardized API. The `lg2` API seems obvious enough in ergonomics
such that this should not be an impediment to our community of developers.

If it is later decided that we need more robust formatting or the `lg2::format`
flags were a bad idea they could be deprecated and replaced. The format flags
are a unique C++ type, which makes syntax parsing easier. As long as they are
replaced with a similar unique C++ type both syntaxes could be supported for a
time. Thus enhancing to support something like `fmt::arg` in the future could be
done without impacting existing code usage. Also, an ambitious developer could
write a Clang-Tidy code upgrader to transition from format flags to something
else, like Abseil provides for API changes.

> Doesn't duplicating the structured data in the message decrease the available
> journal space?

Far less than you might imagine. Journald keeps the messages in a compressed
binary format. Since the data embedded in the message and the structured data
are identical in content, and very near each other in the on-disk format, they
compress exceptionally well. Likely on the order of 1-2 bytes per structured
data element.

The lack of data in the default `journalctl` output was a serious impediment to
adoption of the `logging` API by some members of the development community.
Unless we dispense with structured logging entirely, this duplication seems like
a reasonable compromise.

> Doesn't the `{HEADER}` syntax needlessly lengthen the message strings?

Lengthen, yes. Needlessly, no?

An earlier `lg2` proposal had a format flag that appended data to the message
string instead of putting it in-place. The consensus was that this did not
create as human-friendly messages as developers desired so the placeholder
syntax was implemented instead.

`{fmt}` can use shorter placeholders of `{}` or `{3}`. The non-indexed syntax
would require structured data elements be in specific order and could be error
prone with code maintenance. The indexed syntax is similarly error prone and
harder to review. Both of them are more work for string formatting on the
library.

The `{HEADER}` syntax is identical to `{fmt}`'s "Named Argument" syntax but
actually with better parameter ergonomics as `{fmt}` would require wrapping the
named argument with a `fmt::arg` call, which is similar to `logging`'s `entry`.
