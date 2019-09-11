#include "extensions/openpower-pels/data_interface.hpp"

#include <gmock/gmock.h>

namespace openpower
{
namespace pels
{

class MockDataInterface : public DataInterfaceBase
{
  public:
    MockDataInterface()
    {
    }
    MOCK_CONST_METHOD0(getMachineTypeModel, std::string());
    MOCK_CONST_METHOD0(getMachineSerialNumber, std::string());
};

} // namespace pels
} // namespace openpower
