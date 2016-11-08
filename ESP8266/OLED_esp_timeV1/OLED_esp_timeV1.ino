
/* 
 * Demo for SSD1306 based 128x64 OLED module using Adafruit SSD1306 
 * library (https://github.com/adafruit/Adafruit_SSD1306).
 * 
 * See https://github.com/pacodelgado/arduino/wiki/SSD1306-based-OLED-connected-to-Arduino 
 * for more information.
 *
 */

 /*

 Udp NTP Client

 Get the time from a Network Time Protocol (NTP) time server
 Demonstrates use of UDP sendPacket and ReceivePacket
 For more on NTP time servers and the messages needed to communicate with them,
 see http://en.wikipedia.org/wiki/Network_Time_Protocol

 created 4 Sep 2010
 by Michael Margolis
 modified 9 Apr 2012
 by Tom Igoe
 updated for the ESP8266 12 Apr 2015 
 by Ivan Grokhotkov

 This code is in the public domain.

 */
 
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

// If using software SPI (the default case):
#define OLED_MOSI  D3 //D1
#define OLED_CLK   D2   //D0
#define OLED_DC    D5
#define OLED_CS    D6
#define OLED_RESET D4

/* Create an instance for the SSD1306 OLED display in SPI mode 
 * connection scheme: 
 *   D0=sck=Pin 12 / 0 / 2
 *   D1=mosi=Pin 11 / 1 / 3
 *   CS=Pin 8  / 4 / 6
 *   DC=A0=Pin 9 / 3 / 5
 *   Reset=Pin 10 / 2 / 4
 */

char ssid[] = "jomarAP-SP";  //  your network SSID (name)
char pass[] = "maquinay1";       // your network password
// const char* host = "utcnist2.colorado.edu";
const char* host = "128.138.141.172";

int ln = 0;
String TimeDate = "";

//start NTPClientMod

//mod
#include <RTClib.h>                       // RTC-Library
RTC_Millis RTC;                           // RTC (soft)
DateTime now;                             // current time
int ch,cm,cs,os,cdy,cmo,cyr,cdw;          // current time & date variables
int nh,nm,ns,ndy,nmo,nyr,ndw;             // NTP-based time & date variables

//end mod

unsigned int localPort = 2390;      // local port to listen for UDP packets

/* Don't hardwire the IP address or we won't get the benefits of the pool.
 *  Lookup the IP address for the host name instead */
//IPAddress timeServer(129, 6, 15, 28); // time.nist.gov NTP server
IPAddress timeServerIP; // time.nist.gov NTP server address
const char* ntpServerName = "time.nist.gov";

const int NTP_PACKET_SIZE = 48; // NTP time stamp is in the first 48 bytes of the message

byte packetBuffer[ NTP_PACKET_SIZE]; //buffer to hold incoming and outgoing packets

// A UDP instance to let us send and receive packets over UDP
WiFiUDP udp;


//end NTP

Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

void setup()   {  

//start NTP
Serial.begin(115200);
  Serial.println();
  Serial.println();

  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, pass);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Starting UDP");
  udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(udp.localPort());
//end NTP

                
//  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC);
  display.display();
  delay(1000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

 }

void loop()
{
  //NTP start

//get a random server from the pool
  WiFi.hostByName(ntpServerName, timeServerIP); 

  sendNTPpacket(timeServerIP); // send an NTP packet to a time server
  // wait to see if a reply is available
  delay(1000);
  
  int cb = udp.parsePacket();
  if (!cb) {
    Serial.println("no packet yet");
  }
  else {
    Serial.print("packet received, length=");
    Serial.println(cb);
    // We've received a packet, read the data from it
    udp.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer

    //the timestamp starts at byte 40 of the received packet and is four bytes,
    // or two words, long. First, esxtract the two words:

    unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
    unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
    // combine the four bytes (two words) into a long integer
    // this is NTP time (seconds since Jan 1 1900):
    unsigned long secsSince1900 = highWord << 16 | lowWord;
    Serial.print("Seconds since Jan 1 1900 = " );
    Serial.println(secsSince1900);

    // now convert NTP time into everyday time:
    Serial.print("Unix time = ");
    // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
    const unsigned long seventyYears = 2208988800UL;
    // subtract seventy years:
    unsigned long epoch = secsSince1900 - seventyYears;
    // print Unix time:
    Serial.println(epoch);


    // print the hour, minute and second:
    Serial.print("The UTC time is ");       // UTC is the time at Greenwich Meridian (GMT)
    Serial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)
    Serial.print(':');
    if ( ((epoch % 3600) / 60) < 10 ) {
      // In the first 10 minutes of each hour, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
    Serial.print(':');
    if ( (epoch % 60) < 10 ) {
      // In the first 10 seconds of each minute, we'll want a leading '0'
      Serial.print('0');
    }
    Serial.println(epoch % 60); // print the second
    //mod
int tz = 8;                                            // adjust for PH time

      DateTime gt(epoch + (tz*60*60));                       // obtain date & time based on NTP-derived epoch...
      DateTime ntime(epoch + (tz*60*60));                    // if in DST correct for GMT-4 hours else GMT-5
      RTC.adjust(ntime);                                     // and set RTC to correct local time   
      nyr = ntime.year()-2000;                       
      nmo = ntime.month();
      ndy = ntime.day();    
      nh  = ntime.hour(); if(nh==0) nh=24;                   // adjust to 1-24            
      nm  = ntime.minute();                     
      ns  = ntime.second();                     

      display.clearDisplay();
      display.setCursor(0,0);

      Serial.print(F("... NTP packet local time: [GMT + ")); Serial.print(tz); Serial.print(F("]: "));       // Local time at Greenwich Meridian (GMT) - offset  
      if(nh < 10) Serial.print(F(" ")); Serial.print(nh);  Serial.print(F(":"));          // print the hour 
      if(nm < 10) Serial.print(F("0")); Serial.print(nm);  Serial.print(F(":"));          // print the minute
      if(ns < 10) Serial.print(F("0")); Serial.print(ns);                       // print the second

      Serial.print(F(" on "));                                        // Local date
      if(nmo < 10) Serial.print(F("0")); Serial.print(nmo);  Serial.print(F("/"));        // print the month
      if(ndy < 10) Serial.print(F("0")); Serial.print(ndy); Serial.print(F("/"));                   // print the day
      if(nyr < 10) Serial.print(F("0")); Serial.println(nyr);          // print the year
      Serial.println();

      if(nh < 10) display.print(F(" ")); display.print(nh);  display.print(F(":"));          // print the hour 
      if(nm < 10) display.print(F("0")); display.print(nm);  display.print(F(":"));          // print the minute
      if(ns < 10) display.print(F("0")); display.print(ns);                       // print the second

      display.print(F(" - "));                                        // Local date
      if(nmo < 10) display.print(F("0")); display.print(nmo);  display.print(F("/"));        // print the month
      if(ndy < 10) display.print(F("0")); display.print(ndy); display.print(F("/"));                   // print the day
      if(nyr < 10) display.print(F("0")); display.print(nyr);          // print the year
      display.println();      

//mod end
  }
  // wait ten seconds before asking for the time again
  delay(10000);

  //NTP End

  
  //screen
  static unsigned long thisMicros = 0;
  static unsigned long lastMicros = 0;
  display.print("123456789012345678901\n");
  display.print("Hanesbrand \n");
  display.print(thisMicros - lastMicros);
  display.print(" microseconds");
  display.display();
  lastMicros = thisMicros;
  thisMicros = micros();

  //sleep for 1min
  //ESP.deepSleep(60000000);
  delay(1000*60);
  
}



// send an NTP request to the time server at the given address
unsigned long sendNTPpacket(IPAddress& address)
{
  Serial.println("sending NTP packet...");
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12]  = 49;
  packetBuffer[13]  = 0x4E;
  packetBuffer[14]  = 49;
  packetBuffer[15]  = 52;

  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  udp.beginPacket(address, 123); //NTP requests are to port 123
  udp.write(packetBuffer, NTP_PACKET_SIZE);
  udp.endPacket();
}