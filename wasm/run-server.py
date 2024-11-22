#!/usr/bin/python3

import http.server
import os
import urllib.request

logo_file_path = "build/logo.png"
logo_url = "https://github.com/Hossein-Noroozpour/gearoenix-static-files/raw/master/logo.png"

if not os.path.exists(logo_file_path):
    os.makedirs(os.path.dirname(logo_file_path), exist_ok=True)
    urllib.request.urlretrieve(logo_url, logo_file_path)


class MyHTTPRequestHandler(http.server.SimpleHTTPRequestHandler):
    def end_headers(self):
        self.send_header('Cross-Origin-Embedder-Policy', 'require-corp')
        self.send_header('Cross-Origin-Opener-Policy', 'same-origin')
        http.server.SimpleHTTPRequestHandler.end_headers(self)


if __name__ == '__main__':
    server_address = ("0.0.0.0", 8080)
    httpd = http.server.HTTPServer(server_address, MyHTTPRequestHandler)
    httpd.serve_forever()
