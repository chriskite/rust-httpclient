# httpclient
## Simple HTTP client library for Rust

## API
    fn get(url: str) -> response
    fn post(url: str, data: map<str,str>, headers: [str]) -> response
    fn post_raw(url: str, post_body: str, headers: [str]) -> response

## Example
    let resp = httpclient::get("http://www.example.com");
    std::io::println(resp.body);
