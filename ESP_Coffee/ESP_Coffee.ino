// ESP8266 as Webserver turning LED on/off or turning a coffee machine on/off


#include <ESP8266WiFi.h>

// WiFi Definitions
const char WiFiSSID[] = "";
const char WiFiPSK[] = "";

// Pin Definitions
const int COFFEE_PIN = 2; // pin to toggle LED on/off
const int LED = 0; // LED that indicates status of coffee machine

WiFiServer server(8323);

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
                                if (req.indexOf("/state/1") != -1)
                                        on(client); // Will turn Coffee on
                                          else if (req.indexOf("/state/0") != -1)
                                                  off(client); // Will turn Coffee off
                                                    else if (req.indexOf("/read") != -1)
                                                            read(client); // Will print coffee status
                                                              else if (req.indexOf("/toggle") != -1)
                                                                      toggle(client); // Will toggle coffee 
                                                                        // Otherwise request will be invalid.
                                                                          else help(client);
                                                                            
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
            digitalWrite(COFFEE_PIN, 0);
}

void off(WiFiClient client){
      digitalWrite(COFFEE_PIN,0);
        sendResponse(0, client);
}
void on(WiFiClient client){
      digitalWrite(COFFEE_PIN,1);
        sendResponse(1, client);
}
void toggle(WiFiClient client){
      int reading = digitalRead(LED);
        digitalWrite(COFFEE_PIN,!reading);
          delay(500);
            digitalWrite(COFFEE_PIN, reading);
              sendResponse(reading, client);
}
void read(WiFiClient client){
      int reading = digitalRead(LED);
        sendResponse(reading, client);
}

//Send http response to client
void sendResponse(int status, WiFiClient client){
      client.flush();
        String state = (status) ? "\"on\"" : "\"off\"";
          
            String s = "HTTP/1.1 200 OK\r\n";
              s += "Content-Type: application/json\r\n\r\n";
                s += "{\n";
                  s += "\t\"state\":" + state + "\n}";
                    
                      // Send the response to the client
                        client.print(s);
}

void help(WiFiClient client){
      client.print("HTTP/1.1 400 Bad Request\r\n");
        client.print("Content-Type: text/html\r\n\r\n");
          client.println("Try with /read, /toggle, /state/0 or /state/1");
}

