# Structured Logging

There are currently two APIs for structured logging:
[log](../lib/include/phosphor-logging/log.hpp) and
[lg2](../lib/include/phosphor-logging/lg2.hpp).  If your code is C++20 (or
later) it is preferred to use `lg2`.

## Why structured logging?

Structured logging is a method of logging where every variable piece of data is
tagged with some identifier for the data.  This is opposite of unstructured
logging where logged events are free-form strings.

The principal logging daemon in OpenBMC (systemd-journald) natively supports
structured logging.  As a result, there are some designs in place where
specific structured events are added to the journal and downstream these events
can be consumed.  For example, one implementation of the IPMI SEL utilizes
specific journal structured data to stored and later retrieve SEL events.

Even if an argument might be made against the merits of using the journal as a
form of IPC, the value of structured logging persists.  It is very common as
part of various failure-analysis operations, either on the part of a system
manufacturer or an end-user, to need to interrogate the system logs to determine
when/where/why a situation degraded.  With unstructured logging, the
implementation is left chasing message format and data changes, where as with
structured logging the format is somewhat static and easily parsed.

A specific example of where structured logging is beneficial is a service which
gathers `error` or higher log reports issues and creates issues when a new or
unknown signature is discovered.  If the only information available is an
unstructured string, any kind of signature identification requires creating a
regular expression (likely).  With structured log, specific identifiers can
be used as the error signature while others are ignored.  For instance, maybe a
specific `ERROR_RC` is critical to identifying the scenario but `FILE_PATH`
is variable and ignored.

For deeper understanding of the OpenBMC logging subsystem, it may be useful
to read the manpages for `man 1 journalctl` and `man 3 sd_journal_send`.
Generally accepted log-levels and their definition is historically documented
in `man 3 syslog`.

## log

The pre-C++20 logging APIs presented by phosphor-logging are
`phosphor::logging::log`.  The basic format of a log call is:

```
    log<level>("A message", entry("TAG0=%s", "value"), entry("TAG1=%x", 2));
```

Each log call has a level or priority, which corresponds to syslog priorities,
such as 'debug', 'info', 'emergency', a free-form message string, and any number
of entries, which are key-value pairs of data.

The 'key' of an entry is an upper-case tag for the data along with a
printf-style format string for the data.  Journald imposes some restrictions on
the tag: it must be all capital letters, numbers, or underscores and must not
start with an underscore.  Unfortunately, if these restrictions are not followed
or the printf string is invalid for the data, the code will compile but journald
may silently drop the log request (or pieces of it).

It is highly discouraged to dynamically create the free-form message string
because the contents are then, effectively, unstructured.

## lg2

The post-C++20 logging APIs presented by phosphor-logging are
`lg2::log`.  The basic format of a log call is:

```
    lg2::level("A message: {TAG0}", "TAG0", "value"_s, "TAG1", lg2::hex, 2);
```

Each log call has a level or priority, but the level is indicated by the
function call name (such as `lg2::debug(...)`).  The log call also has a
free-form message string and any number of entries indicated by 2 or 3
argument sets:

- key name (with the same `[_A-Z0-9]` requirements imposed by journald).
- [optional] set of format flags
- data value

The free-form message may also optionally contain brace-wrapped key names,
for which the message will be dynamically modified to contain the formatted
value in place of the `{KEY}`.  This enables human-friendly message strings
to be formed without relying on verbose journald output modes.

Note: Since a free-form message with data can be created using the `{KEY}`
mechanism, no other string formatting libraries are necessary or should be
used.  Avoiding the `{KEY}` feature causes the journal messages to become
unstructured.  Do not use `sstream` or `{fmt}` to format the message!

The supported format flags are:
- `bin`, `dec`, `hex`
    - The [integer] data should be formatted in the requested manner.
    - Decimal is the default.
    - Examples:
        + `(bin, 0xabcd)` -> `0b1010101111001101`
        + `(hex, 1234)` -> `0x4d2`
- `field8`, `field16`, `field32`, `field64`
    - The [integer] data should be padded as if it were a field of
      specified bit-length (useful only for `bin` or `hex` data).
    - Examples:
        + `(bin | field8, 0xff)` -> `0b11111111`
        + `(hex | field16, 10)` -> `0x000a`

Format flags can be OR'd together as necessary: `hex | field32`.

The APIs can handle (and format appropriately) any data of the following
types: signed or unsigned integers, floating point numbers, booleans, strings
(C-strings, std::strings, or std::string_views), sdbusplus enums, and pointers.

The APIs also perform compile-time analysis of the arguments to give descriptive
error messages for incorrect parameters or format flags.  Some examples are:

- `(dec | hex)` yields:
    - `error: static assertion failed: Conflicting flags found for value type.`
- `dec` applied to a string yields:
    - `error: static assertion failed: Prohibited flag found for value type.`
- Missing a key yields:
    - `error: static assertion failed: Found value without expected header
       field.`
- Missing data yields:
    - `error: static assertion failed: Found header field without expected
       data.`
- Missing a message yields:
    - `error: use of deleted function ‘lg2::debug<>::debug()’`

### LOG2_FMTMSG key

The API adds an extra journald key to represent the original message prior
to `{KEY}` replacement, which is saved with the `LOG2_FMTMSG` key.  This
is done to facilitate searching the journal with a known fixed version of
the message (prior to the dynamic replacement).

### Key format checking

The journald APIs require that keys (also called data 'headers') may only
contain underscore, uppercase letters, or numbers (`[_A-Z0-9]`) and may not
start with underscores.  If these requirements are ignored, the journal API
silently drops journal requests.  In order to prevent silent bugs, the code
performs compile-time checking of these requirements.

The code the enables compile-time header checking imposes two constraints:
1. Keys / headers must be passed as constant C-string values.
    - `"KEY"` is valid; `"KEY"_s` or `variable_key` is not.
2. Any constant C-string may be interpreted as a key and give non-obvious
   compile warnings about format violations.
    - Constant C-strings (`"a string"`) should be passed as a C++ literal
      (`"a string"_s`) instead.

### stderr output

When running an application or daemon on a console or SSH session, it might
not be obvious that the application is writing to the journal.  The `lg2` APIs
detect if the application is running on a TTY and additionally mirror the
information sent to the journal to the TTY.

### Why a new API?

There were a number of issues raised by `logging::log` which are not easily
fixed with the existing API.

1. The mixture of template and temporary-constructed `entry` parameters is
   verbose and clumsy.  `lg2` is far more succinct in this regard.
2. The printf format-strings were error prone and potentially missed in code
   reviews.  `lg2` eliminates the need for printf strings by generating the
   formatting internally.
3. `logging::log` generates incorrect code location information (see
   openbmc/openbmc#2297).  `lg2` uses C++20's `source_location` to, by default,
   generate correct code location info and can optionally be passed a
   non-defaulted `source_location` for rare but appropriate cases.
4. The previous APIs had no mechanism to generate dynamic user-friendly strings
   which caused some developers to rely on external formatting libraries such
   as `{fmt}`.  `{KEY}` replacement is now a core feature of the new API.
5. When running on a TTY, `logging::log` sent data to journal and the TTY was
   silent.  This resulted in some applications creating custom code to write
   some data to `stdout` and some to `logging::log` APIs.  `lg2` automatically
   detects if it is running on a TTY and duplicates logging data to the console
   and the journal.

It is possible #3 and #5 could be fixed with the existing APIs, but the
remainder are only possible to be resolved with changes to the API syntax.

### Why C++20?

Solving issue openbmc/openbmc#2297 requires C++20's `source_location` feature.
It is possible that this could be solved in the `logging::log` APIs by utilizing
`#ifdef` detection of the `source_location` feature so that C++20 applications
gain this support.

Implementing much of the syntactic improvements of the `lg2` API is made
possible by leveraging C++20's Concepts feature.  Experts in C++ may argue that
this could be implemented in earlier revisions of C++ using complex SFINAE
techniques with templated-class partial-specialization and overloading.  Those
experts are more than welcome to implement the `lg2` API in C++17 on their own.
