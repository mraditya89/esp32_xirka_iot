#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);

// TODO 
// 1. Setup Wifi config
// 2. Setup mqtt client config
// 3. Setup mqtt broker config
// 4. Change device & topic switch

// 1. WIFI CONFIG
const char* SSID = "<ssid>";
const char* SSID_PASSWORD = "<password>";

// 2. MQTT CLIENT CONFIG
char CLIENT[]= "<client>";
char CLIENT_PASSWORD[]="<client_password>";

// 3. MQTT BROKER CONFIG
const char* BROKER_SERVER = "<broker_ip>";
const int BROKER_PORT = <broker_port>;

// 4. DEVICE
const char* DEVICE = "<device_name>";
const char* TOPIC_SWITCH = "<device_name>/switch";


// MISC
long lastMsg = 0;

// PAYLOAD 
char payload[512];

// LED PIN
volatile byte ledState = LOW;

void setup() {
  Serial.begin(115200);
  setupWifi();
  client.setServer(BROKER_SERVER, BROKER_PORT);
  client.setCallback(mqttCallback);
  pinMode(LED_BUILTIN, OUTPUT);
}

void setupWifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(SSID);

  WiFi.begin(SSID, SSID_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void mqttCallback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println(messageTemp);

  if (String(topic) == TOPIC_SWITCH) {
    Serial.print("Changing output to ");
    if(messageTemp == "{\"payload\":{\"message\":\"on\"}}"){
      Serial.println("on");
      ledState = HIGH;
    }
    else if(messageTemp == "{\"payload\":{\"message\":\"off\"}}"){
      Serial.println("off");
      ledState = LOW;
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("esp32client", CLIENT, CLIENT_PASSWORD)) {
      Serial.println("connected");
      // Subscribe
      Serial.println("Subscribe");
      Serial.println(TOPIC_SWITCH);
      client.subscribe(TOPIC_SWITCH);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  // TURN ON / OFF LED   
  digitalWrite(LED_BUILTIN, ledState);
  
  // TRY TO CONNECT TO BROKER 
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  // HANDLER MESSAGE
  long now = millis();
  if (now - lastMsg > 10000) {
    lastMsg = now;

    int temperature = random(0, 30);
    int humidity = random(0, 20);
    sprintf(payload,"{\"payload\":{\"humidity\":%d, \"temperature\":%d}}", humidity, temperature);

    Serial.println(payload);
    client.publish(DEVICE, payload);
  }
}

//{"payload":{"message":"off"}}
