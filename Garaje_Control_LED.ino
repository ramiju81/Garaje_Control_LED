/*
  WiFi Web Server LED Blink
  Note: Arduino IDE version 2.2 or above
  A simple web server that lets you blink an LED via the web.
  This sketch will create a new access point (with no password).
  It will then launch a new server and print out the IP address
  to the Serial Monitor. From there, you can open that address in a web browser
  to turn on and off the LED on pin 13.

  If the IP address of your board is yourAddress:
    http://yourAddress/H turns the LED on
    http://yourAddress/L turns it off

  created 25 Nov 2012
  by Tom Igoe
  adapted to WiFi AP by Adafruit

  Find the full UNO R4 WiFi RTC documentation here:
  https://docs.arduino.cc/tutorials/uno-r4-wifi/wifi-examples#access-point
 */


#include "WiFiS3.h"
#include "arduino_secrets.h"

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;  // your network SSID (name)
char pass[] = SECRET_PASS;  // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;           // your network key index number (needed only for WEP)

int led = LED_BUILTIN;
int ledA = 11;
int ledB = 12;
int ledC = 13;
int ledD = 14;
int vent = 15;
int puer = 16;
int status = WL_IDLE_STATUS;
WiFiServer server(80);

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }
  Serial.println("Access Point Web Server");

  pinMode(led, OUTPUT);   // set the LED pin mode
  pinMode(ledA, OUTPUT);  // set the LEDA pin mode
  pinMode(ledB, OUTPUT);  // set the LEDB pin mode
  pinMode(ledC, OUTPUT);  // set the LEDC pin mode
  pinMode(ledD, OUTPUT);  // set the LEDD pin mode
  pinMode(vent, OUTPUT);  // set the ventilador pin mode
  pinMode(puer, OUTPUT);  // set the puerta pin mode

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // by default the local IP address will be 192.168.4.1
  // you can override it with the following:
  WiFi.config(IPAddress(192, 168, 1, 8));

  // print the network name (SSID);
  Serial.print("Creating access point named: ");
  Serial.println(ssid);

  // Create open network. Change this line if you want to create an WEP network:
  status = WiFi.beginAP(ssid, pass);
  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    // don't continue
    while (true)
      ;
  }

  // wait 10 seconds for connection:
  delay(10000);

  // start the web server on port 80
  server.begin();

  // you're connected now, so print out the status
  printWiFiStatus();
}


void loop() {

  // compare the previous status to the current status
  if (status != WiFi.status()) {
    // it has changed update the variable
    status = WiFi.status();

    if (status == WL_AP_CONNECTED) {
      // a device has connected to the AP
      Serial.println("Device connected to AP");
    } else {
      // a device has disconnected from the AP, and we are back in listening mode
      Serial.println("Device disconnected from AP");
    }
  }

  WiFiClient client = server.available();  // listen for incoming clients

  if (client) {                    // if you get a client,
    Serial.println("new client");  // print a message out the serial port
    String currentLine = "";       // make a String to hold incoming data from the client
    while (client.connected()) {   // loop while the client's connected
      delayMicroseconds(10);       // This is required for the Arduino Nano RP2040 Connect - otherwise it will loop so fast that SPI will never be served.
      if (client.available()) {    // if there's bytes to read from the client,
        char c = client.read();    // read a byte, then
        Serial.write(c);           // print it out to the serial monitor
        if (c == '\n') {           // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("<p style=\"font-size:7vw;\">  <a href=\"/A\">Turn ON</a> LED_BAHIA_1<br></p>");
            client.print("<p style=\"font-size:7vw;\">  <a href=\"/B\">Turn OFF</a> LED_BAHIA_1<br></p>");
            client.print("<p style=\"font-size:7vw;\">  <a href=\"/C\">Turn ON</a> LED_BAHIA_2<br></p>");
            client.print("<p style=\"font-size:7vw;\">  <a href=\"/D\">Turn OFF</a> LED_BAHIA_2<br></p>");
            client.print("<p style=\"font-size:7vw;\">  <a href=\"/E\">Turn ON</a> LED_BAHIA_3<br></p>");
            client.print("<p style=\"font-size:7vw;\">  <a href=\"/F\">Turn OFF</a> LED_BAHIA_3<br></p>");
            client.print("<p style=\"font-size:7vw;\">  <a href=\"/G\">Turn ON</a> LED_BAHIA_4<br></p>");
            client.print("<p style=\"font-size:7vw;\">  <a href=\"/H\">Turn OFF</a> LED_BAHIA_4<br></p>");
            client.print("<p style=\"font-size:7vw;\">  <a href=\"/I\">Turn ON</a> VENTILADOR<br></p>");
            client.print("<p style=\"font-size:7vw;\">  <a href=\"/J\">Turn OFF</a> VENTILADOR<br></p>");
            client.print("<p style=\"font-size:7vw;\">  <a href=\"/M\">Turn ON</a> PUERTA<br></p>");
            client.print("<p style=\"font-size:7vw;\">  <a href=\"/N\">Turn OFF</a> PUERTA<br></p>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {  // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /A")) {
          digitalWrite(led, HIGH);   // GET /H turns the LED on
          digitalWrite(ledA, HIGH);  // GET /H turns the LEDB on
        }
        if (currentLine.endsWith("GET /B")) {
          digitalWrite(led, LOW);   // GET /L turns the LED off
          digitalWrite(ledA, LOW);  // GET /L turns the LEDB off
        }
        if (currentLine.endsWith("GET /C")) {
          digitalWrite(led, HIGH);   // GET /H turns the LED on
          digitalWrite(ledB, HIGH);  // GET /H turns the LEDB on
        }
        if (currentLine.endsWith("GET /D")) {
          digitalWrite(led, LOW);   // GET /L turns the LED off
          digitalWrite(ledB, LOW);  // GET /L turns the LEDB off
        }
        if (currentLine.endsWith("GET /E")) {
          digitalWrite(led, HIGH);   // GET /H turns the LED on
          digitalWrite(ledC, HIGH);  // GET /H turns the LEDB on
        }
        if (currentLine.endsWith("GET /F")) {
          digitalWrite(led, LOW);   // GET /L turns the LED off
          digitalWrite(ledC, LOW);  // GET /L turns the LEDB off
        }
        if (currentLine.endsWith("GET /G")) {
          digitalWrite(led, HIGH);   // GET /H turns the LED on
          digitalWrite(ledD, HIGH);  // GET /H turns the LEDB on
        }
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(led, LOW);   // GET /L turns the LED off
          digitalWrite(ledD, LOW);  // GET /L turns the LEDB off
        }
        if (currentLine.endsWith("GET /I")) {
          digitalWrite(led, HIGH);   // GET /H turns the LED on
          digitalWrite(vent, HIGH);  // GET /H turns the LEDB on
        }
        if (currentLine.endsWith("GET /J")) {
          digitalWrite(led, LOW);   // GET /L turns the LED off
          digitalWrite(vent, LOW);  // GET /L turns the LEDB off
        }
        if (currentLine.endsWith("GET /M")) {
          digitalWrite(led, HIGH);   // GET /H turns the LED on
          digitalWrite(puer, HIGH);  // GET /H turns the LEDB on
        }
        if (currentLine.endsWith("GET /N")) {
          digitalWrite(led, LOW);   // GET /L turns the LED off
          digitalWrite(puer, LOW);  // GET /L turns the LEDB off
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}