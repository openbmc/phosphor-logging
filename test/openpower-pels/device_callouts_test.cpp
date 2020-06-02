/**
 * Copyright © 2020 IBM Corporation
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

#include "extensions/openpower-pels/device_callouts.hpp"
#include "extensions/openpower-pels/paths.hpp"

#include <fstream>

#include <gtest/gtest.h>

using namespace openpower::pels;
using namespace openpower::pels::device_callouts;
namespace fs = std::filesystem;

// The callout JSON looks like:
// "I2C":
//   "<bus>":
//     "<address>":
//       "Callouts": ...
//
// "FSI":
//   "<fsi link>":
//     "Callouts": ...
//
// "FSI-I2C":
//    "<fsi link>":
//      "<bus>":
//        "<address>":
//          "Callouts": ...
//
// "FSI-SPI":
//    "<fsi link>":
//      "<bus>":
//        "Callouts": ...

const auto calloutJSON = R"(
{
    "I2C":
    {
        "0":
        {
            "32":
            {
                "Callouts":[
                    {
                       "Name": "/chassis/motherboard/cpu0",
                       "LocationCode": "P1-C19",
                       "Priority": "H"
                    }
                ],
                "Dest": "proc-0 target"
            },
            "81":
            {
                "Callouts":[
                    {
                       "Name": "/chassis/motherboard/cpu0",
                       "LocationCode": "P1-C19",
                       "Priority": "H"
                    }
                ],
                "Dest": "proc-0 target"
            }
        },
        "14":
        {
            "112":
            {
                "Callouts":[
                    {
                       "Name": "/chassis/motherboard/cpu0",
                       "LocationCode": "P1-C19",
                       "Priority": "H"
                    }
                ],
                "Dest": "proc-0 target"
            },
            "114":
            {
                "Callouts":[
                    {
                       "Name": "/chassis/motherboard/cpu0",
                       "LocationCode": "P1-C19",
                       "Priority": "H"
                    },
                    {
                       "Name": "/chassis/motherboard",
                       "LocationCode": "P1",
                       "Priority": "M"
                    }
                ],
                "Dest": "proc-0 target"
            }
        }
    },
    "FSI":
    {
        "0":
        {
           "Callouts":[
                {
                    "Name": "/chassis/motherboard/cpu0",
                    "LocationCode": "P1-C19",
                    "Priority": "H"
                }
           ],
           "Dest": "proc-0 target"
        },
        "0-1":
        {
           "Callouts":[
                {
                    "Name": "/chassis/motherboard/cpu0",
                    "LocationCode": "P1-C19",
                    "Priority": "H",
                    "MRU": "core"
                }
           ],
           "Dest": "proc-0 target"
        }
    },
    "FSI-I2C":
    {
        "0-3":
        {
           "7":
           {
              "24":
              {
                 "Callouts":[
                    {
                       "Name": "/chassis/motherboard/cpu0",
                       "LocationCode": "P1-C19",
                       "Priority": "H"
                    }
                 ],
                 "Dest": "proc-0 target"
              },
              "25":
              {
                 "Callouts":[
                    {
                       "Name": "/chassis/motherboard/cpu5",
                       "LocationCode": "P1-C25",
                       "Priority": "H"
                    },
                    {
                       "Name": "/chassis/motherboard",
                       "LocationCode": "P1",
                       "Priority": "M"
                    },
                    {
                        "Name": "/chassis/motherboard/bmc",
                        "LocationCode": "P2",
                        "Priority": "L"
                    }
                 ],
                 "Dest": "proc-5 target"
              }
           }
        }
    },
    "FSI-SPI":
    {
        "8":
        {
            "3":
            {
                "Callouts":[
                    {
                       "Name": "/chassis/motherboard/cpu0",
                       "LocationCode": "P1-C19",
                       "Priority": "H"
                    }
                ],
                "Dest": "proc-0 target"
            },
            "4":
            {
                "Callouts":[
                    {
                       "Name": "/chassis/motherboard/cpu0",
                       "LocationCode": "P1-C19",
                       "Priority": "H"
                    }
                ],
                "Dest": "proc-0 target"
            }
        }
    }
})"_json;

class DeviceCalloutsTest : public ::testing::Test
{
  public:
    static void SetUpTestCase()
    {
        dataPath = getPELReadOnlyDataPath();
        std::ofstream file{dataPath / filename};
        file << calloutJSON.dump();
    }

    static void TearDownTestCase()
    {
        fs::remove_all(dataPath);
    }

    static std::string filename;
    static fs::path dataPath;
};

std::string DeviceCalloutsTest::filename = "systemA_dev_callouts.json";
fs::path DeviceCalloutsTest::dataPath;

// Test looking up the JSON file based on the system compatible names
TEST_F(DeviceCalloutsTest, getJSONFilenameTest)
{
    {
        std::vector<std::string> compatibles{"system1", "systemA", "system3"};
        EXPECT_EQ(util::getJSONFilename(compatibles),
                  fs::path{dataPath / filename});
    }

    // Actual filename not in compatibles
    {
        std::vector<std::string> compatibles{"system5", "system6"};
        EXPECT_THROW(util::getJSONFilename(compatibles), std::invalid_argument);
    }
}
