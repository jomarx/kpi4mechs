#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

//LCD
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display

SoftwareSerial rfidReader(D3,D4);
String tagString;
char tagNumber[14];
char realTagNum[12];
boolean receivedTag;

const char* ssid = "jomarAP-SP";
const char* password = "maquinay1";

//rfid tag
int validRFID = 7754087;

//buzzer
const int buzzer = D8;

//button
const int startButton = D6;
const int cancelButton = D7;
int buttonState1 = 1;
int buttonState2 = 1;

//mbdc Machine Break Down Code
int mbdc = 0;
int EmpCd = 0;

//potentiometer selector
int potPin = A0;
int potVal = 0;       // variable to store the potValue coming from the sensor

void setup() {
	
	pinMode(startButton, INPUT_PULLUP);
	pinMode(cancelButton, INPUT_PULLUP);
	
  delay(1000);
  pinMode(D1, OUTPUT);
  rfidReader.begin(9600); // the RDM6300 runs at 9600bps
  Serial.begin(9600);

  Serial.println("\n\n\nRFID Reader...ready!");
  
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  //LCD init
  lcd.init();                      // initialize the lcd 
  lcd.init();
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Report Problem");
  lcd.setCursor(0,1);
  lcd.print("Kiosk by JM");
  delay(3000);
  buzzerFunction(3);
  
}

void loop(){
	
  receivedTag=false;
  int TNLeaveLoop = 0;
  int countToFifteen = 0;
  int tempTimer = 0;

	delay (100);
    Serial.println("Start loop");  
	ClearLCD();
	lcd.print("Please Scan ID");
	
	//Reading from RFID
if (rfidReader.available() > 0){
    int BytesRead = rfidReader.readBytesUntil(3, tagNumber, 15);//EOT (3) is the last character in tag 
    receivedTag=true;
	buzzerFunction(1);
    Serial.println("Got a tag");    
	rfidReader.flush(); 
}
	
  	
  if (receivedTag){
    tagString=tagNumber;
    
    Serial.println();
    Serial.print("Tag Number: ");
    Serial.println(tagString);
    tagString.substring(3,11).toCharArray(realTagNum, sizeof(realTagNum));
    Serial.println(realTagNum);
    unsigned long result = hex2int(realTagNum, 8);
    
    Serial.println(result);
    
    if (validRFID == result) {
      Serial.println("RFID allowed");
	  
	  //LCD
		ClearLCD();
		lcd.print("RFID allowed");
		delay(3000);
		
		ClearLCD();
		//put Emp# check here
		lcd.print("Emp# : ");
		lcd.setCursor(8,0);
		lcd.print(EmpCd);
		lcd.setCursor(0,1);
		lcd.print("Brkdwn Cd : P");
		lcd.print(mbdc);
		delay (2000);
		
			while (TNLeaveLoop < 1) {
				buttonState1 = digitalRead(startButton);
				buttonState2 = digitalRead(cancelButton);
				potVal = analogRead(potPin);    // read the potValue from the sensor
				
				if (potVal > 100 && potVal < 286) {mbdc=1;}
				else if (potVal > 297 && potVal < 323) {mbdc=2;}
				else if (potVal > 334 && potVal < 360) {mbdc=3;}
				else if (potVal > 371 && potVal < 397) {mbdc=4;}
				else if (potVal > 408 && potVal < 434) {mbdc=5;}
				else if (potVal > 445 && potVal < 471) {mbdc=6;}
				else if (potVal > 482 && potVal < 508) {mbdc=7;}
				else if (potVal > 519 && potVal < 545) {mbdc=8;}
				else if (potVal > 556 && potVal < 582) {mbdc=9;}
				else if (potVal > 593 && potVal < 619) {mbdc=10;}
				else if (potVal > 630 && potVal < 656) {mbdc=11;}
				else if (potVal > 667 && potVal < 693) {mbdc=12;}
				else if (potVal > 704 && potVal < 730) {mbdc=13;}
				else if (potVal > 741 && potVal < 767) {mbdc=14;}
				else if (potVal > 778 && potVal < 804) {mbdc=15;}
				else if (potVal > 815 && potVal < 841) {mbdc=16;}
				else if (potVal > 852 && potVal < 878) {mbdc=17;}
				else if (potVal > 889 && potVal < 915) {mbdc=18;}
				else if (potVal > 926 && potVal < 952) {mbdc=19;}
				else if (potVal > 963 && potVal < 989) {mbdc=20;}
							
				if (buttonState1 == LOW && buttonState2 == HIGH){
					    Serial.print("Start");
						ClearLCD();
						lcd.setCursor(0,0);
						lcd.print("Starting!");
						TNLeaveLoop = 2;
						delay (2000);
				}
				
				if (buttonState1 == HIGH && buttonState2 == LOW){
						Serial.print("Cancel");
						ClearLCD();
						lcd.setCursor(0,0);
						lcd.print("Cancel!");
						TNLeaveLoop = 2;
						delay (2000);
				}
				
				if (countToFifteen > 18000 ){
					Serial.print("Timout Resetting");
					ClearLCD();
					lcd.print("Task Timeout");
					lcd.setCursor(0,1);
					lcd.print("Resetting");
					
					buzzerFunction(4);
					TNLeaveLoop = 2;	
					delay(3000);
					ESP.restart();
				}
				delay(100);
				Serial.print(countToFifteen);
				countToFifteen++;
				
				ClearLCD();
				lcd.print("Emp# : ");
				lcd.setCursor(8,0);
				lcd.print(EmpCd);
				lcd.setCursor(0,1);
				lcd.print("Brkdwn Cd : P");
				lcd.print(mbdc);
			}
	
    } else {
      Serial.println("RFID is not allowed");
		ClearLCD();
		lcd.print("RFID is not allowed");
    }
    
    memset(tagNumber,0,sizeof(tagNumber)); //erase tagNumber
    // this clears the rest of data on the serial, to prevent multiple scans
    while (Serial.read() >= 0) {
      Serial.flush(); // trying
    }
  }

  delay(500);
}

unsigned long hex2int(char *a, unsigned int len)
{
    int i;
    unsigned long val = 0;

    for(i=0;i<len;i++)
       if(a[i] <= 57)
         val += (a[i]-48)*(1<<(4*(len-1-i)));
       else
        val += (a[i]-55)*(1<<(4*(len-1-i)));

    return val;
}

void ClearLCD() {
	lcd.clear();
	lcd.setCursor(0,0);
}

int buzzerFunction(int counter){
  for (int buzzerTimer = 1; buzzerTimer <= counter; buzzerTimer++){
  tone(buzzer, 5000); // Send 5KHz sound signal...
  delay(100);        // ...for .1 sec
  noTone(buzzer);     // Stop sound...
  delay(100);        // ...for .1sec
  }
}