#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <TimeLib.h> 

/* 1. Define the WiFi credentials */
#define WIFI_SSID "XXXXX"
#define WIFI_PASSWORD "XXXXX"

/* 2. Define the API Key */
#define API_KEY "XXXXX-XXXXX"

/* 3. Define the project ID */
#define FIREBASE_PROJECT_ID "water-alarm-swarnagouri"

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "XXXXX"
#define USER_PASSWORD "XXXXX"

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

String uid;

String path;

String firebaseStatus = "on";

// Water alarm
const int FloatSensor = 2; // Replace with the actual pin for your FloatSensor
int buttonState = 1;
int count = 0;
String tankStatus = "UNKNOWN";
String lastFilledTime = "N/A";
String lastFilledDate = "N/A";
String lastFilledDay = "N/A";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "time.google.com", 19800); // 19800 is the Indian Time Zone offset in seconds (5 hours and 30 minutes)

void setup()
{
    Serial.begin(115200);
    pinMode(FloatSensor, INPUT);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

    /* Assign the api key (required) */
    config.api_key = API_KEY;

    /* Assign the user sign in credentials */
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;

    /* Assign the callback function for the long running token generation task */
    config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

    Firebase.begin(&config, &auth);
    
    Firebase.reconnectWiFi(true);
    Serial.println("Getting User UID");
    while ((auth.token.uid) == "") {
      Serial.print('.');
      delay(1000);
    }

    // Print user UID
    uid = auth.token.uid.c_str();
    Serial.print("User UID: ");
    Serial.println(uid);  

    // Configure NTP time synchronization
    timeClient.begin();
    timeClient.setTimeOffset(19800); // Set the time zone offset
}


void getDay(){
  String day = String(timeClient.getDay());
  lastFilledDay = day;
}

void getDate() {
  time_t epochTime = timeClient.getEpochTime();

  struct tm *timeinfo;
  timeinfo = localtime(&epochTime);
  
  int day = timeinfo->tm_mday;
  int month = timeinfo->tm_mon + 1; // tm_mon is 0-based
  int year = timeinfo->tm_year + 1900; // Years since 1900

  int hour = timeinfo->tm_hour;
  int minute = timeinfo->tm_min;
  int second = timeinfo->tm_sec;

  Serial.print("IST Date and Time: ");
  Serial.print(day);
  Serial.print("/");
  Serial.print(month);
  Serial.print("/");
  Serial.print(year);
  Serial.print(" ");
  Serial.print(hour);
  Serial.print(":");
  Serial.print(minute);
  Serial.print(":");
  Serial.println(second);

  char buffer[11];
  sprintf(buffer, "%02d-%02d-%04d", day, month, year);
  lastFilledDate = buffer;
}


void updateFirebase(){
  // Save the tank status and last_filled time in the "water_tank" collection
  FirebaseJson content;
  content.set("fields/status/stringValue", tankStatus);
  content.set("fields/time/stringValue", lastFilledTime);
  content.set("fields/day/stringValue", lastFilledDay);
  content.set("fields/date/stringValue", lastFilledDate);

  String path = "water_tank/tank"; // Collection: "water_tank", Document ID: "tank"

  Serial.print("Updating document... ");
  if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", path.c_str(), content.raw(), "", "", "")) {
    Serial.println("ok");
    Serial.println(fbdo.payload().c_str());
  } else {
    Serial.println(fbdo.errorReason());
  }
}


void loop()
{  
  // Determine water tank status based on FloatSensor state
  buttonState = digitalRead(FloatSensor); 
  Serial.println(buttonState);
  if (buttonState == 0 && tankStatus!="HIGH")
  {
    Serial.println( "WATER LEVEL - HIGH");
    tankStatus = "HIGH";
    // Updating Last filled time
    timeClient.update();
    lastFilledTime = timeClient.getFormattedTime();
    getDate();
    getDay();
    updateFirebase();
  }
  else if(buttonState == 1 && tankStatus!="LOW")
  {
    Serial.println( "WATER LEVEL - LOW" );
    tankStatus = "LOW";

    updateFirebase();
  } 
  delay(1000);
}
