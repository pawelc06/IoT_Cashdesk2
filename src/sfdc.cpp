#include "sfdc.h"
#include "cacert.h"

//extern ESP8266WiFiMulti WiFiMulti;
HTTPClient http;



int loginAndGetToken(String user, String password, char* instanceURL,char* token,String securityToken,String clientId,String clientSecret){
  WiFiClientSecure client;
  StaticJsonDocument<1024> doc;
   byte lineCounter=0;
  String line;
  String headers;
  bool successFlag;

  
  String loginPayload = "grant_type=password&client_id="+clientId+"&client_secret="+clientSecret+"&username=" + user+"&password="+password+securityToken;
  const char host[]="login.salesforce.com";
  String path = "/services/oauth2/token";

  //strcpy(host,"login.salesforce.com");
  
  if((WiFi.status() == WL_CONNECTED)) {

        

        USE_SERIAL.print("[HTTP] begin...\n");

        USE_SERIAL.printf("Connecting to host: %s\n",host);

          int res = client.connect(host, 443);

          
         
          if (!res) {
            USE_SERIAL.printf("connection failed\n");
            return -1;
          }
          

         
          USE_SERIAL.printf("Sending POST request...\n");
         
          Serial.println(loginPayload);

          client.print(String("POST ") + path + " HTTP/1.1\r\n" +
          "Host: " + host + "\r\n" +
          "Connection: Keep-Alive\r\n" +
          "Accept: */*\r\n" +
          "User-Agent: Mozilla/4.0 (compatible; esp8266 Lua; Windows NT 5.1)\r\n" +
          "Content-Type: application/x-www-form-urlencoded\r\n" +
          "Content-Length: "+loginPayload.length()+"\r\n" +
          "\r\n" + 
          loginPayload + "\r\n");

          USE_SERIAL.printf("Done\n");
          
          unsigned long timeout = millis();
          while (client.available() == 0) {
            if (millis() - timeout > 5000) {
              USE_SERIAL.println(">>> Client Timeout !");
              client.stop();
              return -1;
            }
          }
         
          // Read all the lines of the reply from server and print them to Serial
         while (client.connected()) {
            line = client.readStringUntil('\r');
            client.readStringUntil('\n');
            headers = headers + line+"\r\n";
            if (line == "") {
              Serial.println("headers received:");
              Serial.println(headers);
              break;
            }
         }
         
         line = client.readStringUntil('\n');
         line = client.readStringUntil('\n');

         
        Serial.println("payload received:");
        Serial.println(line);

        DeserializationError error = deserializeJson(doc, line);
        if (error)
          return -1;
        const char* tokenJSON = doc["access_token"];
        const char* instanceURLJSON = doc["instance_url"];

        strcpy(token,tokenJSON);
        strcpy(instanceURL,instanceURLJSON);
        
        
        }
        return 0;
}

int createCustomerVisitEvent(char *customerNumber,char *productCode,bool checkout, String instanceURL,String token){
 StaticJsonDocument<1024> doc;
  String RESTpath = "/services/data/v41.0/sobjects/CustomerVisit__e/";
  String authHeader = "Bearer "+token;

  String endpointURL = instanceURL + RESTpath;
  WiFiClientSecure client;
  int httpCode;
  String payload;
  byte lineCounter=0;
  String line;
  String headers;
  bool successFlag;

  // Set up time to allow for certificate validation
  //configTime(2 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  configTime(0, 3600, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));

  //client.setCACert_P(caCert,caCertLen);
  client.setCACert(digicert_root_ca);


 

    if((WiFi.status() == WL_CONNECTED)) {
          String host = instanceURL.substring(8);

          //JsonObject& root = doc.createObject();
          doc["DeviceId__c"] = WiFi.macAddress();
          doc["AccountNumber__c"] = customerNumber;
          doc["ProductCode__c"] = productCode;
          doc["Checkout__c"] = checkout;
          
          
          
          //root.printTo(payload);
          serializeJson(doc, payload);
          

          USE_SERIAL.printf("Connecting to host: %s\n",host.c_str());

          int res = client.connect(host.c_str(), 443);

          
         
          if (!res) {
            USE_SERIAL.printf("connection failed\n");
            return -1;
          }
          

         
          USE_SERIAL.printf("Sending POST request...\n");
          Serial.println(customerNumber);
          Serial.println(payload);

          client.print(String("POST ") + RESTpath + " HTTP/1.1\r\n" +
          "Host: " + host + "\r\n" +
          "Connection: close\r\n" +
          "Accept: */*\r\n" +
          "User-Agent: Mozilla/4.0 (compatible; esp8266 Lua; Windows NT 5.1)\r\n" +
          "Content-Type: application/json;charset=utf-8\r\n" +
          "Authorization: Bearer "+token+"\r\n" + 
          "Content-Length: "+payload.length()+"\r\n" +
          "\r\n" + 
          payload + "\r\n");

          USE_SERIAL.printf("Done\n");
          
          unsigned long timeout = millis();
          while (client.available() == 0) {
            if (millis() - timeout > 5000) {
              USE_SERIAL.println(">>> Client Timeout !");
              client.stop();
              return -1;
            }
          }
         
          // Read all the lines of the reply from server and print them to Serial
        while (client.connected()) {
            line = client.readStringUntil('\r');
            client.readStringUntil('\n');
            headers = headers + line+"\r\n";
            if (line == "") {
              Serial.println("headers received:");
              Serial.println(headers);
              break;
            }
         }
         
         line = client.readStringUntil('\n');
         line = client.readStringUntil('\n');

         line = line + "}";

         //JsonObject& root2 = jsonBuffer.parseObject(line);
         DeserializationError error = deserializeJson(doc, line);
        if (error) {
           Serial.println("parseObject() failed");
        
        } else {
          bool successFlag = doc["success"]; 
          if(successFlag){
            const char* id = doc["id"];
            Serial.print("Event succesfully created. Id: ");
            Serial.println(id);
            
          } else {
            const char* error = doc["errors"][0];
            Serial.print("Event creation failed.Error: ");
            Serial.println(error);
          }
        }
         
         
         
         
         Serial.print(line);  
              
    }  
    return 0;
}