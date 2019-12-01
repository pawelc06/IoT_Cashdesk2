#include <FS.h> //this needs to be first, or it all crashes and burns...
#include "SPIFFS.h"
#include <ArduinoJson.h>          // https://github.com/bblanchon/ArduinoJson
#include "file_manager.h"

int loadParamsFromFile(char *sfdcLogin,char *sfdcPassword,char *sfdcSecToken,char *sfdcOAuthClientId,char *sfdcClientSecret){
  //read configuration from FS json
  Serial.println("mounting FS...");

  if (SPIFFS.begin())
  {
    Serial.println("mounted file system");
    //bool formatted = SPIFFS.format();
    if (SPIFFS.exists("/config.json"))
    {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile)
      {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        Serial.println(buf.get());
        DynamicJsonDocument doc1(1024);
        //JsonObject &json = jsonBuffer.parseObject(buf.get());
        DeserializationError error =  deserializeJson(doc1, buf.get());


        
        if (!error)
        {
          //doc1.printTo(Serial);
          serializeJson(doc1, Serial);

          Serial.println("\nparsed json");
          strcpy(sfdcLogin, doc1["sfdcLogin"]);
          strcpy(sfdcPassword, doc1["sfdcPassword"]);
          strcpy(sfdcSecToken, doc1["sfdcSecToken"]);
          strcpy(sfdcOAuthClientId, doc1["sfdcOAuthClientId"]);
          strcpy(sfdcClientSecret, doc1["sfdcClientSecret"]);
        }
        else
        {
          Serial.println("failed to load json config");

          return -2;
        }
      }
    } else {
      Serial.println("File does not exist!");
    }
  }
  else
  {
    Serial.println("failed to mount FS");
    Serial.println("Formattinf SPIFSS...");
    bool formatted = SPIFFS.format();
    return -1;
  }
  //end read
  return 0;
}

int saveParamsToFile(String sfdcLogin,String sfdcPassword,String sfdcSecToken,String sfdcOAuthClientId,String sfdcClientSecret){
    
    Serial.println("saving config");
    if (SPIFFS.begin()){
        Serial.println("mounted file system");
        //if (SPIFFS.exists("/config.json"))        {
            DynamicJsonDocument doc(1024);
            //JsonObject &json = doc.createObject();
            doc["sfdcLogin"] = sfdcLogin;
            doc["sfdcPassword"] = sfdcPassword;
            doc["sfdcSecToken"] = sfdcSecToken;
            doc["sfdcOAuthClientId"] = sfdcOAuthClientId;
            doc["sfdcClientSecret"] = sfdcClientSecret;

            File configFile = SPIFFS.open("/config.json", "w");
            if (!configFile)
            {
                Serial.println("failed to open config file for writing");
                return -1;
            }


            Serial.println("********* writing the following:");
            //json.printTo(Serial);
            serializeJson(doc, Serial);

            //json.printTo(configFile);
            serializeJson(doc, configFile);
            configFile.close();
            //end save
            return 0;
      //  }
    }
}