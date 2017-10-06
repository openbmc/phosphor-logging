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

## REST command to delete an error

curl -c cjar -k -X POST -H "Content-Type: application/json" -d '{"data": [ "root", "<root password>" ] }' https://<hostname/ip>/login

curl -c cjar -b cjar -k -H "Content-Type: application/json" -X POST -d '{"data": []}' https://<hostname/ip>/xyz/openbmc_project/logging/entry/<entry num>/action/Delete

example :
curl -c cjar -b cjar -k -H "Content-Type: application/json" -X POST -d '{"data": []}' https://<hostname/ip>/xyz/openbmc_project/logging/entry/4/action/Delete

## REST command to delete all errors

curl -c cjar -k -X POST -H "Content-Type: application/json" -d '{"data": [ "root", "<root password>" ] }' https://<hostname/ip>/login

curl -c cjar -b cjar -k -H "Content-Type: application/json" -X POST https://<hostname/ip>//xyz/openbmc_project/logging/action/deleteAll -d "{\"data\": [] }"

