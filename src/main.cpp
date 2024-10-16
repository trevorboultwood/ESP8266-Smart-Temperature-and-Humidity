/*
SmartTemp Client and/or Server
--------------------------------
This code allows you to configure an ESP device to act as either an HTTP server, 
an HTTP client, or both. Before using this code, make sure to modify the 
wifi_config header file located at: include/wifi_config.h

Uncomment the necessary lines to enable the desired functionality.
*/

#define HTTPSERVER      // Uncomment to enable HTTP Server
#define HTTPCLIENT      // Uncomment to enable HTTP Client

/*
Network Configuration:
---------------------
If an HTTP client is enabled, set the server endpoint below. 
Example: 192.168.1.2. Ensure the server software is running to receive communications.
*/
const char* server_ip = "192.168.1.177"; // Replace with your server IP.
const int server_port = 80;               // Replace with your server port.
const int local_server_port = 80;         // This is the port of the ESP Server.

/*
Post Frequency:
---------------
How frequently the POST request should be made if CLIENT is enabled.
*/
#define POSTFrequency 10

/*
Sensor Configuration:
---------------------
Uncomment the line corresponding to the sensor being used:
*/
//#define Sensor_DHT11   // Uncomment for DHT11 sensor
#define Sensor_DHT22   // Uncomment for DHT22 sensor
#define DHTPIN 2     // Digital pin connected to the DHT sensor 
/*
Debugging:
-----------
Uncomment to enable serial debugging messages.
*/
#define SERIALDEBUG    // Uncomment to enable serial debugging output

/*
Deep Sleep Configuration:
-------------------------
Uncomment the following line if the deep sleep jumper is installed. 
Note: If uncommented and the jumper is not installed, the ESP will fail to wake up!
*/

//#define ESPDEEPSLEEP

//Imported libraries.
#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <wifi_config.h>
#ifdef HTTPCLIENT
  #include <ESP8266HTTPClient.h>
#endif
#include <ESP8266WiFi.h>
#ifdef Sensor_DHT11
  #define DHTTYPE    DHT11     // DHT 11
#endif
#ifdef Sensor_DHT22
  #define DHTTYPE    DHT22     // DHT 22
#endif



//On board LED pin (Not used)
const int led = 16;

// DHT variables
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

#ifdef HTTPSERVER
WiFiServer server(local_server_port);
#endif
//sometimes we dont need this.
unsigned long previousMillis = 0;

unsigned int clientPOSTFrequency = POSTFrequency; // In Seconds

void setup() {

#ifdef SERIALDEBUG
  Serial.begin(115200);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
#endif

  WiFi.begin(ssid, password);
   
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
#ifdef SERIALDEBUG
    Serial.print(".");
#endif
  }
#ifdef SERIALDEBUG
  Serial.println("");
  Serial.println("WiFi connected");
#endif

// Initialize device.
  dht.begin();
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
#ifdef SERIALDEBUG
  Serial.println(F("DHTxx Unified Sensor Example"));
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  Serial.println(F("Humidity Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
#endif
  Serial.println(F("------------------------------------"));
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;

#ifdef HTTPSERVER
  // Start the server
  server.begin();
  #ifdef SERIALDEBUG
  Serial.println("Server started");
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
  #endif
#endif
#ifdef HTTPCLIENT
  void sendPOSTRequest();
  #ifndef HTTPSERVER
  //If ONLY a client is set, we deep sleep. Otherwise, we continue hosting the server.
    #ifdef ESPDEEPSLEEP
  ESP.deepSleep(clientPOSTFrequency * 1000000);  // 10e6 microseconds = 10 seconds
    #endif
  #endif
#endif
}

#ifdef HTTPCLIENT
void sendPOSTRequest() // Do not need to wrap an ifdef arond this, as the compilers optimiser will remove it.
{
 // Get the latest readings.
  sensors_event_t tempEvent, humidEvent;
  dht.temperature().getEvent(&tempEvent);
  dht.humidity().getEvent(&humidEvent);
            
  // Check if the readings are valid.
  float temperature = tempEvent.temperature;
  float humidity = humidEvent.relative_humidity;
  String postData = "temperature=" + String(temperature) + "&humidity=" + String(humidity);

  // Make HTTP POST request.
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client; // Create a WiFiClient object.
    HTTPClient http;
    http.setTimeout(5000); // Set timeout to 5 seconds.
    http.begin(client,server_ip,server_port);  // Specify the URL.
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");  // Specify the content type.

    // Send POST request.
    int httpResponseCode = http.POST(postData);
#ifdef SERIALDEBUG
    // Check the response.
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.print("Response Code: ");
      Serial.println(httpResponseCode);
      Serial.println("Response: " + response);
    } else {
      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }
#endif
    http.end();  // Close connection.
  }
}
#endif

void loop() {
  // Only when we are running the server do we need to use the loop() function or we are a client without ESPdeepsleep.
  #ifdef HTTPSERVER
  // Listen for incoming clients.
  WiFiClient client = server.available();   
  if (client) {                             
    Serial.println("New Client.");          
    String currentLine = "";                
    while (client.connected()) {            
      if (client.available()) {             
        char c = client.read();             
        Serial.write(c);                    
        if (c == '\n') {                    
          // End of client HTTP request.
          if (currentLine.length() == 0) {
            // HTTP headers.
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            
            // Get the latest readings.
            sensors_event_t tempEvent, humidEvent;
            dht.temperature().getEvent(&tempEvent);
            dht.humidity().getEvent(&humidEvent);
            
            // Check if the readings are valid.
            float temperature = tempEvent.temperature;
            float humidity = humidEvent.relative_humidity;
            
            // Prepare the response body.
            String responseBody;
            if (isnan(temperature) || isnan(humidity)) {
              responseBody = "<h1>Error reading from DHT sensor</h1>";
            } else {
              responseBody = "<h1>DHT11 Sensor Data</h1>"
                             "<p>Temperature: " + String(temperature) + " &deg;C</p>"
                             "<p>Humidity: " + String(humidity) + " %</p>";
            }

            // Send Content-Length header.
            client.println("Content-Length: " + String(responseBody.length()));
            client.println();
            client.println(responseBody);
            
            // End of the HTTP response.
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    // Close the connection.
    client.stop();
    #ifdef SERIALDEBUG
    Serial.println("Client Disconnected.");
    #endif
  }

  // Add a delay between each client response to avoid overwhelming the server.
  delay(100);
  #endif

  #ifdef HTTPCLIENT
  unsigned long currentMillis = millis();

  // Check if 10 seconds have passed.
  if (currentMillis - previousMillis >= clientPOSTFrequency * 1000) {
    previousMillis = currentMillis;  // Update the last time.
    // Post now
    sendPOSTRequest();
  }
  #endif
}
