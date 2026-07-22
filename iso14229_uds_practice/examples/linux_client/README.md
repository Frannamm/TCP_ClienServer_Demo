# Linux Client Example

Basic UDS client example using Linux kernel ISO-TP sockets (socketcan).

## Overview

This example demonstrates a minimal UDS client running on Linux, sending
ReadDataByIdentifier (0x22) requests to a UDS server over SocketCAN.

## Development history

`raw_socket_client_attempt1.c` is an earlier version written using raw
SOCK_RAW/CAN_RAW sockets and hand-built ISO-TP frames, before switching to the
iso14229 library's client API in `main.c`. Kept for reference.

## Files

- `main.c` - Client implementation, sends an RDBI request and prints the response

## Building

```bash
make
```

## Running

Requires a CAN interface (virtual or physical) with ISO-TP support, and a UDS
server (see `../linux_server`) already running on the same interface.

```bash
./client vcan0
```

## Requirements

- Linux kernel with ISO-TP support (CONFIG_CAN_ISOTP)
- SocketCAN interface