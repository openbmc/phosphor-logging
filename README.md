# Introduction
phosphor logging provides mechanism for common event and logging creation based
on information from the journal log.

## To Build
```
To build this package, do the following steps:

    1. ./bootstrap.sh
    2. ./configure ${CONFIGURE_FLAGS}
    3. make

To clean the repository run `./bootstrap.sh clean`.
```

## REST commands
### Logging in
* Before you can do anything you need to first login:
```
    curl -c cjar -k -X POST -H "Content-Type: application/json" \
    -d '{"data": [ "root", "<root password>" ] }' https://<BMC IP>/login
```    

### List logging child objects recursively
```
curl -b cjar -k https://<BMC IP>/xyz/openbmc_project/logging/list
```

### List logging attributes of child objects
```
curl -s  -b cjar -k  -H 'Content-Type: application/json' -d '{"data" : []}' -X GET \
https://<BMC IP>///xyz/openbmc_project/logging/enumerate
```    

### Delete error
```
curl -c cjar -b cjar -k -H "Content-Type: application/json" -X POST \
-d '{"data": []}' \
https://<BMC IP>/xyz/openbmc_project/logging/entry/<entry num>/action/Delete
```    

### Delete all errors
```
curl -c cjar -b cjar -k -H "Content-Type: application/json" \
-X POST https://<<BMC IP>/xyz/openbmc_project/logging/action/deleteAll \
-d "{\"data\": [] }"
```
