/*
 *  This sketch demonstrates how to set up a simple HTTP-like server.
 *  The server will set a GPIO pin depending on the request
 *    http://server_ip/gpio/0 will set the GPIO2 low,
 *    http://server_ip/gpio/1 will set the GPIO2 high
 *  server_ip is the IP address of the ESP8266 module, will be 
 *  printed to Serial when the module is connected.
 */

#include <ESP8266WiFi.h>

#define LED 4
const char* ssid = "IoTGateway";
const char* password = "Wifi950926";

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(6666);

IPAddress local_IP(192, 168, 1, 111);
IPAddress gateway(192, 168, 0, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress dns1(0, 0, 0, 0);
IPAddress dns2(0, 0, 0, 0);

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  delay(10);

  // prepare GPIO2
  pinMode(2, OUTPUT);
  digitalWrite(2, 0);
  
  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.config(local_IP, gateway, subnet, dns1, dns2);
  
  
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
  Serial.println(WiFi.localIP());
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
  
  // Wait until the client sends some data
  Serial.println("new client");
while(client.connected()){
  if(client.available()){
    uint8_t command[1];
    client.read(command,1);
    if(command[0] == '1'){
      digitalWrite(LED, HIGH);
      Serial.println("ON");
      String ON = "ON";
      //client.print(ON);
    }
    if(command[0] == '0'){
      digitalWrite(LED, LOW);
      Serial.println("OFF");
      String OFF = "OFF";
      //client.print(OFF);
    }
  delay(1);
  }
}
  Serial.println("Client disonnected");
  return;

  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
}

