export LDFLAGS=-fPIC

all : librustcurl.so httpclient

librustcurl.so : rustcurl.c
	gcc -O2 -fPIC -o librustcurl.so -c rustcurl.c -lcurl

httpclient : httpclient.rs httpclient.rc librustcurl.so
	rustc --lib httpclient.rc

