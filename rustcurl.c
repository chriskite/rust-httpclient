#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
 
typedef struct {
    struct curl_httppost *first;
    struct curl_httppost *last;
    char *body;
    struct curl_slist *headerlist;
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

void rustcurl_post_init(rustcurl_post *post) {
    post->first = post->last = post->body = post->headerlist = NULL;
}

void rustcurl_post_free(rustcurl_post *post) {
    curl_formfree(post->first);
    curl_slist_free_all(post->headerlist);
    if(post->body != NULL) {
        free(post->body);
    }
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

void rustcurl_post_body_set(rustcurl_post *post, char *body) {
    size_t len = strlen(body);
    post->body = malloc(len * sizeof(char));
    memcpy(post->body, body, len);
}

void rustcurl_post_field_add(rustcurl_post *post, const char *field, const char *value) {
    curl_formadd(&post->first,
                 &post->last,
                 CURLFORM_COPYNAME, field,
                 CURLFORM_COPYCONTENTS, value,
                 CURLFORM_END);
}

void rustcurl_post_header_add(rustcurl_post *post, const char *header) {
    post->headerlist = curl_slist_append(post->headerlist, header);
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
    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, post->headerlist);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "rust-httpclient");
    curl_easy_setopt(curl_handle, CURLOPT_HTTPPOST, post->first);

    curl_easy_perform(curl_handle);

    /* cleanup */
    curl_easy_cleanup(curl_handle);
    rustcurl_post_free(post);

    return resp;
}

rustcurl_response* rustcurl_http_post_raw(const char *url, rustcurl_post *post) {
    CURL *curl_handle;

    rustcurl_response *resp = (rustcurl_response*)
                                     malloc(sizeof(rustcurl_response));
    rustcurl_response_init(resp);

    curl_handle = curl_easy_init();

    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, rustcurl_write_handler);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&resp->body);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEHEADER, (void *)&resp->header);
    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, post->headerlist);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "rust-httpclient");
    curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, post->body);

    curl_easy_perform(curl_handle);

    /* cleanup */
    curl_easy_cleanup(curl_handle);
    rustcurl_post_free(post);

    return resp;
}

void rustcurl_response_free(rustcurl_response *resp) {
    free(resp->body.buf);
    resp->body.size = 0;
    free(resp->header.buf);
    resp->header.size = 0;
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
