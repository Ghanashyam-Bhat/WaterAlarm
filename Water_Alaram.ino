//https://www.gadgetronicx.com/interfacing-float-sensor-arduino/#:~:text=The%20float%20sensor%20consists%20of%20two%20terminals%2C%20VCC%20and%20GND,either%20being%20high%20or%20low.

//https://randomnerdtutorials.com/telegram-esp8266-nodemcu-motion-detection-arduino/

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// Replace with your network credentials
const char* ssid = "XXXX";
const char* password = "XXXX";
// Initialize Telegram BOT
#define BOTtoken "XXXX:XXXX"
#define CHAT_ID "XXXX"
#define CHAT_ID_ATTE "XXXX" 
#define CHAT_ID_MAVA "XXXX"

X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

const int FloatSensor = 14;
int buttonState = 1;
int count = 0;

void setup() {
  Serial.begin(115200);
  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org

  pinMode(FloatSensor, INPUT_PULLUP);

  // Attempt to connect to Wifi network:
  Serial.print("Connecting Wifi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  bot.sendMessage(CHAT_ID, "Bot started up", "");
}


void sendTelegramMessage()
{
  String message = "WATER IS FULL";

  if(bot.sendMessage(CHAT_ID, message, "Markdown"))
  {
    bot.sendMessage(CHAT_ID_ATTE, message, "Markdown");
    bot.sendMessage(CHAT_ID_MAVA, message, "Markdown");
    Serial.println("TELEGRAM Successfully sent");
  }
}

void loop() {
  buttonState = digitalRead(FloatSensor); 


  //---------------------------

  if (buttonState == 0)
    {
      count ++;Serial.println( "WATER LEVEL - HIGH");
    }
    else
    {
      count = 0;
      Serial.println( "WATER LEVEL - LOW" );
    }
    if (count!=0 && count<4)
    {
      sendTelegramMessage();
    }

  
}
