
/* 
 * Demo for SSD1306 based 128x64 OLED module using Adafruit SSD1306 
 * library (https://github.com/adafruit/Adafruit_SSD1306).
 * 
 * See https://github.com/pacodelgado/arduino/wiki/SSD1306-based-OLED-connected-to-Arduino 
 * for more information.
 *
 */
 
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>

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

const char* ssid     = "outsourcing2.5r";
const char* password = "alabang1";
// const char* host = "utcnist2.colorado.edu";
const char* host = "128.138.141.172";

int ln = 0;
String TimeDate = "";


Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

void setup()   {                
//  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC);
  display.display();
  delay(1000);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  // WIFI
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  WiFi.begin(ssid, password);
  delay(100);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
}

void loop()
{
  //wifi
  Serial.print("connecting to ");
  Serial.println(host);

  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 13;

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

   // This will send the request to the server
  client.print("HEAD / HTTP/1.1\r\nAccept: */*\r\nUser-Agent: Mozilla/4.0 (compatible; ESP8266 NodeMcu Lua;)\r\n\r\n");

  delay(100);

  // Read all the lines of the reply from server and print them to Serial
  // expected line is like : Date: Thu, 01 Jan 2015 22:00:14 GMT
  char buffer[12];
  String dateTime = "";
  
  while(client.available())
  {
    String line = client.readStringUntil('\r');

    if (line.indexOf("Date") != -1)
    {
      Serial.print("=====>");
    } else
    {
      // Serial.print(line);
      // date starts at pos 7
      TimeDate = line.substring(7);
      Serial.println(TimeDate);
      // time starts at pos 14
      TimeDate = line.substring(7, 24);
      TimeDate.toCharArray(buffer, 20);
      // clear screen then print
      display.clearDisplay();
      display.setCursor(0,0);
      display.print(buffer);
      display.print("\n");
    }
  }
  
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

