# 📁 FTP File Transfer Server

A lightweight TCP-based file transfer system built in C using POSIX sockets. The server handles multiple concurrent client connections via `fork()`, allowing clients to request and download files over the network.

---

## 📌 Features

- TCP socket-based client-server architecture
- Concurrent client handling using `fork()`
- File transfer with size-based header protocol
- Command-line driven — no configuration files needed
- Clean error handling and informative status messages

---

## 🛠️ Tech Stack

- **Language:** C (C99)
- **OS:** Linux / Unix
- **Libraries:** POSIX sockets, `unistd.h`, `fcntl.h`, `sys/stat.h`

---

## 📂 Project Structure

```
.
├── server.c       # Server-side code (accepts connections, sends files)
├── client.c       # Client-side code (connects, requests file, saves locally)
└── README.md
```

---

## ⚙️ Build Instructions

### Compile Server
```bash
gcc server.c -o server
```

### Compile Client
```bash
gcc client.c -o client
```

---

## 🚀 Usage

### Start the Server

```bash
./server <Port>
```

**Example:**
```bash
./server 9000
```

### Run the Client

```bash
./client <IP Address> <Port> <Remote Filename> <Local Save Filename>
```

**Example:**
```bash
./client 127.0.0.1 9000 Demo.txt Downloaded.txt
```

| Argument | Description |
|----------|-------------|
| `argv[1]` | Server IP Address |
| `argv[2]` | Server Port Number |
| `argv[3]` | Name of the file to download from server |
| `argv[4]` | Name to save the downloaded file as |

---

## 📡 Protocol Design

The client and server communicate using a simple custom text-based protocol:

```
Client  →  Server :  "<filename>\n"
Server  →  Client :  "OK <filesize>\n"   (on success)
                     "ERR\n"             (if file not found)
Server  →  Client :  <raw file bytes>
```

1. Client connects and sends the requested filename followed by `\n`
2. Server responds with a header: `OK <filesize>\n`
3. Server streams the raw file content
4. Client reads exactly `<filesize>` bytes and writes them to the output file

---

## 🔄 Architecture Flow

```
Client                          Server
  |                               |
  |------- connect() ------------>|
  |------- "Demo.txt\n" -------->|
  |<------ "OK 1700\n" ----------|
  |<------ <file bytes> ---------|
  |                               |
  |------- close() ------------->|
```

---

## ⚠️ Limitations

- No authentication or access control
- File path is relative to the server's working directory
- Maximum filename length is 50 characters (server-side buffer)
- No encryption (plain TCP)
- Zombie processes may accumulate (no `SIGCHLD` handler for `wait()`)

---

## 👤 Author

**Mangesh Ashok Bedre**

---

## 📄 License

This project is intended for educational purposes. Feel free to use and modify.
 