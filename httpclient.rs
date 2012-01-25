use std;
import std::map;
import ctypes::*;

/* Interface with rustcurl */
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
    fn rustcurl_http_post(url: *u8, post: *void) -> *rustcurl_response;
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
    ret {body: body, header: header}
}

/* TODO implement this as post */
fn post(url: str) -> response unsafe {
    let url_bytes = str::bytes(url);

    let resp = rustcurl::rustcurl_http_get(vec::unsafe::to_ptr(url_bytes));
    let body = str::unsafe_from_bytes(vec::unsafe::from_buf((*resp).body.buf, (*resp).body.size));
    let header = str::unsafe_from_bytes(vec::unsafe::from_buf((*resp).header.buf, (*resp).header.size));


    ret {body: body, header: header}
}


// ./httpclient [url]
fn main(args: [str]) {
    let resp = get(args[1]);
    std::io::println(resp.body);
}
