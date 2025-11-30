# HTTP Server For Windows

This is a simple http server built from scratch in C on top of the
winsock2 API. 

It was built as an excercise in understanding http but it 
would still support serving a static sites fairly. It has the following 
features:

- Multi-Threaded
- MIME support
- Trailing Slash Redirection to path/index.html
- Protection from ".." file grabbing
- Support for images
- That's it there's not much to an http server for static sites

Looking to add:

- Support for caching on the browser
- Extra metadata for headers (dates, etc)

This project compiles using the MinGW toolchain.

To exit safely use Ctrl-C.
