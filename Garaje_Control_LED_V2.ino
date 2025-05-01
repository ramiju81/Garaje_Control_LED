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


#include "Servo.h"
#include "WiFiS3.h"
#include "arduino_secrets.h"

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;  // your network SSID (name)
char pass[] = SECRET_PASS;  // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;           // your network key index number (needed only for WEP)

int led = LED_BUILTIN;
int ledA = 8;
int ledB = 9;
int ledC = 10;
int ledD = 11;

// Control de encendido/apagado de Motor DC
byte motorDC = 12;

// Control del servomotor para abrir la puerta
Servo servo;
int pos = 0;

int status = WL_IDLE_STATUS;
WiFiServer server(80);

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial port to connect. Needed for native USB port only
  }

  // Motor ventilador, to run once:
  pinMode(12, OUTPUT);

  // Servomotor, to run once:
  servo.attach(13); 

  Serial.println("Access Point Web Server");

  pinMode(led, OUTPUT);   // set the LED pin mode
  pinMode(ledA, OUTPUT);  // set the LEDA pin mode
  pinMode(ledB, OUTPUT);  // set the LEDB pin mode
  pinMode(ledC, OUTPUT);  // set the LEDC pin mode
  pinMode(ledD, OUTPUT);  // set the LEDD pin mode

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

            // Contenido de la página
            client.println("<!DOCTYPE html>");
            client.println("<html>");
            client.println("<head>");
            client.println("<title>Taller de Carros Inteligente</title>");
            client.println("<style>");
            client.println("body { font-family: Arial, sans-serif; text-align: center; background-color: #f4f4f9; margin: 0; padding: 20px; }");
            client.println("h1 { color: #333; }");
            client.println(".button { display: inline-block; padding: 15px 25px; font-size: 20px; color: #fff; background-color: #007BFF; text-decoration: none; border-radius: 5px; margin: 10px; }");
            client.println(".button:hover { background-color: #0056b3; }");
            client.println("</style>");
            client.println("</head>");
            client.println("<body>");
            client.println("<h1>Controla tu Taller</h1>");
            client.println("<p>Presiona un botón para encender o apagar los LEDs:</p>");

            // the content of the HTTP response follows the header:
            client.println("<a href=\"/A\" class=\"button\">Encender BAHIA 1</a>");
            client.println("<a href=\"/B\" class=\"button\">Apagar BAHIA 1</a>");
            client.println("<a href=\"/C\" class=\"button\">Encender BAHIA 2</a>");
            client.println("<a href=\"/D\" class=\"button\">Apagar BAHIA 2</a>");
            client.println("<a href=\"/E\" class=\"button\">Encender BAHIA 3</a>");
            client.println("<a href=\"/F\" class=\"button\">Apagar BAHIA 3</a>");
            client.println("<a href=\"/G\" class=\"button\">Encender BAHIA 4</a>");
            client.println("<a href=\"/H\" class=\"button\">Apagar BAHIA 4</a>");
 
             // Motor Ventilador, to run repeatedly:
            digitalWrite(motorDC, HIGH);
            delay(2000); 
            digitalWrite(motorDC, LOW);
            delay(2000);

            // Servomotor, to run repeatedly:
            for (pos = 0; pos <= 180; pos += 1){ 
              servo.write(pos);              
              delay(10);                       
            }
            for (pos = 180; pos >= 0; pos -= 1){ 
              servo.write(pos);              
              delay(10);      
            }


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
