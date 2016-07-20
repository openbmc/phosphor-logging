/**
 * Copyright © 2016 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __OBMC_JOURNAL_H__
#define __OBMC_JOURNAL_H__

#include <string>
#include <systemd/sd-journal.h>

// Output messages to systemd journal log via sd_journal_send().
// For complete documentation refer to the man page for sd_journal_send().
// Usage: journal_print(int priority, const char *msg, ...);
// Required fields: int priority, const char *msg.
// Examples:
//   journal_pring(LOG_NOTICE, "This is an example");
//   journal_print(LOG_ERR, "Could not start process %s", process_name);
//   journal_print(LOG_DEBUG, "Debug msg for dbus object", DBUSOBJ=obj_name);
//   journal_print(LOG_INFO, "File op failed %d", rc, "FILE=%s", filename);
template <typename T, typename U, typename... Args>
void journal_print(T priority, U message, Args... args)
{
    // Build PRIORITY field
    // long long needed to convert priority int to string via std::to_string
    long long prio = priority;
    std::string prio_str = "PRIORITY=" + std::to_string(prio);

    // Build MESSAGE field
    std::string msg_str="MESSAGE=";
    msg_str.append(message);

    // Send to journal
    sd_journal_send(prio_str.c_str(), msg_str.c_str(), args..., NULL);
}

#endif
