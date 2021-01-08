#include <SoftwareSerial.h>
SoftwareSerial myserial(10, 11); //rx tx

#include <ArduinoJson.h>
StaticJsonBuffer<200> jsonBuffer; 

#include <TinyGPS.h>
TinyGPS gps;

void setup() {
  Serial.begin(9600);
  myserial.begin(9600);
  DynamicJsonBuffer jsonBuffer;
}

void loop() {
  smartdelay(1000);
  Serial.println();

  uint8_t sat = gps.satellites();
  Serial.print("sat: "); Serial.println(sat);

  float flat, flon;
  unsigned long age;
  gps.f_get_position(&flat, &flon, &age);
  Serial.print("lat: "); Serial.println(flat, 6);
  Serial.print("lon: "); Serial.println(flon, 6);
  int spd = gps.f_speed_kmph();
  Serial.print("speed: "); Serial.println(spd);

 /********************GPRS Baglanti baslatiliyor********************/
 
  if (myserial.available())
  Serial.write(myserial.read());
 
  myserial.println("AT");
  delay(1000);
 
  myserial.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
  delay(2000);
  ShowSerialData();
 
  myserial.println("AT+SAPBR=3,1,\"APN\",\"internet\"");//APN
  delay(2000);
  ShowSerialData();
 
  myserial.println("AT+SAPBR=1,1");
  delay(2000);
  ShowSerialData();
 
  myserial.println("AT+SAPBR=2,1");
  delay(2000);
  ShowSerialData();
 
 
  myserial.println("AT+HTTPINIT");
  delay(2000);
  ShowSerialData();
 
  myserial.println("AT+HTTPPARA=\"CID\",1");
  delay(2000);
  ShowSerialData();
 
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& object = jsonBuffer.createObject();
  
  object.set("device_id", 1);
  object.set("latitude",flat, 6);
  object.set("longitude",flon,6);
  object.set("speed",spd);
  object.set("ip_address","");
  
  object.printTo(Serial);
  Serial.println(" ");  
  String sendtoserver;
  object.prettyPrintTo(sendtoserver);
  delay(2000);
  myserial.println("AT+HTTPPARA=\"URL\",\"http://gpstakipapp.herokuapp.com/api/v1/device/1/traces\""); //Elde edilen verilerin gonderilecegi server adresi
  delay(2000);
  ShowSerialData();
 
  myserial.println("AT+HTTPPARA=\"CONTENT\",\"application/json\"");
  delay(2000);
  ShowSerialData();
 
  myserial.println("AT+HTTPDATA=" + String(sendtoserver.length()) + ",100000");
  Serial.println(sendtoserver);
  delay(2000);
  ShowSerialData();
 
  myserial.println(sendtoserver);
  delay(2000);
  ShowSerialData;
 
  myserial.println("AT+HTTPACTION=1");
  delay(2000);
  ShowSerialData();
 
  myserial.println("AT+HTTPREAD");
  delay(2000);
  ShowSerialData();
 
  myserial.println("AT+HTTPTERM");
  delay(6000);
  ShowSerialData;
}

static void smartdelay(unsigned long ms) {
  unsigned long start = millis();
  do {
    while (myserial.available())
      gps.encode(myserial.read());
  } while (millis() - start < ms);
}

void ShowSerialData()
{
  while (myserial.available() != 0)
    Serial.write(myserial.read());
  delay(1000);
}
