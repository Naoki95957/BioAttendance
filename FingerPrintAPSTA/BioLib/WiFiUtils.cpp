#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiServer.h>

#include "WiFiUtils.h"

void connectToSSID(const char * ssid, const char * psk)
{
	Serial.print("connecting to...");
	Serial.println(ssid);

	WiFi.begin(ssid, psk);

	int counter = 0;

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
		++counter;
		if (counter > 20)//wait for approx 10s  
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
	}
	else {
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
		}
		else {
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
