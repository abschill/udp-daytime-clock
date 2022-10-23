# daytime clock client/server via udp in c++

## requirements
- boost & asio
- clang
- cmake


## setting up

```
git clone https://github.com/abschill/udp-daytime-clock
cd udp-daytime-clock
bash setup.sh
./server/server
./client/client <private_ip_address>
```

calling the client will return the relative time of the day to standard output
