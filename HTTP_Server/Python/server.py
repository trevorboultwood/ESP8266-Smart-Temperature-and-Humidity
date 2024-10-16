from http.server import BaseHTTPRequestHandler, HTTPServer
import urllib.parse
port = 80
class MyServer(BaseHTTPRequestHandler):
    def do_POST(self):
        # Get the content length to read the POST data
        content_length = int(self.headers['Content-Length'])
        post_data = self.rfile.read(content_length).decode('utf-8')

        # Parse the POST data
        parsed_data = urllib.parse.parse_qs(post_data)
        temperature = parsed_data.get('temperature', [''])[0]
        humidity = parsed_data.get('humidity', [''])[0]

        print(f"Received Temperature: {temperature}, Humidity: {humidity}")

        # Respond to the POST request
        self.send_response(200)
        self.send_header('Content-type', 'text/html')
        self.end_headers()
        response = "POST data received successfully"
        self.wfile.write(response.encode('utf-8'))

if __name__ == "__main__":
    # Define server address and port
    server_address = ('', port)  # Listening on all IPs, port 8080
    httpd = HTTPServer(server_address, MyServer)
    print(f"HTTP server running on port {port} ...")
    httpd.serve_forever()
