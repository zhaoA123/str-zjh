CC = gcc
CFLAGS = -o
LDFLAGS =  -lsqlite3
TARGETS = client server

all: $(TARGETS)

# 编译client.c
client: client.c
	$(CC) $(CFLAGS)  client client.c

# 编译server.c，链接SQLite3库
server: server.c
	$(CC) $(CFLAGS)  server server.c $(LDFLAGS)

clean:
	rm -f $(TARGETS)
