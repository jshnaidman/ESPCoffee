// ESP8266 as Webserver turning LED on/off and reading TMP36 temperature sensor
// This code is based on Mike bitar's temperature reader


#include <ESP8266WiFi.h>

// WiFi Definitions 
const char WiFiSSID[] = "<INSERT NETWORK NAME";
const char WiFiPSK[] = "<INSERT PASSWORD>";

// Pin Definitions 
const int COFFEE_PIN = 2; // pin to toggle LED on/off
const int LED = 0; // LED that indicates status of coffee machine

WiFiServer server(80);

void setup() 
{
  initHardware();
  connectWiFi();
  server.begin();
}

void loop() 
{
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Match the request
  int val = -1; // We'll use 'val' to keep track of both the
                // request type (read/set) and value if set.
  if (req.indexOf("/coffee/1") != -1)
    val = 1; // Will turn Coffee on
  else if (req.indexOf("/coffee/0") != -1)
    val = 0; // Will turn Coffee off
  else if (req.indexOf("/read") != -1)
    val = -2; // Will print coffee status
  // Otherwise request will be invalid. We'll say as much in HTML

  // Set GPIO0 according to the request
  if (val >= 0)
    digitalWrite(COFFEE_PIN, val);

  client.flush();

  // Prepare the response. Start with the common header:
  String s = "HTTP/1.1 200 OK\r\n";
  s += "Content-Type: text/html\r\n\r\n";
  s += "<!DOCTYPE HTML>\r\n<html>\r\n";
  // If we're setting the LED, print out a message saying we did
  if (val >= 0)
  {
    s += "Coffee is now ";
    s += (val)?"on":"off";
  }
  else if (val == -2)
  { // If we're reading pins, print out those values:
    int reading = digitalRead(LED);
    s += (reading)?"on":"off";
  }
  else
  {
    s += "Invalid Request.<br> Try coffee/1, /coffee/0, or /read.";
  }
  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);
  Serial.println("Client disonnected");

  // The client will actually be disconnected 
  // when the function returns and 'client' object is detroyed
}

void connectWiFi()
{
  byte ledStatus = LOW;
  Serial.println();
  Serial.println("Connecting to: " + String(WiFiSSID));
  // Set WiFi mode to station (as opposed to AP or AP_STA)
  WiFi.mode(WIFI_STA);

  // WiFI.begin([ssid], [passkey]) initiates a WiFI connection
  // to the stated [ssid], using the [passkey] as a WPA, WPA2,
  // or WEP passphrase.
  WiFi.begin(WiFiSSID, WiFiPSK);

  // Use the WiFi.status() function to check if the ESP8266
  // is connected to a WiFi network.
  while (WiFi.status() != WL_CONNECTED)
  {
    // Blink the LED
    digitalWrite(COFFEE_PIN, ledStatus); // Write LED high/low
    ledStatus = (ledStatus == HIGH) ? LOW : HIGH;

    // Delays allow the ESP8266 to perform critical tasks
    // defined outside of the sketch. These tasks include
    // setting up, and maintaining, a WiFi connection.
    delay(100);
    // Potentially infinite loops are generally dangerous.
    // Add delays -- allowing the processor to perform other
    // tasks -- wherever possible.
  }
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}


void initHardware()
{
  Serial.begin(9600);
  pinMode(LED, INPUT_PULLUP);
  pinMode(COFFEE_PIN, OUTPUT);
  digitalWrite(COFFEE_PIN, HIGH);
  // Don't need to set ANALOG_PIN as input, 
  // that's all it can be.
}
