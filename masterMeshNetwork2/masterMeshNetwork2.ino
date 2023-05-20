// root code

#include "painlessMesh.h"
#include <HTTPClient.h>
#include <iostream>
#include <string.h>
#include <WiFi.h>
#include <AsyncTCP.h>

#define   MESH_PREFIX     "Mesh_username"
#define   MESH_PASSWORD   "mesh_password"
#define   MESH_PORT       5555

const char* ssid = "Galaxy Note2073f7";
const char* password = "peae8479";
const char* serverUrl = "http://192.168.69.28:10/receive-data"; // Replace with your server URL

//const char* ssid = "HOTBOX 4-4E78-5GHz";
//const char* password = "DV29QJWG3KN9";
//const char* host = "192.168.181.1";
//const char* host = WiFi.localIP().toString().c_str(); // pc ip
const uint16_t PORT = 8820;
String message = "";

Scheduler userScheduler;
painlessMesh mesh;

// functions declerations
void sendmsg() ;

Task taskSendmsg( TASK_SECOND * 1 , 3, &sendmsg);


void sendmsg() {
  String msg = "hey i recieved your msg couse im the root , my id is: ";
  msg += mesh.getNodeId();
 
  uint32_t dest_id = mesh.getNodeList().back();
  mesh.sendSingle(dest_id, msg);
  Serial.printf("send successful\n");
 
  taskSendmsg.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
 
}


void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("Received from %u: a message that contain: %s\n", from, msg.c_str());
  message = msg;
  taskSendmsg.enable();
  //ESP.restart();
  sendMsgToServer(msg);
}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void setup() {
  Serial.begin(115200);
 

  
  // mesh network part
  mesh.setDebugMsgTypes( ERROR | STARTUP  );  

  mesh.init( MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT );
 
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.onReceive(&receivedCallback);

  // set a root
  Serial.printf("I am root?: %d \n", mesh.isRoot());
  mesh.setRoot();
  Serial.printf("I am root?: %d\n", mesh.isRoot());

  userScheduler.addTask( taskSendmsg );
  taskSendmsg.enable();

}

void loop() {
  mesh.update();
}


void sendMsgToServer(String &msg)
{
  /*
  HTTPClient http;
    // Make a POST request to the server
  http.begin(serverUrl);
  http.addHeader("Content-Type", "text/plain");
  uint8_t* uint8Ptr = reinterpret_cast<uint8_t*>(const_cast<char*>(msg.c_str()));
  int httpResponseCode = http.POST(uint8Ptr, msg.length());

  if (httpResponseCode > 0) {
    Serial.print("HTTP response code: ");
    Serial.println(httpResponseCode);
  } else {
    Serial.println("Error sending data to server");
  }

  http.end();
  */
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);

    // Example data to send
    String data = "{\"node_id\": 1, \"temperature\": 25, \"humidity\": 60}";

    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST(data);
    while (httpResponseCode <= 0)
    {
      httpResponseCode = http.POST(data);
      Serial.println("Error sending message");
    }
    Serial.print("HTTP response code: ");
    Serial.println(httpResponseCode);
    /*   
    if (httpResponseCode > 0) {
      Serial.print("HTTP response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.println("Error sending message");
    }
    */
    http.end();
  }
}

