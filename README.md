# Quick k/v

A small, quick in-memory cache and k/v database.

```
              _____      ______      ______         
______ ____  ____(_)________  /__    ___  /____   __
_  __ `/  / / /_  /_  ___/_  //_/    __  //_/_ | / /
/ /_/ // /_/ /_  / / /__ _  ,<       _  ,<  __ |/ / 
\__, / \__,_/ /_/  \___/ /_/|_|      /_/|_| _____/  
  /_/                                               
```

## Keys Format

* 12 character, base 62
* array<char, 12> ?
* timestamp, random
* worst case colision probability is 0.00128%. _(based on creating 3 keys in the same microsecond where 2 of the three generate the same random part)_

## Data Format

* std::string 
* std::string?
* custom short / long string?
* compression if > 1000 chars?

## Backing Database

* file system
* redis
* sqlite3
* postgress

## Interface

* direct, in-project
* unix sockets (rpc like interface)
* tcp sockets (rpc like interface)

### RPC Interface

Basic interface is CMD KEY [VALUE]. where command is numeric 000-999 followed by a 12 char key, 4 character data length for value. If data length
is zero, then no data is available; if the command does not take data, a value is ignored.

* set key value -> ok or err
* get key -> value or error
* delete key - ok or err
* search filter -> values that match the filter
* keys filter -> keys that match the filter
* last n -> last n records
* read filepath -> ok or err
* write filepath -> ok or err

###### 2025.03.27 | dpw
