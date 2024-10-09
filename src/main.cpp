#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#include <ESP8266WiFi.h> //is this right?

//definitions
#define DHTTYPE    DHT11     // DHT 11
#define DHTPIN 2     // Digital pin connected to the DHT sensor 

// DHT variables
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;


// router and Webserver

const char* ssid = "RakeEnd_Residents";//type your ssid
const char* password = "maggie123";//type your password
WiFiServer server(80);






void setup() {
  Serial.begin(115200);


//initalise the webserver

// Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
   
  WiFi.begin(ssid, password);
   
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
   
  // Start the server
  server.begin();
  Serial.println("Server started");
 
  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
    

  // Initialize device.
  dht.begin();
  Serial.println(F("DHTxx Unified Sensor Example"));
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
  Serial.println(F("------------------------------------"));
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;
}

void loop() {
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
    Serial.println("Client Disconnected.");
  }

  // Add a delay between each client response to avoid overwhelming the server
  delay(100);
}
