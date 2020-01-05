#include <Arduino.h>

/* ESP8266 Webserver obtaining arguments from a connected client
 The 'MIT License (MIT) Copyright (c) 2016 by David Bird'. Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, 
 distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the
 following conditions: 
   The above copyright ('as annotated') notice and this permission notice shall be included in all copies or substantial portions of the Software and where the
   software use is visible to an end-user.
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHOR OR COPYRIGHT HOLDER BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF, OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
See more at http://dsbird.org.uk
*/
#include <FS.h> //this needs to be first, or it all crashes and burns...
//#include <ESP8266WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>     // https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h>          // https://github.com/bblanchon/ArduinoJson
#include "SPIFFS.h"
#include "file_manager.h"

//WiFiClient client;
WebServer server(80); // Start server on port 80 (default for a web-browser, change to your requirements, e.g. 8080 if your Router uses port 80
                             // To access server from the outsid of a WiFi network e.g. ESP8266WebServer server(8266); and then add a rule on your Router that forwards a
                             // connection request to http://your_network_ip_address:8266 to port 8266 and view your ESP server from anywhere.
                             // Example http://g6ejd.uk.to:8266 will be directed to http://192.168.0.40:8266 or whatever IP address your router gives to this server

String Argument_Name, Clients_Response;

// Assign output variables to GPIO pins
 
extern char sfdcLogin[];
extern char sfdcPassword[];
extern char sfdcSecToken[];
extern char sfdcOAuthClientId[];
extern char sfdcClientSecret[];


extern String strSfdcLogin; 
extern String strSfdcPassword;
extern String strSfdcSecToken;
extern String strSfdcOAuthClientId;
extern String strSfdcClientSecret;



void HandleClient() {
  String webpage;

if (server.args() > 0 ) { // Arguments were received
    for ( uint8_t i = 0; i < server.args(); i++ ) {
      Serial.print(server.argName(i)); // Display the argument
      Argument_Name = server.argName(i);
     
      Serial.println(" = "+server.arg(i));
       
    }

    strSfdcLogin = server.arg(0);
    strSfdcPassword = server.arg(1);
    strSfdcSecToken = server.arg(2);
    strSfdcOAuthClientId = server.arg(3);
    strSfdcClientSecret = server.arg(4);

    Serial.println("saving config");
    saveParamsToFile(strSfdcLogin,strSfdcPassword,strSfdcSecToken,strSfdcOAuthClientId,strSfdcClientSecret);
    //end save
  } else {
    loadParamsFromFile(sfdcLogin,sfdcPassword,sfdcSecToken,sfdcOAuthClientId,sfdcClientSecret);
    strSfdcLogin = String(sfdcLogin);
    strSfdcPassword = String(sfdcPassword);
    strSfdcSecToken = String(sfdcSecToken);
    strSfdcOAuthClientId = String(sfdcOAuthClientId);
    strSfdcClientSecret = String(sfdcClientSecret);
  }

webpage="<!DOCTYPE html><html>";
              webpage+="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
              webpage+="<link rel=\"icon\" href=\"data:,\">";
              // CSS to style the on/off buttons 
              // Feel free to change the background-color and font-size attributes to fit your preferences
              webpage+="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}";
              webpage+=".button { background-color: #195B6A; border: none; color: white; padding: 16px 40px;";
              webpage+="text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}";
              webpage+=".button2 {background-color: #77878A;}</style></head>";
              
              // Web Page Heading
              webpage+="<body><h1>IoT Cashdesk ESP8266 Web Server</h1>";
              
              webpage+="<h2>Stored configuration parameters:</h2>";
              String IPaddress = WiFi.localIP().toString();
              webpage += "<form action='http://"+IPaddress+"' method='POST'>";

              webpage+="<p>SFDC Login:<br/><input type=\"text\" name=\"sfdcLogin\" value=\"" + strSfdcLogin + "\" size=\"30\"></p>";
              webpage+="<p>SFDC Password:<br/><input type=\"password\" name=\"sfdcPassword\" value=\"" + strSfdcPassword + "\" size=\"30\"></p>";
              webpage+="<p>SFDC Security token:<br/><input type=\"text\" name=\"sfdcSecToken\" value=\"" + strSfdcSecToken + "\" size=\"30\"></p>";
              webpage+="<p>SFDC Client Id:<br/><input type=\"text\" name=\"sfdcOAuthClientId\" value=\"" + strSfdcOAuthClientId  + "\" size=\"30\" maxlength=\"90\"></p>";
              webpage+="<p>SFDC Client Secret:<br/><input type=\"text\" name=\"sfdcClientSecret\" value=\"" + strSfdcClientSecret + "\" size=\"30\" maxlength=\"80\"></p>";

              
              webpage+="<input class=\"button\" type=\"submit\" value=\"Update\">";

              webpage+="</form></br>";
              webpage+="<a href=\"login\">Login test</a></br>";
              webpage+="<a href=\"eventTest\">Platform event test</a></br>";

              
              webpage+="</body></html>";
  
  server.send(200, "text/html", webpage); // Send a response to the client asking for input
  
}

void ShowClientResponse() {
  String webpage;
  webpage =  "<html>";
   webpage += "<head><title>ESP8266 Input Example</title>";
    webpage += "<style>";
     webpage += "body { background-color: #E6E6FA; font-family: Arial, Helvetica, Sans-Serif; Color: blue;}";
    webpage += "</style>";
   webpage += "</head>";
   webpage += "<body>";
    webpage += "<h1><br>ESP8266 Server - This was what the client sent</h1>";
    webpage += "<p>Response received was: " + Clients_Response + "</p>";
   webpage += "</body>";
  webpage += "</html>";
  server.send(200, "text/html", webpage); // Send a response to the client asking for input
}





