// MY CONNECTION TO 802.1x NETWORK IS BASED
// ON MY MAC ADDRESS SO I NEEDED TO UPDATE IT MANUALLY
#include <ESP8266WiFi.h>
#include <Arduino_JSON.h>
#include <ESP8266HTTPClient.h>
#include <time.h>
WiFiServer server(80);
String header;
extern "C" {
#include "user_interface.h"
#include "wpa2_enterprise.h"
}

// Your Domain name with URL path or IP address with path
String openWeatherMapApiKey = "5d12f945551054f701e34b811effb568";


// Replace with your country code and city
String city = "Bauru";
String countryCode = "BR";
const char* Timezone  = "<-03>3,M3.5.0/02,M10.5.0/01";  // Choose your time zone from: https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv 
const char* ntpServer = "br.pool.ntp.org";                  // Or, choose a time server close to you, but in most cases it's best to use pool.ntp.org to find an NTP server
                                                           // then the NTP system decides e.g. 0.pool.ntp.org, 1.pool.ntp.org as the NTP syem tries to find  the closest available servers
                                                           // EU "0.europe.pool.ntp.org"
                                                           // US "0.north-america.pool.ntp.org"
                                                           // See: https://www.ntppool.org/en/        
int  gmtOffset_sec = -10800;    // UK normal time is GMT, so GMT Offset is 0, for US (-5Hrs) is typically -18000, AU is typically (+8hrs) 28800
int  daylightOffset_sec = 3600; // In the UK DST is +1hr or 3600-secs, other countries may use 2hrs 7200 or 30-mins 1800 or 5.5hrs 19800 Ahead of GMT use + offset behind - offset
String CurrentTime, CurrentDate = "";
bool display_EU = true;
// THE DEFAULT TIMER IS SET TO 10 SECONDS FOR TESTING PURPOSES
// For a final application, check the API call limits per hour/minute to avoid getting blocked/banned
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 10 seconds (10000)
unsigned long timerDelay = 10000;

String jsonBuffer;



static const char* ssid = "eduroam";
static const char* username = "@unesp.br";
static const char* password = "";
char buff[20];
String ip;

uint8_t mac[6] {0x30, 0xAE, 0xA4, 0x0F, 0x89, 0x78};



void setup() {
  // put your setup code here, to run once:
 pinMode(LED_BUILTIN, OUTPUT);
	
ESP.wdtEnable(10000);



  Serial.begin(115200);
  Serial.println("Free memory before setting wifi connection to unesp");
  Serial.println(system_get_free_heap_size());
  delay(500);
  Serial.print("OLD ESP8266 MAC: ");
  Serial.println(WiFi.macAddress()); //This will read MAC Address of ESP
  wifi_set_macaddr(0, const_cast<uint8*>(mac));   //This line changes MAC adderss of ESP8266
  Serial.print("NEW ESP8266 MAC: ");
  Serial.println(WiFi.macAddress()); //This will read MAC Address of ESP
  wifi_set_opmode(STATION_MODE);
  struct station_config wifi_config;
  memset(&wifi_config, 0, sizeof(wifi_config));
  strcpy((char*)wifi_config.ssid, ssid);
  wifi_station_set_config(&wifi_config);
  wifi_station_clear_cert_key();
  wifi_station_clear_enterprise_ca_cert();
  wifi_station_set_wpa2_enterprise_auth(1);
  wifi_station_set_enterprise_identity((uint8*)username, strlen(username));
  wifi_station_set_enterprise_username((uint8*)username, strlen(username));
  wifi_station_set_enterprise_password((uint8*)password, strlen(password));
  wifi_station_connect();
  Serial.println();
  Serial.println("Waiting for connection and IP Address from DHCP");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Free memory during connecting to unesp wifi");
    Serial.println(system_get_free_heap_size());
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Free memory after connection sucessful");
  Serial.println(system_get_free_heap_size());
  server.begin();
  SetupTime();
  UpdateLocalTime();
}

void loop(){




 // Send an HTTP GET request
 	
ESP.wdtFeed();

  if ((millis() - lastTime) > timerDelay) {
    // Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey;
      
      jsonBuffer = httpGETRequest(serverPath.c_str());
      Serial.println(jsonBuffer);
      JSONVar myObject = JSON.parse(jsonBuffer);
  
      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined") {
        Serial.println("Parsing input failed!");
        digitalWrite(LED_BUILTIN, LOW);
        delay(11000);
        return;
      }
    
      Serial.print("JSON object = ");
      Serial.println(myObject);
      Serial.print("Temperature: ");
      Serial.println(myObject["main"]["temp"]);
      Serial.print("Pressure: ");
      Serial.println(myObject["main"]["pressure"]);
      Serial.print("Humidity: ");
      Serial.println(myObject["main"]["humidity"]);
      Serial.print("Wind Speed: ");
      Serial.println(myObject["wind"]["speed"]);
      UpdateLocalTime();
      digitalWrite(LED_BUILTIN, LOW);
      delay(25);
      digitalWrite(LED_BUILTIN, HIGH);
    }
    else {
      Serial.println("WiFi Disconnected");
      digitalWrite(LED_BUILTIN, LOW);
      delay(11000); //delay to wachdog reset
    }
    lastTime = millis();
  }
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(client, serverName);
  
  // Send HTTP POST request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
    digitalWrite(LED_BUILTIN, LOW);
    delay(11000); //delay to wachdog reset
  }
  // Free resources
  http.end();

  return payload;
}





boolean SetupTime() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer, "time.nist.gov"); //(gmtOffset_sec, daylightOffset_sec, ntpServer)
  setenv("TZ", Timezone, 1);  //setenv()adds the "TZ" variable to the environment with a value TimeZone, only used if set to 1, 0 means no change
  tzset(); // Set the TZ environment variable
  delay(2000);
  bool TimeStatus = UpdateLocalTime();
  return TimeStatus;
}

boolean UpdateLocalTime() {
  struct tm timeinfo;
  time_t now;
  time(&now);
  //See http://www.cplusplus.com/reference/ctime/strftime/
  char output[30];
  if (display_EU == true) {
    strftime(output, 30, "%d/%m/%y", localtime(&now));
    CurrentDate = output;
    strftime(output, 30, "%H:%M:%S", localtime(&now));
    CurrentTime = output;
  }
  else { 
    strftime(output, 30, "%m/%d/%y", localtime(&now));
    CurrentDate = output;
    strftime(output, 30, "%r", localtime(&now));
    CurrentTime = output;
  }
  Serial.println(CurrentTime);
  Serial.println(CurrentDate);
  return true;
}
