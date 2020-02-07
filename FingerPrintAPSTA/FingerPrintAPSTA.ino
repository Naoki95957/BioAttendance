#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WebServer.h>]


//note 2.4ghz wifi
//this is not broadcasted
const char *assid = "esp32";
const char *asecret = "secret123456";

const char* www_username = "admin";
const char* www_password = "esp32";

WiFiServer server(80);
String html ="<!DOCTYPE html> \
<html> \
<body> \
<center><h1>ESP32 Soft access point</h1></center> \
<center><h2>Web Server</h2></center> \
<form> \
<button name=\"BUTTON\" button style=\"color:green\" value=\"ON\" type=\"submit\">BUTTON ON</button> \
<button name=\"BUTTON\" button style=\"color=red\" value=\"OFF\" type=\"submit\">BUTTON OFF</button><br><br> \
</form> \
</body> \
</html>";


void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_AP_STA);

  //access point part
  Serial.println("Creating Accesspoint");
  Serial.println(assid);
  Serial.println(asecret);
  WiFi.softAP(assid,asecret);
  Serial.print("IP address:\t");
  Serial.println(WiFi.softAPIP());

  //do stuff
  server.begin();
  //dont judge me plz lol
  //I tried to make my hobby slightly more appealing to my ex at the time
  connectToSSID("Sugar Internet (2.4GHz)", "f8g92svbf9nic4sugar");
}

void connectToSSID(const char * ssid, const char * password)
{
  Serial.print("connecting to...");
  Serial.println(ssid);

  WiFi.begin(ssid,password);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());   
}

void scanNetworks()
{
  Serial.println("scan start");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == 0) ? " " : "*");
      delay(10);
    }
  }
  Serial.println("");
}

void loop() {
  WiFiClient client = server.available();  
  if(client)//handling input to hardware is going to be based on indexing page requests
  {
  String request = client.readStringUntil('\r');
  if(request.indexOf("BUTTON=ON") != -1)
  {
    //enable something
    scanNetworks();
  }
  if(request.indexOf("BUTTON=OFF") != -1)
  {
     //disable something
  }
  client.print(html);
  request = "";
  }
}
