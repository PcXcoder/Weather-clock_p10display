#include "header.h"
#include "WifiCfg.h"
#include "TimeZoneCfg.h"
//#include "translate.h"
boolean UpdateLocalTime();
boolean SetupTime();
String httpGETRequest(const char* serverName);
String exportdata;
const char* idTranslate(int status);
int status;
const char* translated;
int datasync = 10000;
//SoftwareSerial swSer(14, 12); //Define hardware connections to uno [ SoftwareSerial(rxPin, txPin) ]

void setup(){
  // put your setup code here, to run once:
 pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
 // swSer.begin(9600);
//  Serial.println("Free memory before setting wifi connection to unesp");
//  Serial.println(system_get_free_heap_size());
  delay(500);
//  Serial.print("OLD ESP8266 MAC: ");
//  Serial.println(WiFi.macAddress()); //This will read MAC Address of ESP
  wifi_set_macaddr(0, const_cast<uint8*>(mac));   //This line changes MAC adderss of ESP8266
//  Serial.print("NEW ESP8266 MAC: ");
//  Serial.println(WiFi.macAddress()); //This will read MAC Address of ESP
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
//  Serial.println();
//  Serial.println("Waiting for connection and IP Address from DHCP");
  while (WiFi.status() != WL_CONNECTED) {
//    Serial.println("Free memory during connecting to unesp wifi");
//    Serial.println(system_get_free_heap_size());
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000);
//    Serial.print(".");
  }
//  Serial.println("");
//  Serial.println("WiFi connected");
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
//  Serial.println("IP address: ");
//  Serial.println(WiFi.localIP());
//  Serial.println("Free memory after connection sucessful");
//  Serial.println(system_get_free_heap_size());
  server.begin();
  SetupTime();
  UpdateLocalTime();


}
void loop(){	
ESP.wdtFeed();
  if ((millis() - lastTime) > timerDelay) {
    // Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      String serverPath = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "," + countryCode + "&APPID=" + openWeatherMapApiKey;
      jsonBuffer = httpGETRequest(serverPath.c_str());
      //Serial.println(jsonBuffer);
      JSONVar myObject = JSON.parse(jsonBuffer);
      // JSON.typeof(jsonVar) can be used to get the type of the var
      if (JSON.typeof(myObject) == "undefined") {
        digitalWrite(LED_BUILTIN, LOW);
        delay(11000);
        ESP.restart();
        return;
      }
      UpdateLocalTime();
      digitalWrite(LED_BUILTIN, LOW);
      delay(25);
      digitalWrite(LED_BUILTIN, HIGH);
      Tc = (int(myObject["main"]["temp"]) - 273);
      status = int(myObject["weather"][0]["id"]);
      delay(900);
      translated = idTranslate(status);
      exportdata = CurrentTime + Tc + "/" + translated;
      Serial.println(exportdata);  //print time H:M
      delay(datasync);     //garantees that the arduino is not bussy
  //    Serial.println(" "); //clear display for new update
    }
    else {
//      Serial.println("WiFi Disconnected");
      digitalWrite(LED_BUILTIN, LOW);
      delay(11000); //delay to wachdog reset
      ESP.restart();
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
    //Serial.print("HTTP Response code: ");
    //Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
//    Serial.print("Error code: ");
//    Serial.println(httpResponseCode);
    digitalWrite(LED_BUILTIN, LOW);
    delay(11000); //delay to wachdog reset
    ESP.restart();
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
    strftime(output, 30, "%H/%M/", localtime(&now));
    CurrentTime = output;
  }
  else { 
    strftime(output, 30, "%m/%d/%y", localtime(&now));
    CurrentDate = output;
    strftime(output, 30, "%r", localtime(&now));
    CurrentTime = output;
  }
  return true;
}
const char* idTranslate(int status){
    if (status >= 800) {
           if(status == 800){
      return "Ceus Limpos";
           }
           if(status == 801){
    return "Raras Nuvens";
           }
           if(status == 802){
    return "Poucas Nuvens";
           }
           if(status == 803){
    return "Levemente Nublado";
           }
           if(status == 804){
    return "Nublado";
           }}

    if(status >= 700){
           if(status == 701){
    return "Neblina";
           }
           if(status == 711){
    return "Fumaça";
           }
           if(status == 721){
    return "Névoa";
           }
           if(status == 731){
    return "Tempestade de areia";
           }
           if(status == 741){
    return "Neblina";
           }
           if(status == 751){
    return "Tempestade de areia";
           }
           if(status == 761){
    return "Empoeirado (todo dia)";
           }
           if(status == 762){
    return "Cinzas vulcanicas??";
           }
           if(status == 771){
    return "rajadas de vento";
           }
           if(status == 781){
    return "Tornado!";
           }}

    if(status >= 600){
           if(status == 600){
    return "Neve fraca?";
           }
           if(status == 601){
    return "Neve?";
           }
           if(status == 602){
    return "Neve forte?";
           }
           if(status == 611){
    return "chuva de granizo";
           }
           if(status == 612){
    return "chuva de granizo leve";
           }
           if(status == 613){
    return "chuva de granizo";
           }
           if(status == 615){
    return "chuva leve de agua e neve?";
           }
           if(status == 616){
    return "chuva pesada de agua e neve?";
           }
           if(status == 620){
    return "Nevando?";
           }
           if(status == 621){
    return "Neve?";
           }
           if(status == 622){
    return "MUITA NEVE!!!!??";
           }}


    if(status >= 500){
           if(status == 500){
    return "chuva leve";
           }
           if(status == 501){
    return "chuva moderada";
           }
           if(status == 502){
    return "chuva forte";
           }
           if(status == 503){
    return "chuva muito forte!";
           }
           if(status == 504){
    return "chuva MUITO FORTE!!";
           }
           if(status == 511){
    return "chuva de pedra";
           }
           if(status == 520){
    return "chuva leve";
           }
           if(status == 521){
    return "chuva";
           }
           if(status == 522){
    return "chuva forte";
           }
           if(status == 531){
    return "chuva irregular";
           }}

    if(status >= 300){ //check for 3xx
       if(status == 300){
    return "chuvisco leve";
       }
       if(status == 301){
    return "chuvisco";
       }
       if(status == 302){
    return "chuvisco pesado";
       }
       if(status == 310){
    return "chuva leve";
       }
       if(status == 311){
    return "chuva torrencial";
       }
       if(status == 312){
    return "chuva forte";
       }
       if(status == 313){
    return "chuva e garoa";
       }
       if(status == 314){
    return "chuva pesada e garoa"; 
    }}



    if(status >= 200){ //check for 2xx
       if(status == 200){
    return "trovoada com chuva fraca";
       }
       if(status == 201){
    return "trovoada com chuva";
       }
       if(status == 202){
    return "trovoada com chuva forte";
       }
       if(status == 210){
    return "trovoada fraca";
       }
       if(status == 211){
    return "trovoada";
       }
       if(status == 212){
    return "trovoada forte";
       }
       if(status == 221){
    return "tempestade irregular";
       }
       if(status == 230){
    return "trovoada com leve chuva";
       }
       if(status == 231){
    return "trovoada com chuva";
       }
       if(status == 232){
    return "trovoada com chuva pesada";
       }}
  return "Suporte Tecnico: deu ruim!!";
}
