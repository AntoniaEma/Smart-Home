#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <ESP8266WiFi.h>
#include "DHT.h"

#ifndef STASSID
#define STASSID "AIA"
#define STAPSK "UPT-aia!"
#endif

#define DHTPIN 4    
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

const char* ssid = STASSID;
const char* wifiPassword = STAPSK;
IPAddress server_addr(104,155,173,175); 
char user[] = "nume"; 
char password[] = "nume"; 

WiFiClient client;
MySQL_Connection conn((Client *)&client);
char INSERT_SQL[] = "UPDATE hihome.setari SET Temperatura_ESP = '%.2f' WHERE ID=1";
char query[128];

void setup() {
    Serial.begin(9600);
    // We start by connecting to a WiFi network
    dht.begin();
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
    would try to act as both a client and an access-point and could cause
    network-issues with your other WiFi-devices on your WiFi-network. */
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, wifiPassword);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    //MySql section
    Serial.println("Connecting...");
}

void sendData(float t){
 
  
    Serial.println("Recording data.");
    
    MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
    sprintf(query,INSERT_SQL,t);
    Serial.println(query);
    cur_mem->execute(query);
  
    delete cur_mem; 
}

void loop(){
  if (conn.connect(server_addr, 3306, user, password)) {
        float t = dht.readTemperature();
        sendData(t);
        delay(1000);
    }
    else
        Serial.println("Connection failed.");
 
  conn.close();
  
  delay(5000);
}
