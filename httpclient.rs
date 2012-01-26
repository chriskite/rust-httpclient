use std;
import ctypes::*;
import std::map::map;

/* Interface with rustcurl */
type rustcurl_post = {
    first: *void,
    last: *void,
    body: *u8,
    headerlist: *void
};

type rustcurl_buffer = {
    buf: *u8,
    size: size_t
};

type rustcurl_response = {
    body: rustcurl_buffer,
    header: rustcurl_buffer
};

#[link_args="-L."]
native mod rustcurl {
    fn rustcurl_http_get(url: *u8) -> *rustcurl_response;
    fn rustcurl_http_post(url: *u8, post: *rustcurl_post) -> *rustcurl_response;
    fn rustcurl_post_field_add(post: *rustcurl_post, field: *u8, value: *u8) -> *rustcurl_response;
    fn rustcurl_response_free(resp: *rustcurl_response);
    fn rustcurl_post_init(post: *rustcurl_post);
}

native mod curl {
}

/* httpclient */

type response = {
    body: str,
    header: str
};

fn get(url: str) -> response unsafe {
    let url_bytes = str::bytes(url);
    let resp = rustcurl::rustcurl_http_get(vec::unsafe::to_ptr(url_bytes));
    let body = str::unsafe_from_bytes(vec::unsafe::from_buf((*resp).body.buf, (*resp).body.size));
    let header = str::unsafe_from_bytes(vec::unsafe::from_buf((*resp).header.buf, (*resp).header.size));
    rustcurl::rustcurl_response_free(resp);
    ret {body: body, header: header}
}

fn post(url: str, data: map<str,str>) -> response unsafe {
    let post: rustcurl_post = {first: 0 as *void, last: 0 as *void, body: 0 as *u8, headerlist: 0 as *void};
    rustcurl::rustcurl_post_init(ptr::addr_of(post));

    data.items { |field, value|
        let field_bytes = str::bytes(field);
        let value_bytes = str::bytes(value);
        let field_ptr = vec::unsafe::to_ptr(field_bytes);
        let value_ptr = vec::unsafe::to_ptr(value_bytes);
        rustcurl::rustcurl_post_field_add(ptr::addr_of(post), field_ptr, value_ptr);
    }

    let url_bytes = str::bytes(url);
    let resp = rustcurl::rustcurl_http_post(vec::unsafe::to_ptr(url_bytes), ptr::addr_of(post));
    let body = str::unsafe_from_bytes(vec::unsafe::from_buf((*resp).body.buf, (*resp).body.size));
    let header = str::unsafe_from_bytes(vec::unsafe::from_buf((*resp).header.buf, (*resp).header.size));
    rustcurl::rustcurl_response_free(resp);

    ret {body: body, header: header}
}

// ./httpclient [url]
fn main(args: [str]) {
    let data = std::map::new_str_hash();
    data.insert("foo", "bar");
    let resp = post(args[1], data);
    std::io::println(resp.body);
}
