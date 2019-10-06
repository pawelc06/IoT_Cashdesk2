#include <Arduino.h>
#include <ArduinoJson.h>
//#include <ESP8266WiFiMulti.h>
//#include <ESP8266HTTPClient.h>
#include <WiFi.h>
#include <WiFiMulti.h>

#include <HTTPClient.h>

#define USE_SERIAL Serial



int loginAndGetToken(String user, String password, char* instanceURL,char* token,String securityToken,String clientId,String clientSecret);
int createCustomerVisitEvent(char *customerNumber,char *productCode,bool checkout, String instanceURL,String token);