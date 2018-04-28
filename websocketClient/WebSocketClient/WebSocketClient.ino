/*
 * WebSocketClient.ino
 *
 *  Created on: 24.05.2015
 *
 */

#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <WebSocketsClient.h>

#include <Hash.h>

ESP8266WiFiMulti WiFiMulti;
WebSocketsClient webSocket;

#define USE_SERIAL Serial

//global variables
boolean DAQon=false;
String smg="";
String srvrAddr="";
String myIP="";
//global variables for microphone
int i_loop=0;
int meanSum=0;
int peakValue=0;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

	switch(type) {
		case WStype_DISCONNECTED:
			USE_SERIAL.printf("[WSc] Disconnected!\n");
			break;
		case WStype_CONNECTED: {
			USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);

			// send message to server when Connected
      String mg = "sconnected;" + String(myIP) +";sceptre1";
			webSocket.sendTXT(mg);
		}
			break;
		case WStype_TEXT:
			USE_SERIAL.printf("[WSc] get text: %s\n", payload);

			// send message to server
			// webSocket.sendTXT("message here");
			break;
		case WStype_BIN:
			USE_SERIAL.printf("[WSc] get binary length: %u\n", length);
			hexdump(payload, length);

			// send data to server
			// webSocket.sendBIN(payload, length);
			break;
	}

}

void start_ws(String ws_ip){
// server address, port and URL
  webSocket.begin(ws_ip, 81, "/");
  Serial.println("Connecting to websocket at IP:");
  Serial.println(ws_ip);
  String mg = "sconnected;" + String(myIP) +";sceptre1";
  webSocket.sendTXT(mg);
  // event handler
  webSocket.onEvent(webSocketEvent);

  // use HTTP Basic Authorization this is optional remove if not needed
  //webSocket.setAuthorization("user", "Password");

  // try ever 5000 again if connection has failed
  webSocket.setReconnectInterval(5000);
  Serial.println("Websocket started");
  srvrAddr=ws_ip;
}

void connect_wifi(){
  //WiFiMulti.addAP("SFR-ee08", "doesntmatter");
    WiFiMulti.addAP("HomeX", "1223334444");
  //WiFi.disconnect();
  while(WiFiMulti.run() != WL_CONNECTED) {
    printf(".");
    delay(100);
  }
  start_ws("192.168.43.137");
  
}

void setup() {
	// USE_SERIAL.begin(921600);
	USE_SERIAL.begin(9600);

	//Serial.setDebugOutput(true);
	USE_SERIAL.setDebugOutput(true);

	USE_SERIAL.println();
	USE_SERIAL.println();
	USE_SERIAL.println();

	for(uint8_t t = 4; t > 0; t--) {
		USE_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
		USE_SERIAL.flush();
    connect_wifi();
		delay(1000);
	}

 pinMode(LED_BUILTIN, OUTPUT);

	
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");   //Serial.println(WiFi.localIP());
    IPAddress ipA = WiFi.localIP();
    myIP = String(ipA[0]) + "." + String(ipA[1])  + "." + String(ipA[2])  + "." + String(ipA[3]) ;
    Serial.println(myIP);
    
}
 

void sendData(int &value){
  String header="data;sceptre1;";
  String data1=String(value);
  String message=header+data1;
  webSocket.sendTXT(message);
  //delay(100);
}



void loop() {
	
  //sendData(100);
  int sensorValue=analogRead(A0);
  //int sensorValue=255;
  sensorValue-=0;
  i_loop++;
  meanSum+=sensorValue;
  if(sensorValue>peakValue) peakValue=sensorValue;
  if(i_loop>100){
    Serial.println(peakValue);
    sendData(peakValue);
    i_loop=0;
    meanSum=0;
    peakValue=0;
    
    //sendData(sensorValue);
  }
  webSocket.loop();
  if(WiFi.status()!=WL_CONNECTED){connect_wifi();}
  delay(1);

 
}
