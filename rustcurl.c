#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
 
typedef struct {
    struct curl_httppost *first;
    struct curl_httppost *last;
} rustcurl_post;

typedef struct {
    char *buf;
    size_t size;
} rustcurl_buffer;
 
typedef struct {
    rustcurl_buffer body;
    rustcurl_buffer header;
} rustcurl_response;
 
void rustcurl_response_init(rustcurl_response *resp) {
    resp->body.buf = malloc(1);
    resp->body.size = 0;
    resp->header.buf = malloc(1);
    resp->header.size = 0;
}

size_t
rustcurl_write_handler(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    rustcurl_buffer *data = (rustcurl_buffer*)userp;

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
 
rustcurl_response* rustcurl_http_get(const char *url) {
    CURL *curl_handle;

    rustcurl_response *resp = (rustcurl_response*)
                                     malloc(sizeof(rustcurl_response));
    rustcurl_response_init(resp);

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

void rustcurl_post_add(rustcurl_post *post, const char *field, const char *value) {
    curl_formadd(&post->first,
                 &post->last,
                 CURLFORM_COPYNAME, field,
                 CURLFORM_COPYCONTENTS, value,
                 CURLFORM_END);
}

rustcurl_response* rustcurl_http_post(const char *url, rustcurl_post *post) {
    CURL *curl_handle;

    rustcurl_response *resp = (rustcurl_response*)
                                     malloc(sizeof(rustcurl_response));
    rustcurl_response_init(resp);

    curl_handle = curl_easy_init();

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, rustcurl_write_handler);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&resp->body);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEHEADER, (void *)&resp->header);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "rust-httpclient");
    curl_easy_setopt(curl_handle, CURLOPT_HTTPPOST, post->first);

    curl_easy_perform(curl_handle);

    /* cleanup */
    curl_easy_cleanup(curl_handle);
    curl_formfree(post->first);

    return resp;
}
 
#if 0
int main(int argc, char **argv)
{
    char *url = argv[1];
    rustcurl_response* resp;


    resp = rustcurl_http_get(url);
    printf("%s\n", resp->body.buf);
    printf("%lu body bytes retrieved\n", (long)resp->body.size);
    printf("%s\n", resp->header.buf);
    printf("%lu header bytes retrieved\n", (long)resp->header.size);

    curl_global_cleanup();

    return 0;
}
#endif
