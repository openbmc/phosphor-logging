# phosphor-logging
phosphor logging provides mechanism for common event and logging creation based on information from the journal log.

## To Build
```
To build this package, do the following steps:

    1. ./bootstrap.sh
    2. ./configure ${CONFIGURE_FLAGS}
    3. make

To clean the repository run `./bootstrap.sh clean`.
```

##To delete indivitual log entry

busctl call xyz.openbmc_project.Logging /xyz/openbmc_project/logging/entry/<entry num>  xyz.openbmc_project.Object.Delete Delete

curl -c cjar -b cjar -k -H "Content-Type: application/json" -X POST -d '{"data": []}' https://<hostname/ip>/xyz/openbmc_project/logging/entry/<entry num>/action/Delete

##example

curl -c cjar -b cjar -k -H "Content-Type: application/json" -X POST -d '{"data": []}' https://<hostname/ip>/xyz/openbmc_project/logging/entry/4/action/Delete

##To delete all event logs in one shot

busctl call xyz.openbmc_project.Logging  /xyz/openbmc_project/logging xyz.openbmc_project.Collection.DeleteAll DeleteAll

curl -c cjar -b cjar -k -H "Content-Type: application/json" -X POST https://<hostname/ip>//xyz/openbmc_project/logging/action/deleteAll -d "{\"data\": [] }"

