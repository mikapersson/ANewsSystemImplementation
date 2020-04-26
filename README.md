# A Simple News System Implementation :newspaper:
An overview of the system:

![overview](./src/csuml.png)

## Background
A news system has been developed consisting of a news server and a news client. The server can handle two types databases, one stored in the primary memory (in-memory) and another stored on disk. A database contains newsgroups, where each newsgroup contains articles, and the client accepts commands from the user and communicates with the server. Multiple clients can be connected to the server at any time.

## Installation
Execute the following commands in the /src/ directory:

```
make all
make install
```

## Usage
After the installation, navigate to the /bin/ directory where the executables now will lie with:

```
cd ../bin
```
### Run the Server
Run the in-memory version of the server with:
```
./inmemserver [port]
```

Run the disk version of the server with:
```
./diskserver [port]
```






