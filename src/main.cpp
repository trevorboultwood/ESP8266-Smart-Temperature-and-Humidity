/*
      SmartTemp Client or Server
      Mdify the wifi_config header file (include/wifi_config.h)
*/
//Uncomment out whether you want a HTTP server or client or both.
//#define HTTPSERVER
#define HTTPCLIENT

//Uncomment out if the deesleep jumper has been installed. If Uncommented out, and the jumper is not installed, the ESP will fail to wake up!
//#define ESPDEEPSLEEP

//If a HTTPCLient is enabled, this must contain your endpoint. eg http://192.168.1.2. THe server software must also be running to receive this communication.
const char* serverURL = "http://your-server.com/post-endpoint";  // Replace with your server URL

//Uncomment out whether a DHT22 or DHT11 is being used.
//#define Sensor_DHT11
#define Sensor_DHT22

//Comment out to prevent the ESP sending debug messages via Serial.
#define SERIALDEBUG


// If client, this needs to be updated.


#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <wifi_config.h>

#ifdef HTTPCLIENT
  #include <ESP8266HTTPClient.h>
#endif

#include <ESP8266WiFi.h> //is this right?


#ifdef Sensor_DHT11
  #define DHTTYPE    DHT11     // DHT 11
#endif
#ifdef Sensor_DHT22
  #define DHTTYPE    DHT22     // DHT 22
#endif

#define DHTPIN 2     // Digital pin connected to the DHT sensor 

// DHT variables
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;


WiFiServer server(80);

//sometimes we dont need this.
unsigned long previousMillis = 0;

unsigned int clientPOSTFrequency = 10; // In Seconds

void setup() {
  Serial.begin(115200);


//initalise the webserver

// Connect to WiFi network
#ifdef SERIALDEBUG
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
  sendPOSTRequest();
  #ifndef HTTPSERVER
  //If ONLY a client is set, we deep sleep. Otherwise, we continue hosting the server.
    #ifdef ESPDEEPSLEEP
  ESP.deepSleep(clientPOSTFrequency * 1000000);  // 10e6 microseconds = 10 seconds
    #endif
  #endif
#endif
}

void sendPOSTRequest() // Do not need to wrap an ifdef arond this, as the compilers optimiser will remove it.
{
 // Get the latest readings
  sensors_event_t tempEvent, humidEvent;
  dht.temperature().getEvent(&tempEvent);
  dht.humidity().getEvent(&humidEvent);
            
  // Check if the readings are valid
  float temperature = tempEvent.temperature;
  float humidity = humidEvent.relative_humidity;
  String postData = "temperature=" + String(temperature) + "&humidity=" + String(humidity);

  // Make HTTP POST request
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    
    http.begin(serverURL);  // Specify the URL
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");  // Specify the content type

    // Send POST request
    int httpResponseCode = http.POST(postData);
#ifdef SERIALDEBUG
    // Check the response
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
    http.end();  // Close connection
  }
}
void loop() {
//Only when we are running the server do we need to use the loop() function.
#ifdef HTTPSERVER
  // Listen for incoming clients
  WiFiClient client = server.available();   
  if (client) {                             
    Serial.println("New Client.");          
    String currentLine = "";                
    while (client.connected()) {            
      if (client.available()) {             
        char c = client.read();             
        Serial.write(c);                    
        if (c == '\n') {                    
          // If the line is blank, you got two newlines in a row.
          // That’s the end of the client HTTP request, so you can send a response
          if (currentLine.length() == 0) {
            // HTTP headers
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // Get the latest readings
            sensors_event_t tempEvent, humidEvent;
            dht.temperature().getEvent(&tempEvent);
            dht.humidity().getEvent(&humidEvent);
            
            // Check if the readings are valid
            float temperature = tempEvent.temperature;
            float humidity = humidEvent.relative_humidity;
            
            if (isnan(temperature) || isnan(humidity)) {
              client.println("<h1>Error reading from DHT sensor</h1>");
            } else {
              // Send the HTML content
              client.println("<h1>DHT11 Sensor Data</h1>");
              client.println("<p>Temperature: " + String(temperature) + " &deg;C</p>");
              client.println("<p>Humidity: " + String(humidity) + " %</p>");
            }

            // The HTTP response ends with another blank line
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    // Close the connection
    client.stop();
    #ifdef SERIALDEBUG
    Serial.println("Client Disconnected.");
    #endif
  }

  // Add a delay between each client response to avoid overwhelming the server
  delay(100);
#endif

#ifdef HTTPCLIENT
unsigned long currentMillis = millis();

  // Check if 10 seconds have passed
  if (currentMillis - previousMillis >= clientPOSTFrequency * 1000) {
    previousMillis = currentMillis;  // Update the last time
    //post now.
    sendPOSTRequest();
 

  }
#endif

}
