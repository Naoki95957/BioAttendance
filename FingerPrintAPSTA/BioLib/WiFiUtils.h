#ifndef WIFI_UTILS_H
#define WIFI_UTILS_H

void connectToSSID(const char * ssid, const char * psk);
String scanNetworks();
String decodeURLStrings(String original);
char * decodeURL(char * encodedURL);

#endif
