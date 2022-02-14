
#include "audit_manager.hpp"

#include <iostream>

namespace phosphor
{
namespace audit
{

void Manager::notify(uint64_t /* transactionId */)
{
    // TODO: Retrieve audit log by transactionId from journal and do further
    // action

    return;
}
} // namespace audit
} // namespace phosphor