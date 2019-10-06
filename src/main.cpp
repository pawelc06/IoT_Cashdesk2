#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include "main.h"

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

char instanceURL[512];
char token[512];

//flag for saving data
bool shouldSaveConfig = false;

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}


WiFiClientSecure client;
const char*  loginServer = "login.salesforce.com";  // Server URL


void setup() {
  Serial.begin(38400);
  
  Serial.println(F("Start"));
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  Serial.println(F("Display configured"));
  

  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  if(!loadParamsFromFile(sfdcLogin,sfdcPassword,sfdcSecToken,sfdcOAuthClientId,sfdcClientSecret)){

    WiFiManagerParameter sfdc_login("sfdcLogin", "sfdcLogin", sfdcLogin, 30);
    WiFiManagerParameter sfdc_pass("sfdcPassword", "sfdcPassword", sfdcPassword, 30);
    WiFiManagerParameter sfdc_sec_token("sfdcSecToken", "sfdcSecToken", sfdcSecToken, 30);
    WiFiManagerParameter sfdc_oauth_client_id("sfdcOAuthClientId", "sfdcOAuthClientId", sfdcOAuthClientId, 90);
    WiFiManagerParameter sfdc_oauth_client_secret("sfdcClientSecret", "sfdcClientSecret", sfdcClientSecret, 30);
 
    //set config save notify callback
    wifiManager.setSaveConfigCallback(saveConfigCallback);

    // set custom ip for portal
    //wifiManager.setAPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

    //add all your parameters here
    wifiManager.addParameter(&sfdc_login);
    wifiManager.addParameter(&sfdc_pass);
    wifiManager.addParameter(&sfdc_sec_token);
    wifiManager.addParameter(&sfdc_oauth_client_id);
    wifiManager.addParameter(&sfdc_oauth_client_secret);

    wifiManager.autoConnect("IoT_Cashdesk");
    // or use this for auto generated name ESP + ChipID
    //wifiManager.autoConnect();

    // if you get here you have connected to the WiFi
    Serial.println("WiFi connected..");

    strcpy(sfdcLogin, sfdc_login.getValue());
    strcpy(sfdcPassword, sfdc_pass.getValue());
    strcpy(sfdcSecToken, sfdc_sec_token.getValue());
    strcpy(sfdcOAuthClientId, sfdc_oauth_client_id.getValue());
    strcpy(sfdcClientSecret, sfdc_oauth_client_secret.getValue());
  }

  

  //save the custom parameters to FS
  if (shouldSaveConfig)
  //if(true)
  {
    saveParamsToFile(sfdcLogin,sfdcPassword,sfdcSecToken,sfdcOAuthClientId,sfdcClientSecret);
  }

  server.begin();
  Serial.println("Webserver started...");           // Start the webserver
  Serial.print("Use this URL to connect: http://"); // Print the IP address
  // NOTE: You must use the IP address assigned to YOUR Board when printed/displayed here on your serial port
  // If your IP address is http://10.0.1.10/ then that's the address you must use, not the one I used !
  Serial.print(WiFi.localIP());
  Serial.println("/");

  // Next define what the server should do when a client connects
  server.on("/", HandleClient); // The client connected with no arguments e.g. http:192.160.0.40/
  server.on("/result", ShowClientResponse);

  /**********************************/
    
    //if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");

  
    Serial.println("\nStarting connection to server...");
  if (!client.connect(loginServer, 443))
    Serial.println("Connection failed!");
  else {
    Serial.println("Logging to Salesforce...");
    
    loginAndGetToken(sfdcLogin , sfdcPassword, instanceURL, token, sfdcSecToken,sfdcOAuthClientId,sfdcClientSecret);
    Serial.print("Instance URL:");
    Serial.println(instanceURL);

    Serial.print("Token:");
    Serial.println(token);

    String strToken = String(token);
    String strInstanceURL = String(instanceURL);

    createCustomerVisitEvent("123456","334455",false, strInstanceURL,strToken);
    
  }
  
}

void loop() {
    // put your main code here, to run repeatedly:
    server.handleClient();
    
}