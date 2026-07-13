# TCP Client-Server Demo
A simple TCP client-server implementation in C using the sockets API.
This project demonstrates a TCP connection between a client and server,
including socket creation, connecting, sending/receiving data, and handling
multiple simultaneous clients using `fork()`.

## Files
- `TCP_server.c` — Sets up a server socket, listens for incoming connections,
  and forks a new child process to handle each connected client, allowing
  multiple clients to connect at once.
- `TCP_client.c` — Connects to the server (given an IP address and port) and
  receives the data sent by the server.
- `Makefile` — Builds both `server` and `client` binaries.

## How to Compile
```bash
make
```
This builds two binaries: `server` and `client`.

To remove the compiled binaries:
```bash
make clean
```

## How to Run
Both programs take the server's IP address and port as command-line arguments.

1. Start the server first:
```bash
./server 127.0.0.1 5555
```

2. In a separate terminal, run the client:
```bash
./client 127.0.0.1 5555
```

You can run multiple clients (in separate terminals) against the same
running server to see the multi-client handling in action.

## Notes
This is a demonstration of TCP socket communication and multi-client
handling via `fork()`. Possible extensions include: bidirectional
messaging, per-client identification/logging, and more robust error handling.
