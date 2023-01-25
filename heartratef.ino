
uint32_t delayMS;
#include <WiFi.h>
#include <WiFiClient.h>
#include <PubSubClient.h> 

//  Variables
int PulseSensorPurplePin = 34;       
int LED13 = 2;  
int Signal;               
int Threshold = 2550;          

const char* ssid = "Boys Hostel 4th floor(2)";
const char* password = "123456789";

#define ORG "oi3e1z"
#define DEVICE_TYPE "esp32"
#define DEVICE_ID "device1"
#define TOKEN "B+4y(iPhpa0aAm(pR)"



char server[] = ORG ".messaging.internetofthings.ibmcloud.com";
char pubTopic[] = "iot-2/evt/status/fmt/json";
char subTopic[] = "iot-2/cmd/test/fmt/String";
char authMethod[] = "use-token-auth";
char token[] = TOKEN;
char clientId[] = "d:" ORG ":" DEVICE_TYPE ":" DEVICE_ID;

WiFiClient wifiClient;
PubSubClient client(server, 1883, NULL, wifiClient);

void receivedCallback(char* pubTopic, byte* payload, unsigned int length) {
  Serial.print("Message received: ");
  Serial.println(pubTopic);

  Serial.print("payload: ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  /* we got '1' -> on */
}


void setup() {
   pinMode(LED13,OUTPUT);        
   Serial.begin(115200);         
   Serial.println("IBM Watson IoT ESP32 Pulse Monitor Amped"); // just a funky text
   Serial.print("Connecting to "); 
    Serial.print(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    } 
    Serial.println("");
    
    Serial.print("WiFi connected, IP address: "); 
    Serial.println(WiFi.localIP());

    if (!client.connected()) {
        Serial.print("Reconnecting client to ");
        Serial.println(server);
        while (!client.connect(clientId, authMethod, token)) {
            Serial.print(".");
            delay(500);
        }
        client.setCallback(receivedCallback);
        if (client.subscribe(subTopic)) {
            Serial.println("subscribe to cmd OK");
        } else {
            Serial.println("subscribe to cmd FAILED");
        }
        Serial.println("IBM Watson IoT connected");
    }
}

long lastMsg = 0;
long pulse = 0;

// The Main Loop Function
void loop() {

  Signal = analogRead(PulseSensorPurplePin);  
                                              
   if(Signal > Threshold){                        
     digitalWrite(LED13,HIGH);
   } else {
     digitalWrite(LED13,LOW);              

    client.loop();
    long now = millis();
    if (now - lastMsg > 3000) {
        lastMsg = now;
        pulse = (Signal);
        
        String payload = "{\"d\":{\"Name\":\"" DEVICE_ID "\"";
               payload += ",\"pulse\":";
               payload += pulse;
               payload += "}}";
        Serial.print("Sending payload: ");
        Serial.println(payload);
  // Serial.println(Signal);                    
        if (client.publish(pubTopic, (char*) payload.c_str())) {
            Serial.println("Publish ok");
        } else {
            Serial.println("Publish failed");
        }
    }



delay(10);


}
