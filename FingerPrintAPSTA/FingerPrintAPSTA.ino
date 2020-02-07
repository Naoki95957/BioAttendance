#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WebServer.h>]

//kind of a working demo for a wifi scanner so far....

//note 2.4ghz wifi
const char *assid = "esp32";
const char *asecret = "naoki123456";

const char* www_username = "admin";
const char* www_password = "esp32";

WiFiServer server(80);

// Auxiliar variables to store the current output state
bool buttonState = "off";

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
//will return with </p> for indentation in html
String scanNetworks()
{
  Serial.println("scan start");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  String output = "";
  Serial.println("scan done");
  if (n == 0) {
    Serial.println("no networks found");
    output += "no networks found</p>";
  } else {
    Serial.print(n);
    output += n;
    Serial.println(" networks found");
    output += " networks found</p>";
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      output += (i + 1);
      Serial.print(": ");
      output += ": ";
      Serial.print(WiFi.SSID(i));
      output += WiFi.SSID(i);
      Serial.print(" (");
      output += " (";
      Serial.print(WiFi.RSSI(i));
      output += WiFi.RSSI(i);
      Serial.print(")");
      output += ")";
      Serial.println((WiFi.encryptionType(i) == 0) ? " " : "*");
      output += ((WiFi.encryptionType(i) == 0) ? " " : "*");
      output += "</p>";
    }
  }
  Serial.println("");
  output += "</p>";
  return output;
}

void loop() {
  WiFiClient client = server.available();  
  if(client)//handling input to hardware is going to be based on indexing page requests
  {
  String request = client.readStringUntil('\r');
  bool willScan = false;
  if(request.indexOf("SCANNER=ON") >= 0)
  {
    //scanner button
    willScan = true;
  }
  if(request.indexOf("BUTTON=NOTHING") >= 0)
  {
     //useless button
  }

  String html = "<!DOCTYPE html> \
<html> \
  <body> \
    <center><h1>ESP32 Soft access point</h1></center> \
    <center><h2>Web Server</h2></center> \
    <form> \
      <button name=\"SCANNER\" button style=\"color:green\" value=\"ON\" type=\"submit\">SCAN NETWORKS</button> \
      <button name=\"BUTTON\" button style=\"color=red\" value=\"NOTHING\" type=\"submit\">USELESS BUTTON</button><br><br> \
      </p>";//break a line
   
  String closingHtml = "</form> \
  </body> \
</html>";
  client.print(html);
  if(willScan)
  {
    willScan = false;
    client.print(scanNetworks());
  }
  client.print(closingHtml);
  request = "";
  }
}
