# C-IPFS
IPFS implementation in C, (not just an API client library).

## Quick start for users:
* **ipfs init** to create an ipfs repository on your machine
* **ipfs add MyFile.txt** to add a file to the repository, will return with a hash that can be used to retrieve the file.
* **ipfs cat _hash_** to retrieve a file from the repository

## For techies (ipfs spec docs):
* [getting started](https://github.com/ipfs/specs/blob/master/overviews/implement-ipfs.md)
* [specifications](https://github.com/ipfs/specs)
* [getting started](https://github.com/ipfs/community/issues/177)
* [libp2p](https://github.com/libp2p/specs)

## Prerequisites: To compile the C version you will need:
* [lmdb](https://symas.com/lmdb/technical/)
* [c-libp2p](https://github.com/nathanhourt/c-libp2p)
* [mbedtls](https://tls.mbed.org/)

## How to compile the C version:
```
meson build
cd build
ninja
```
