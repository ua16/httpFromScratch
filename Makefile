
all:
	gcc.exe -static main.c -o main.exe -lws2_32

# Compiling the example code from MS docs
# example:
# 	gcc.exe -static ./server_example.c -o server_example.exe -lws2_32
# 	gcc.exe -static ./client_example.c -o client_example.exe -lws2_32 -lmswsock -ladvapi32

