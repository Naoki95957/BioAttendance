#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WebServer.h>
#include <WiFiUtils.h>

//kind of a working demo for a wifi scanner so far....
//TODO use an auxiliary variable to get the device to stop attempting to connect after a timeout
//note 2.4ghz wifi
const char *assid = "esp32";
const char *asecret = "secret123456";

WiFiServer server(80);

// Auxiliary variables
bool scannedNetwork = false;
String newPSK = "";
String newSSID = "";
String lastRequest = "";

//runs once
void setup() {
  //this is the buad rate
  //you can view serial with 'CTRL + SHIFT + M'
  Serial.begin(115200);
  WiFi.mode(WIFI_AP_STA);

  //access point part
  Serial.println("Creating Accesspoint");
  Serial.println(assid);
  Serial.println(asecret);
  WiFi.softAP(assid,asecret);
  Serial.print("IP address:\t");
  //this address by defualt is 192.168.4.1
  Serial.println(WiFi.softAPIP());

  //doing stuff that I'm unaware of
  server.begin();
}

//runs indefinetly
void loop() {
  WiFiClient client = server.available();  
  if(client)//handling input to hardware is going to be based on indexing page requests
  {
  String request = client.readStringUntil('\r');

  if(lastRequest == request)//no change found
  {
    delay(10);
    return;
  }

  //-----------------------------START-----------------------------//
  ///////////////Client response checking based on url///////////////
  bool scanRequest = false;//temp aux is needed so that networks can be listed in html to client
  if(request.indexOf("SCANNER=ON") != -1)
  {
    //scanner button
    scanRequest = true;
    scannedNetwork = true;
  }
  
  //if textboxes contain info
  if(request.indexOf("T1=") != -1 && request.indexOf("T2=") != -1)
  {
    //plus 3 to account for the length of the string
    newSSID = request.substring(request.indexOf("T1=") + 3, request.indexOf("&T2="));
    int i = 0;
    bool allDigits = true;
    if(scannedNetwork)
    {
      for(int j = 0; j < newSSID.length(); ++j )
      {
        if(!isDigit(newSSID.charAt(j)))
        {
          allDigits = false;
        }
      }
      if(allDigits)
      {
        i = newSSID.toInt();//string to int
        newSSID = WiFi.SSID(i - 1);//int to proper ssid
      }
      else
      {
        newSSID = decodeURLStrings(newSSID);
      }
    }
    
    Serial.println(newSSID);
    newPSK = request.substring(request.indexOf("T2=") + 3, request.indexOf("HTTP") - 1);
    newPSK = decodeURLStrings(newPSK);
    Serial.println(newPSK);
  }
  
  if(request.indexOf("JOIN_NETWORK=JOIN") != -1)//Both this and the scanner cannot be a possibility
  {
    if(newSSID.length() > 0)
    {
      Serial.println("ATTEMPTING TO ESTABLISH STA");
      char * cssid = new char[newSSID.length()];
      char * cpsk = new char[newPSK.length()];
  
      newSSID.toCharArray(cssid, newSSID.length() + 1);
      newPSK.toCharArray(cpsk, newPSK.length() + 1);
  
      Serial.println(cssid);
      Serial.println(cpsk);
      
      connectToSSID(cssid, cpsk);
    }
  }
  ///////////////Client response checking based on url///////////////
  //------------------------------END------------------------------//

  //HTML is going to be an ugly mess because we're basically building it on the fly
  //alternatively...
  //
  //
  //
  //
  // ...
  // ... we'll make an app that automataically formats the requests for the user
  // this won't be as universal since it'll no longer be supported with browsers however

  //////////////////////Writing HTML to the client///////////////////
  //-----------------------------START-----------------------------//

  //meta tags are necessary to get browsers to stop cache
  //cache control is for http 1.1
  //pragma for http 1.0
  //expires for proxies
  
  String html = "<!DOCTYPE html> \
  <html> \
    <meta http-equiv=\"Cache-Control\" content=\"no-cache, no-store, must-revalidate\" /> \
    <meta http-equiv=\"Pragma\" content=\"no-cache\" /> \
    <meta http-equiv=\"Expires\" content=\"0\" /> \
    <body> \
      <center><h1>ESP32 Soft access point</h1></center> \
      <center><h2>Web Server</h2></center> \
      <form> \
        <button name=\"SCANNER\" button style=\"color:green\" value=\"ON\" type=\"submit\">SCAN NETWORKS</button> \
        <button name=\"JOIN_NETWORK\" button style=\"color=red\" value=\"JOIN\" type=\"submit\">CONNECT TO NETWORK</button><br><br> \
        <input TYPE=TEXT NAME='T1' VALUE='' SIZE='25' MAXLENGTH='50'></input> \
        <input TYPE=TEXT NAME='T2' VALUE='' SIZE='25' MAXLENGTH='50'></input> \
        </p>";//break a line
     
    String closingHtml = "</form> \
    </body> \
  </html>";
  client.print(html);
  if(scanRequest)
  {
    client.print(scanNetworks());
  }
  client.print(closingHtml);

  
  //////////////////////Writing HTML to the client///////////////////
  //------------------------------END------------------------------//
  lastRequest = "";
  request = "";
  }
}
