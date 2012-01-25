all : librustcurl.so httpclient

librustcurl.so : rustcurl.c
	gcc -c -o librustcurl.so rustcurl.c -lcurl

httpclient : httpclient.rs librustcurl.so
	rustc httpclient.rs

