/*
 * Simple-TagoIO-Example
 * A minimal example of an ESP8266 based microcontroller sending
 * "fake" measurement values to a bucket at tago.io.
 *
 * (C) 2021 Alexander Schmitt (https://github.com/alexandermainz) 
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "main.h"

WiFiClient client;
unsigned long lastsend = 0;
const char* ssid = STASSID;
const char* password = STAPSK;

// Tago.io configuration
// The device token of your device goes here:
String Device_Token = TAGO_DEVICE_TOKEN;

void setup() {
  Serial.begin(115200);
  Serial.println();

  // connect WiFi
  initWiFi();

}

void loop() {
  // send sensor value every 1 minute
  if (millis() - lastsend > 1*60*1000) {
    lastsend = millis();

    // your sensor reading would go here.
    // for our exmaple, we make a random temperature
    float tempFake = (float)random(18, 28);

    // call helper function which posts the value to tago.io
    httpPostTago(tempFake);
  }
}

/*
 * Initalize WiFi connection
 */
void initWiFi() {
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  int tryout = 0;
  while (WiFi.status() != WL_CONNECTED && tryout++ < 3) {
    Serial.print(".");
    Serial.println(WiFi.begin(ssid, password));
    int wait = 0;
    while (WiFi.status() != WL_CONNECTED && wait++ < 12) {
      delay(500);
      Serial.print(".");
    }
    
    delay(5000);
  }
  if (WiFi.status() != WL_CONNECTED)  // reboot if WiFi cannot be connected
    ESP.restart();
    
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());  
  Serial.println(WiFi.localIP());
}

/*
 * This method makes a HTTP POST request to the tago.io server, posting the given
 * "temp" float value to the device`s bucket.
 */
void httpPostTago(float temp) {
  // close any connection before send a new request.
  client.stop();

  /*
   * Prepare the post data by creating a JSON string of the following JSON structure:
   * [
   *  {
   *    "variable": "temperature",      // this is name name of the variable for the bucket
   *    "value": "21.8",                // this is the value to be posted for the field "temperature"
   *    "unit": "C"                     // this is the measurement unit for the field value
   *  }
   * ]
   */
  String postData = String("[{\"variable\":\"temperature\", \"value\":") + String(temp) + String(",\"unit\":\"C\"}]");
  /*
   * The device token for the targeted device bucket most be transported via a special HTTP
   * header "Device-Token". This is prepared here.
   */
  String httpHeader = String("Device-Token: ") + String(Device_Token);

  Serial.print("\nStarting connection to tago.io server... ");
  if (client.connect("api.tago.io", 80)) {  // TagoIO server adress and port
    Serial.println("connected to server");
    client.println("POST /data? HTTP/1.1"); // POST endpoint is "/data?""
    client.println("Host: api.tago.io");
    client.println("_ssl: false");           // this special header value MUST be used for non-secured HTTP connection
    client.println("Connection: close");
    client.println(httpHeader);             // send our special header with device token prepared above
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(postData.length());      // send POST data length (mandatory)
    client.println();
    client.println(postData);               // send POST data
    Serial.println("request sent");

    // get HTTP status information and return values from the connection
    long httpStatus = 0;
    delay(10);
    // first get reply headers and check HTTP result code
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      Serial.println("POST -> " + line);
      if (line.startsWith("HTTP/")) {
        httpStatus = line.substring(9,12).toInt();
      }
      if (line == "\r")  // end of headers reached
        break;
      yield();
    }
    // now get the first line of the body of web server's answer, if status is OK
    if ((httpStatus == 200 || httpStatus == 202) && client.connected()) {
      yield();
      String line = client.readStringUntil('\n');
      Serial.println(line);
    }
    else
    {
      Serial.println("No return data available. HTTP return status code was " + String(httpStatus));
    }
    
    client.stop();
  }
  else {
    Serial.println("Web client connection failed!");
  }
}
