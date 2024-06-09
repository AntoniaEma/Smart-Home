#include <Ethernet.h> 
#include <MySQL_Connection.h> 
#include <MySQL_Cursor.h> 
#include <DHT.h>
byte mac_addr[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; 
IPAddress server_addr(104,155,173,175);  
char user[] = "nume";              
char password[] = "nume";      

#define LED_SUFRAGERIE 2
#define LED_DORMITOR 3
#define LED_BAIE 4
#define LED_BUCATARIE 5
#define LED_USA 6
#define LED_CENTRALA 9
#define DHTPIN 12
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

float temperatura_ambientala=29.0;

const char QUERY_POP[] = "SELECT * FROM hihome.setari"; 
char query[128]; 
EthernetClient client; 
MySQL_Connection conn((Client *)&client); 

  void setup() { 
    Serial.begin(9600); 
     
    pinMode(LED_BUCATARIE, OUTPUT);
    pinMode(LED_SUFRAGERIE, OUTPUT);
    pinMode(LED_DORMITOR, OUTPUT);
    pinMode(LED_BAIE, OUTPUT);
    pinMode(LED_USA, OUTPUT);
    pinMode(LED_CENTRALA, OUTPUT);
    Serial.println("Citire senzor DHT11:");
    dht.begin();
  }

  void lumina_camera(int camera, int intensitate){
    if(intensitate)
      digitalWrite(camera, HIGH);
    else
      digitalWrite(camera, LOW);
  }

  void usa(bool stare_incuietoare){
      if (stare_incuietoare)
          digitalWrite(LED_USA, HIGH);
      else
          digitalWrite(LED_USA, LOW);
  }

  void setare_temperatura(float temperatura_setata){
      temperatura_ambientala=temperatura_setata;
  }

  void termostat(int x){
  
    if(x<temperatura_ambientala){
    

    Serial.println(" da");
      digitalWrite(LED_CENTRALA, HIGH);
      
    }
    else{
      Serial.println("nu");
      digitalWrite(LED_CENTRALA, LOW);
    }
  }

  void loop() { 
    
    delay(1000);
    int v[8];
    while (!Serial); // wait for serial port to connect 
    Ethernet.begin(mac_addr);
    Serial.println("Connecting..."); 
    if (conn.connect(server_addr, 3306, user, password)) { 
      delay(1000); 
      Serial.println("> Running SELECT with dynamically supplied parameter"); 
    
      MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn); 
      
      sprintf(query, QUERY_POP, 9000000); 
      
      cur_mem->execute(query); 
      
      column_names *cols = cur_mem->get_columns(); 
      for (int f = 0; f < cols->num_fields; f++) { 
        Serial.print(cols->fields[f]->name); 
        if (f < cols->num_fields-1) { 
          Serial.print(','); 
        } 
      } 
      Serial.println(); 
      row_values *row = NULL; 
      do { 
        row = cur_mem->get_next_row(); 
        if (row != NULL) { 
          for (int f = 0; f < cols->num_fields; f++) { 
            int i = atoi(row->values[f]);
            v[f]=i;
            Serial.print(i); 
            if (f < cols->num_fields-1) { 
              Serial.print(','); 
            } 
          } 
        Serial.println(); 
        } 
      } while (row != NULL);
      
      delete cur_mem; 
    } 
    else 
      Serial.println("Connection failed.");  
    
    setare_temperatura( v[1]);
    termostat(v[7]);
    usa(v[2]);
    lumina_camera(LED_SUFRAGERIE, v[3]);
    lumina_camera(LED_DORMITOR, v[4]);
    lumina_camera(LED_BAIE, v[5]);
    lumina_camera(LED_BUCATARIE, v[6]);
    
  }
