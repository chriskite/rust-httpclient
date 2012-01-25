#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
 
struct rustcurl_buffer {
    char *buf;
    size_t size;
};
 
struct rustcurl_response {
    struct rustcurl_buffer body;
    struct rustcurl_buffer header;
};
 
struct rustcurl_response* rustcurl_init_response() {
    struct rustcurl_response *resp = (struct rustcurl_response*)
                                     malloc(sizeof(struct rustcurl_response));
    resp->body.buf = malloc(1);
    resp->body.size = 0;
    resp->header.buf = malloc(1);
    resp->header.size = 0;
    return resp;
}

size_t
rustcurl_write_handler(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct rustcurl_buffer *data = (struct rustcurl_buffer*)userp;

    data->buf = realloc(data->buf, data->size + realsize + 1);
    if (data->buf == NULL) {
        /* out of memory! */ 
        printf("rustcurl: OUT OF MEMORY\n");
        exit(EXIT_FAILURE);
    }

    memcpy(&(data->buf[data->size]), contents, realsize);
    data->size += realsize;
    data->buf[data->size] = 0;

    return realsize;
}
 
struct rustcurl_response* rustcurl_http_get(char *url) {
    CURL *curl_handle;

    struct rustcurl_response *resp = rustcurl_init_response();

    curl_handle = curl_easy_init();
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, rustcurl_write_handler);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&resp->body);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEHEADER, (void *)&resp->header);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "rust-httpclient");
    curl_easy_perform(curl_handle);
    curl_easy_cleanup(curl_handle);

    return resp;
}
 
int main(int argc, char **argv)
{
    char *url = argv[1];
    struct rustcurl_response* resp;

    curl_global_init(CURL_GLOBAL_ALL);

    resp = rustcurl_http_get(url);
    printf("%s\n", resp->body.buf);
    printf("%lu body bytes retrieved\n", (long)resp->body.size);
    printf("%s\n", resp->header.buf);
    printf("%lu header bytes retrieved\n", (long)resp->header.size);

    curl_global_cleanup();

    return 0;
}
