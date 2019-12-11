#include "extensions/openpower-pels/data_interface.hpp"
#include "extensions/openpower-pels/host_interface.hpp"

#include <fcntl.h>

#include <filesystem>
#include <sdeventplus/source/io.hpp>

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
    MOCK_METHOD(std::string, getMachineTypeModel, (), (const override));
    MOCK_METHOD(std::string, getMachineSerialNumber, (), (const override));
    MOCK_METHOD(std::string, getServerFWVersion, (), (const override));
    MOCK_METHOD(std::string, getBMCFWVersion, (), (const override));

    void changeHostState(bool newState)
    {
        setHostState(newState);
    }

    void setHMCManaged(bool managed)
    {
        _hmcManaged = managed;
    }
};

/**
 * @brief The mock HostInterface class
 */
class MockHostInterface : public HostInterface
{
  public:
    MockHostInterface(sd_event* event, DataInterfaceBase& dataIface) :
        HostInterface(event, dataIface)
    {
    }

    virtual ~MockHostInterface()
    {
    }

    virtual void cancelCmd() override
    {
    }

    MOCK_METHOD(CmdStatus, sendNewLogCmd, (uint32_t, uint32_t), (override));

  protected:
    void receive(sdeventplus::source::IO& source, int fd,
                 uint32_t events) override
    {
        // Keep account of the number of commands responses for testing.
        _cmdsProcessed++;
    }

  private:
    size_t _cmdsProcessed = 0;
};

} // namespace pels
} // namespace openpower
