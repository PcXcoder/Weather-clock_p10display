WiFiServer server(80);
String header;
static const char* ssid = "eduroam";
static const char* username = "bruno.bohrer@unesp.br";
static const char* password = "**************";
String jsonBuffer;
int Tk;     
int Tc;
char buff[20];
String ip;
uint8_t mac[6] {0x30, 0xAE, 0xA4, 0x0F, 0x89, 0x78};