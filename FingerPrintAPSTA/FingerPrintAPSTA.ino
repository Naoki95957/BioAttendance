#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>
#include <WebServer.h>]

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

/***
 * This will attempt to connect to a wifi network
 * 
 * timeout of 10s
 */
void connectToSSID(const char * ssid, const char * password)
{
  Serial.print("connecting to...");
  Serial.println(ssid);

  WiFi.begin(ssid,password);

  int counter = 0;

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
    ++counter;
    if(counter > 20)//wait for approx 10s  
    {
      Serial.print("Failed to connect to:");
      Serial.println(ssid);
      return;
    }
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  return;
}

//will return with </p> for indentation in html
/***
 * scans networks and prints them in serial and 
 * prepares a string for viewing on a webpage
 */
String scanNetworks()
{
  Serial.println("scan start");

  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  String output = "";
  Serial.println("scan done");
  if (n <= 0) {
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

//since %num is a way for html to pass special characters in html
//this will be a growing list :(
//lets just hope no one's passwords contain '%'
String decodeURLStrings(String original)
{
  char * coriginal = new char[original.length()];
  original.toCharArray(coriginal, original.length() + 1);
  return String(decodeURL(coriginal));
}

/***
 * NOT MY CODE
 * Thanks to this fellow:
 * https://arduino.stackexchange.com/questions/18007/simple-url-decoding 
 * 
 * This decodes the %HH into ascii since the urls are encoded
 */
char * decodeURL(char * encodedURL)
{
  // Create two pointers that point to the start of the data
  char *leader = encodedURL;
  char *follower = leader;
  
  // While we're not at the end of the string (current character not NULL)
  while (*leader) {
      // Check to see if the current character is a %
      if (*leader == '%') {
  
          // Grab the next two characters and move leader forwards
          leader++;
          char high = *leader;
          leader++;
          char low = *leader;
  
          // Convert ASCII 0-9A-F to a value 0-15
          if (high > 0x39) high -= 7;
          high &= 0x0f;
  
          // Same again for the low byte:
          if (low > 0x39) low -= 7;
          low &= 0x0f;
  
          // Combine the two into a single byte and store in follower:
          *follower = (high << 4) | low;
      } else {
          // All other characters copy verbatim
          *follower = *leader;
      }
  
      // Move both pointers to the next character:
      leader++;
      follower++;
  }
  // Terminate the new string with a NULL character to trim it off
  *follower = 0; 
  return encodedURL;
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
  String html = "<!DOCTYPE html> \
  <html> \
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
