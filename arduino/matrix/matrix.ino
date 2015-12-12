#include <SPI.h>
#include "LedMatrix.h"
#include <ESP8266WiFi.h>

// your WiFi SSID and Password here
const char* ssid = "muenchen.freifunk.net";
const char* password = "";

unsigned long ulReqcount;
unsigned long ulReconncount;


// Number of 
#define NUMBER_OF_DEVICES 5
#define CS_PIN 2

LedMatrix ledMatrix = LedMatrix(NUMBER_OF_DEVICES, CS_PIN);
String dtext="http://";

int x = 0;
  
WiFiServer server(80);

void urldecode(String &input){
  input.replace("+", " ");
  while(input.indexOf('%') > -1){
    int start = input.indexOf('%');
    String src = input.substring(start, start + 3);
    src.toUpperCase();
    Serial.print("SRC:"+src);
    unsigned char char1 = src.charAt(1);
    unsigned char char2 = src.charAt(2);
    Serial.print(char1); Serial.print(":"); Serial.print(char2); Serial.print(":");
 // normalize A-F   
    if (char1 >= 'A') {char1 = char1 - 7; };
    if (char2 >= 'A') {char2 = char2 - 7; };   
    unsigned char dest = (char1 - 48) * 16 -48 + char2;
        Serial.print(char1); Serial.print(":"); Serial.print(char2); Serial.print(":"); Serial.println(dest);
    String destString = String((char)dest);
    input.replace(src, destString); 
  }
// we have latin1 and want cp437  
// transpose äöüÄÖÜß
  input.replace((char)0xE4, (char)0x84);
  input.replace((char)0xF6, (char)0x94);
  input.replace((char)0xFC, (char)0x81);
  input.replace((char)0xC4, (char)0x8E);
  input.replace((char)0xD6, (char)0x99);
  input.replace((char)0xDC, (char)0x9A);
  input.replace((char)0xDF, (char)0xE1);
}

  
void setup() {
  
  ulReqcount=0; 
  ulReconncount=0;

  
  ledMatrix.init();
  ledMatrix.setText(".");
  ledMatrix.setNextText("+");
  
// start serial
  Serial.begin(9600);
  delay(1);
  
  WiFi.mode(WIFI_STA);
  WiFiStart();
  
}

void WiFiStart()
{
  ulReconncount++;
  
 // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  ledMatrix.setText("connect to: " + (String)ssid);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address

  IPAddress myAddr = WiFi.localIP();
  Serial.println(myAddr);
  String ipAddress = (String)myAddr[0] + (String)"." + (String)myAddr[1] + (String)"." + 
   (String)myAddr[2] + (String)"." + (String)myAddr[3];
   
  ledMatrix.setText((String)"http://"+ (String)myAddr[0] + (String)"." + (String)myAddr[1] + (String)"." + 
   (String)myAddr[2] + (String)"." + (String)myAddr[3]);
  
   dtext = dtext + ipAddress;
}


void loop() {

  ledMatrix.clear();
  ledMatrix.scrollTextLeft();
  ledMatrix.drawText();
  ledMatrix.commit();
  delay(100);
  x=x+1;
  if (x == 1) {
     ledMatrix.setNextText(dtext); 
  }
    // check if WLAN is connected
  if (WiFi.status() != WL_CONNECTED)
  {
    WiFiStart();
  }
  
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) 
  {
    return;
  }
    // Wait until the client sends some data
  Serial.println("new client");
  unsigned long ultimeout = millis()+250;
  while(!client.available() && (millis()<ultimeout) )
  {
    delay(1);
  }
  if(millis()>ultimeout) 
  { 
    Serial.println("client connection time-out!");
    return; 
  }
  
  // Read the first line of the request
  String sRequest = client.readStringUntil('\r');
  Serial.println(sRequest);
//  ledMatrix.setNextText(sRequest);
  client.flush();
  
   // get path; end of path is either space or ?
  // Syntax is e.g. GET /?pin=MOTOR1STOP HTTP/1.1
  String sPath="",sParam="", sCmd="";
  String sGetstart="GET ";
  int iStart,iEndSpace,iEndQuest;
  iStart = sRequest.indexOf(sGetstart);
  if (iStart>=0)
  {
    iStart+=+sGetstart.length();
    iEndSpace = sRequest.indexOf(" ",iStart);
    iEndQuest = sRequest.indexOf("?",iStart);
    
    // are there parameters?
    if(iEndSpace>0)
    {
      if(iEndQuest>0)
      {
        // there are parameters
        sPath  = sRequest.substring(iStart,iEndQuest);
        sParam = sRequest.substring(iEndQuest,iEndSpace);
        Serial.println("Parameters!");
      }
      else
      {
        // NO parameters
        sPath  = sRequest.substring(iStart,iEndSpace);
        Serial.println("No Parameters");
      }
    }
  }
  
  ///////////////////////////////////////////////////////////////////////////////
  // output parameters to serial, you may connect e.g. an Arduino and react on it
  ///////////////////////////////////////////////////////////////////////////////
  if(sParam.length()>0)
  {
    int iEqu=sParam.indexOf("=");
    if(iEqu>=0)
    {
      sCmd = sParam.substring(iEqu+1,sParam.length());
      Serial.println(sCmd);
    }
  }
 
  Serial.println("setNextText");
  Serial.println(sCmd.length());
  Serial.println(sCmd);
  if (sCmd.length() > 0) {
      urldecode(sCmd);
      ledMatrix.setText(sCmd);
  }
  ///////////////////////////
  // format the html response
  ///////////////////////////
  String sResponse,sHeader;
 
    sResponse="<style> input[type=text]{ -webkit-appearance: none; -moz-appearance: none; display: block; margin: 0; width: 100%; height: 40px; line-height: 40px; font-size: 17px; border: 1px solid #bbb; }</style><form action=\"/\" method=\"GET\">Text theMatrix<input type=\"text\" name=\"text\"><input type=\"submit\" value=\"Display Text on theMatrix!\"><form>";
    
    sHeader  = "HTTP/1.1 200 OK\r\n";
    sHeader += "Content-Length: ";
    sHeader += sResponse.length();
    sHeader += "\r\n";
    sHeader += "Content-Type: text/html\r\n";
    sHeader += "Connection: close\r\n";
    sHeader += "\r\n";
  
    client.print(sHeader);
    client.print(sResponse);
  
  // and stop the client
     client.stop();
     Serial.println("Client disonnected");
}
