#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include "main.h"
#include "scanner.h"


//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

bool areCredentialsFilled(){
  
  
  if(strlen(sfdcLogin)>0 && strlen(sfdcPassword)>0 && strlen(sfdcSecToken)>0 && strlen(sfdcOAuthClientId)>0 && strlen(sfdcClientSecret)>0){
    return true; 
  } else { 
    return false;
  }
}


WiFiClientSecure client;
const char*  loginServer = "login.salesforce.com";  // Server URL
String strToken;
String strInstanceURL;

void setup() {
  bool credentialsLoaded=false;
  Serial.begin(38400);

  Serial2.begin(38400);
  
  Serial.println(F("Start"));
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.clearDisplay();

  display.setTextSize(1);             // Normal 1:1 pixel scale
  display.setTextColor(WHITE);        // Draw white text
  display.setCursor(0,0);             // Start at top-left corner
  display.println(F("Starting..."));

  Serial.println(F("Display configured"));
  

  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  if(!loadParamsFromFile(sfdcLogin,sfdcPassword,sfdcSecToken,sfdcOAuthClientId,sfdcClientSecret)){

    credentialsLoaded = true;
    WiFiManagerParameter sfdc_login("sfdcLogin", "sfdcLogin", sfdcLogin, 30);
    WiFiManagerParameter sfdc_pass("sfdcPassword", "sfdcPassword", sfdcPassword, 30);
    WiFiManagerParameter sfdc_sec_token("sfdcSecToken", "sfdcSecToken", sfdcSecToken, 30);
    WiFiManagerParameter sfdc_oauth_client_id("sfdcOAuthClientId", "sfdcOAuthClientId", sfdcOAuthClientId, 90);
    WiFiManagerParameter sfdc_oauth_client_secret("sfdcClientSecret", "sfdcClientSecret", sfdcClientSecret, 80);


 
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

        Serial.print("!!!!!!!!!!!!! sfdc_oauth_client_id.getValue():");
          Serial.println(sfdc_oauth_client_id.getValue());

          Serial.print("!!!!!!!!!!!!! LEN:");
          Serial.println(sfdc_oauth_client_id.getValueLength());

          Serial.print("!!!!!!!!!!!!! sfdcOAuthClientId:");
          Serial.println(sfdcOAuthClientId);
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
  Serial.println("Connected to WiFi access point!");
  display.println("Connected to WiFi!");
  display.println(WiFi.localIP()); 
  display.display();





  if(credentialsLoaded && areCredentialsFilled()){
    Serial.println("\nStarting connection to server...");
    if (!client.connect(loginServer, 443))
      Serial.println("Connection failed!");
    else {
      Serial.println("Logging to Salesforce...");
      
      if(loginAndGetToken(sfdcLogin , sfdcPassword, instanceURL, token, sfdcSecToken,sfdcOAuthClientId,sfdcClientSecret)>-1){
        Serial.print("Instance URL:");
        Serial.println(instanceURL);

        Serial.print("Token:");
        Serial.println(token);

        strToken = String(token);
        strInstanceURL = String(instanceURL);

        //createCustomerVisitEvent("123456","334455",false, strInstanceURL,strToken);
       } else {
         Serial.println("SFDC Login error!");
       }
    
      
    }
  } else {
    Serial.print("credentialsLoaded:");
    Serial.println(credentialsLoaded);

    Serial.print("areCredentialsFilled():");
    Serial.println(areCredentialsFilled());

    Serial.print("sfdcLogin:");
    Serial.println(sfdcLogin);    

    Serial.print("sfdcPassword:");
    Serial.println(sfdcPassword);  

    Serial.print("sfdcSecToken:");
    Serial.println(sfdcSecToken);  

    Serial.print("sfdcOAuthClientId:");
    Serial.println(sfdcOAuthClientId);  

    Serial.print("sfdcClientSecret:");
    Serial.println(sfdcClientSecret);    

  } 

  Serial.println("Completed setup, entering loop...");
  
}

void loop() {
    char barcode[32];
    
    if(handleBarcodeScanner(barcode)){
      display.clearDisplay();
      display.setCursor(0,32);
      display.println("Barcode:"); 
      display.println(barcode); 
      display.display();
      createCustomerVisitEvent("123456",barcode,false, strInstanceURL,strToken);
    }

    server.handleClient();
    
}

