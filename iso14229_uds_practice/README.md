# UDS (ISO 14229) Practice — CAN Client/Server

Educational project practicing UDS diagnostics over CAN, based on the
[iso14229](https://github.com/driftregion/iso14229) library by Nick Kirkby (MIT
licensed), with modifications for hands-on learning.

## What's here
- `iso14229.c` / `iso14229.h` — the UDS library (upstream, unmodified)
- `examples/linux_server/` — UDS server over SocketCAN ISO-TP, extended with
  custom ReadDataByIdentifier (0x22) and WriteDataByIdentifier (0x2E) handlers
  for DIDs `0xF190` and `0xDEAD`
- `examples/linux_client/` — UDS client that sends RDBI requests to the server

## What I changed from upstream
- Added `case UDS_EVT_ReadDataByIdent` and `case UDS_EVT_WriteDataByIdent`
  handlers in `examples/linux_server/main.c` to serve two test DIDs
- Added `examples/linux_client/main.c` from scratch using the library's client API
- Named constants for addresses/DIDs instead of magic numbers

## Setup: virtual CAN interface

```bash
sudo modprobe vcan
sudo ip link add dev vcan0 type vcan
sudo ip link set up vcan0
```

## Running the server

```bash
cd examples/linux_server
make
./server vcan0
```

## Running the client

```bash
cd examples/linux_client
make
./client vcan0
```

## Manual testing with cansend / candump

```bash
# Read DID 0xF190
cansend vcan0 7E0#03.22.F1.90

# Read DID 0xDEAD
cansend vcan0 7E0#03.22.DE.AD

# Write 3 bytes to DID 0xDEAD
cansend vcan0 7E0#06.2E.DE.AD.11.22.33

# Watch traffic
candump vcan0
```

## Requirements

- Linux kernel with ISO-TP support (`CONFIG_CAN_ISOTP`)
- SocketCAN interface
- `can-utils` (`cansend`, `candump`)

## License

`iso14229.c` / `iso14229.h` retain their original MIT license — see `LICENSE`.