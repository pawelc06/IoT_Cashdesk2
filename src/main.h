

#if defined(ESP8266)
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#else
#include <WiFi.h>          //https://github.com/esp8266/Arduino
#endif

//needed for library
#include <DNSServer.h>
#if defined(ESP8266)
#include <ESP8266WebServer.h>
#else
#include <WebServer.h>
#endif
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
/* ----------------------- */


#include <ArduinoJson.h>          // https://github.com/bblanchon/ArduinoJson

#include <WiFiClientSecure.h>
#include <MFRC522.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#include <SPI.h>
#include "sfdc.h"
#include "file_manager.h"

//extern WiFiClient client;
extern WebServer server; // Start server on port 80 (default for a web-browser, change to your requirements, e.g. 8080 if your Router uses port 80
                             // To access server from the outsid of a WiFi network e.g. ESP8266WebServer server(8266); and then add a rule on your Router that forwards a
                             // connection request to http://your_network_ip_address:8266 to port 8266 and view your ESP server from anywhere.
                             // Example http://g6ejd.uk.to:8266 will be directed to http://192.168.0.40:8266 or whatever IP address your router gives to this server

extern String Argument_Name, Clients_Response;


char sfdcLogin[30];
char sfdcPassword[30];
char sfdcSecToken[30];
char sfdcOAuthClientId[90];
char sfdcClientSecret[30];

String strSfdcLogin; 
String strSfdcPassword;
String strSfdcSecToken;
String strSfdcOAuthClientId;
String strSfdcClientSecret;

//flag for saving data
extern bool shouldSaveConfig;

extern void saveConfigCallback (); 
extern void HandleClient();
extern void ShowClientResponse();
void handleKey() ;
unsigned long getID();



char instanceURL[512];
char token[512];

//flag for saving data
bool shouldSaveConfig = false;