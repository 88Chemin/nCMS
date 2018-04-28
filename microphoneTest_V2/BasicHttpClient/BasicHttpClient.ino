/**
 * BasicHTTPClient.ino
 *
 *  Created on: 24.05.2015
 *
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <WebSocketsClient.h>
#include <Hash.h>

#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

//global variables
boolean recordData=false;
String smg="";
String srvrAddr="";
String myIP="";
//

void webSocketEvent(WStype_t type, uint8_t *payload,size_t length){
  switch(type){
  case WStype_DISCONNECTED:
    USE_SERIAL.printf("[Wsc] Disconnected!\n");
    break;
  case WStype_CONNECTED:{
    USE_SERIAL.printf("[WSc] Connected to url: %s\n",payload);
    String mg="sconnected;"+String(myIP)+";sceptre1";
    webSocket.sendTXT(mg);
    }
    break;
  case WStype_TEXT:
      USE_SERIAL.printf("[WSc] get text %s\n",payload);
      smg=(char*)payload;
      if (smg=="startDAQ"){
        recordData=true;
      }
      if (smg=="stopDAQ"){
        recordData=false;
      }
      break;
  case WStype_BIN:
      USE_SERIAL.printf("[WSc] get binary lenght: %u\n",length);
      hexdump(payload,length);
      break;
  } 
}

void start_ws(String ws_ip){
  Serial.print("Starting websocket at:");
  Serial.println(ws_ip);
  webSocket.begin(ws_ip,81);
  //EventHandler
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(5000);
  Serial.println("Websocket started");
  srvrAddr=ws_ip;
}


void setup() {

    USE_SERIAL.begin(9600);
   // USE_SERIAL.setDebugOutput(true);

    USE_SERIAL.println();
    USE_SERIAL.println();
    USE_SERIAL.println();

    for(uint8_t t = 4; t > 0; t--) {
        USE_SERIAL.printf("[SETUP] WAIT %d...\n", t);
        USE_SERIAL.flush();
        delay(1000);
    }

    WiFi.mode(WIFI_STA);
    WiFiMulti.addAP("SFR-ee08","doesntmatter");

    Serial.println();Serial.println();
    Serial.print("Wait for WiFi... ");
        
    while(WiFiMulti.run() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
    }
        
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");   //Serial.println(WiFi.localIP());
    IPAddress ipA = WiFi.localIP();
    myIP = String(ipA[0]) + "." + String(ipA[1])  + "." + String(ipA[2])  + "." + String(ipA[3]) ;
    Serial.println(myIP);
    start_ws("192.168.0.22");
    

}

void loop() {
    // wait for WiFi connection
    if((WiFiMulti.run() == WL_CONNECTED)) {

        HTTPClient http;

        USE_SERIAL.print("[HTTP] begin...\n");
        // configure traged server and url
        //http.begin("https://192.168.0.20/test.html", "7a 9c f4 db 40 d3 62 5a 6e 21 bc 5c cc 66 c8 3e a1 45 59 38"); //HTTPS
        http.begin("http://192.168.0.20/test.html"); //HTTP

        USE_SERIAL.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();

        // httpCode will be negative on error
        if(httpCode > 0) {
            // HTTP header has been send and Server response header has been handled
            USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {
                String payload = http.getString();
                USE_SERIAL.println(payload);
            }
        } else {
            USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }

        http.end();
    }

    delay(10000);
}

