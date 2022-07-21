//main code file 
#include <Adafruit_Fingerprint.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"

#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
SoftwareSerial mySerial(2, 3);
#else
#define mySerial Serial1
#endif
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);


LiquidCrystal_I2C lcd(0x27,16,2); 

#define DHTPIN 5     // Digital pin connected to the DHT sensor

#define DHTTYPE DHT11   

DHT dht(DHTPIN, DHTTYPE);

void setup()
{
  Serial.begin(9600);
  Serial.println(F("DHTxx test!"));
  pinMode (4, OUTPUT);
  dht.begin(); //temp and humidity sensor
  
  //LCD setup
  lcd.init();
  lcd.clear();         
  lcd.backlight();  
  
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit finger detect test");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  delay(5);
  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) { delay(1); }
  }


  finger.getTemplateCount();
}

void loop()                     // run over and over again
{
  getFingerprintID();
  delay(100);           
  gethum_temp();
}

//funtions 
uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
  }

  // OK converted!
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
    digitalWrite (11, HIGH);
    lcd.clear();
    lcd.setCursor (0,0);
    lcd.print (" Access Granted");
    delay (10000);
    lcd.clear();
    digitalWrite (11, LOW);
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    lcd.clear();
    lcd.setCursor (0,0);
    lcd.print ("  Access Denied");
    delay (2000);
    lcd.clear();
    return p;
  }

}

//get humidity and temp function 
void gethum_temp(){
  int temp = analogRead (A1);
  int settemp = map(temp, 1023, 0, 50, 99);

  int h = dht.readHumidity();
  float t = dht.readTemperature();
  int f = dht.readTemperature(true);

  if (isnan(h) || isnan(t) || isnan(f)) {
    lcd.print(F("Failed to read from DHT sensor!"));
    return;
  }
  float hif = dht.computeHeatIndex(f, h);
 
  float hic = dht.computeHeatIndex(t, h, false);

  lcd.setCursor (0,0);
  lcd.print(F(" Humidity: "));
  lcd.print(h);
  lcd.print ("%");
  lcd.setCursor (0,1);
  lcd.print(F("   Temp: ")); 
  lcd.print(f);
  lcd.print("F");
}