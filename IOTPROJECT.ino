#include "Adafruit_IO_Client.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>               
#include <TimeLib.h>                 
#include <LiquidCrystal.h>  
LiquidCrystal lcd(D6, D5, D1, D2, D3, D4); 

WiFiClient client;

//Adafruit bilgileri
#define IO_USERNAME  "iremgulten"
#define IO_KEY       "aio_vaZi83pI3erR6MDeRJRtNTSAMC8d"
Adafruit_IO_Client aio = Adafruit_IO_Client(client,IO_KEY);

Adafruit_IO_Feed AioMedicineName = aio.getFeed("MedicineName");
Adafruit_IO_Feed AioMedicineTimeM = aio.getFeed("MorningMedicine");
Adafruit_IO_Feed AioMedicineTimeA = aio.getFeed("AfternoonMedicine");
Adafruit_IO_Feed AioMedicineTimeE = aio.getFeed("EveningMedicine");

//Wifi 
char* ssid = "impossibleisnothing";  //wifi ssid
char* password = "**busmer97**";   //wifi password

//NTP sunucusuna bağlanma
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "tr.pool.ntp.org", 10800, 60000);


#define buzz 15
#define blue_button 16
#define red_button 13
#define white_button 1
#define green_button 3


char Time[ ] = "00:00:00";
char Date[ ] = "DATE:00/00/2000";
byte last_second, second_, minute_, hour_, day_, month_;
int year_;
char MorningMedicineTime[] = "AA:AA:AA";
char AfternoonMedicineTime[] = "AA:AA:AA";
char EveningMedicineTime[] = "AA:AA:AA";
char MedicineName[] = "00000";

void setup() {
  pinMode(buzz,OUTPUT);
  pinMode(blue_button,INPUT_PULLDOWN_16);
  pinMode(red_button,INPUT);

  //TX ve RX pinleri GPIO pinlerine dönüştürülüyor
  pinMode(white_button, FUNCTION_3);
  pinMode(green_button, FUNCTION_3);
  
  pinMode(white_button, OUTPUT);
  pinMode(green_button, OUTPUT);
  

  //Serial.begin(115200);

  //Display kurulum
  lcd.begin(16, 2);                 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(Time);
  lcd.setCursor(0, 1);
  lcd.print(Date);

  //Wifi baglantisi gerceklestirimi
  WiFi.begin(ssid, password);
  //Serial.print("Connecting.");
  while ( WiFi.status() != WL_CONNECTED ) {
    delay(500);
    //Serial.print(".");
  }
  //Serial.println("connected");
  timeClient.begin();
  aio.begin();

}

void loop() {
  
  timeClient.update();
  unsigned long unix_epoch = timeClient.getEpochTime();    // Get Unix epoch time from the NTP server

  second_ = second(unix_epoch);
  if (last_second != second_) {
    
    minute_ = minute(unix_epoch);
    hour_   = hour(unix_epoch);
    day_    = day(unix_epoch);
    month_  = month(unix_epoch);
    year_   = year(unix_epoch);

    Time[7] = second_ % 10 + 48;
    Time[6] = second_ / 10 + 48;
    Time[4]  = minute_ % 10 + 48;
    Time[3]  = minute_ / 10 + 48;
    Time[1]  = hour_   % 10 + 48;
    Time[0]  = hour_   / 10 + 48;

    Date[5]  = day_   / 10 + 48;
    Date[6]  = day_   % 10 + 48;
    Date[8]  = month_  / 10 + 48;
    Date[9]  = month_  % 10 + 48;
    Date[13] = (year_   / 10) % 10 + 48;
    Date[14] = year_   % 10 % 10 + 48;

    //Guncellenen saat verisi ile displayde saat ve tarih gosterimi
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(Time);
    lcd.setCursor(0, 1);
    lcd.print(Date);
    last_second = second_;
    
  }
    delay(500);

    int buttonstateGreen = digitalRead(green_button);
    int buttonstateWhite = digitalRead(white_button);  
    int buttonstateBlue = digitalRead(blue_button);
    int buttonstateRed = digitalRead(red_button);
    int buzzerState = digitalRead(buzz);

    
    if(buttonstateRed == HIGH){
     
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("GUNDE 1 DEFA");
        lcd.setCursor(0, 1);
        lcd.print("KURULDU");
        delay(1000);
        
      sendMedicineName();
      sendMorningMedicineTime();
    }

    if(buttonstateBlue == HIGH){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("GUNDE 2 DEFA");
        lcd.setCursor(0, 1);
        lcd.print("KURULDU");
        delay(1000);
        
      sendMedicineName();
      sendMorningMedicineTime();
      sendEveningMedicineTime();
    }

    if(buttonstateWhite == HIGH){

        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("GUNDE 3 DEFA");
        lcd.setCursor(0, 1);
        lcd.print("KURULDU");
        delay(1000);
        
      sendMedicineName();
      sendMorningMedicineTime();
      sendAfternoonMedicineTime();
      sendEveningMedicineTime();
    }

    // Adafruittan alinan saat ile o anki saatin karsilastirilmasi. İlac saatinde buzzerin otmesi saglaniyor
    if(MorningMedicineTime[0]==Time[0] && MorningMedicineTime[1]==Time[1] && MorningMedicineTime[3]==Time[3] && MorningMedicineTime[4]==Time[4]&& MorningMedicineTime[6]==Time[6] && MorningMedicineTime[7]==Time[7]){ 
      digitalWrite(buzz,HIGH);    

      MorningMedicineTime[0] = 'A';
      MorningMedicineTime[1] = 'A';
      MorningMedicineTime[3] = 'A';
      MorningMedicineTime[4] = 'A';
      MorningMedicineTime[6] = 'A';
      MorningMedicineTime[7] = 'A';
    }

    if(AfternoonMedicineTime[0]==Time[0] && AfternoonMedicineTime[1]==Time[1] && AfternoonMedicineTime[3]==Time[3] && AfternoonMedicineTime[4]==Time[4]&& AfternoonMedicineTime[6]==Time[6] && AfternoonMedicineTime[7]==Time[7]){ 
      digitalWrite(buzz,HIGH);   
      
      AfternoonMedicineTime[0] = 'A';
      AfternoonMedicineTime[1] = 'A';
      AfternoonMedicineTime[3] = 'A';
      AfternoonMedicineTime[4] = 'A';
      AfternoonMedicineTime[6] = 'A';
      AfternoonMedicineTime[7] = 'A';
    }

    if(EveningMedicineTime[0]==Time[0] && EveningMedicineTime[1]==Time[1] && EveningMedicineTime[3]==Time[3] && EveningMedicineTime[4]==Time[4]&& EveningMedicineTime[6]==Time[6] && EveningMedicineTime[7]==Time[7]){ 
      digitalWrite(buzz,HIGH);    

      EveningMedicineTime[0] = 'A';
      EveningMedicineTime[1] = 'A';
      EveningMedicineTime[3] = 'A';
      EveningMedicineTime[4] = 'A';
      EveningMedicineTime[6] = 'A';
      EveningMedicineTime[7] = 'A';
    }



    //Buzzer oterken LCD'de gosterim
    if(buzzerState == HIGH){
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(MedicineName);
        lcd.setCursor(0, 1);
        lcd.print("ILAC ZAMANI");
        delay(2000);
    }

    //Buzzer oterken yesil buton ile buzzer susturma ve LCD guncellemesi
    if(buzzerState == HIGH && buttonstateGreen == HIGH){              
      digitalWrite(buzz,LOW);
      delay(300);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("YENIDEN");
      lcd.setCursor(0, 1);
      lcd.print("KURMAYI UNUTMA");
      delay(2000);
    }  
}

void sendMedicineName(){
  //Cloud'a ilac adi gonderildi
  AioMedicineName.send("PAROL");
  delay(500);

  //Cloud'dan ilac adi cekildi
  FeedData getMedicineName = AioMedicineName.receive();
  char *tempName;
  tempName = (char *)getMedicineName;

  //Pointerdan char dizisine veri atimi
  MedicineName[0] = tempName[0];
  MedicineName[1] = tempName[1];
  MedicineName[2] = tempName[2];
  MedicineName[3] = tempName[3];
  MedicineName[4] = tempName[4];
  delay(300);
  
}

void sendMorningMedicineTime(){
  // Cloud'a ilac kullanim saati gonderildi
  AioMedicineTimeM.send("08:00:00");
  delay(500);

  //Cloud'dan ilac kullanim saati cekildi
  FeedData getMedicineTime1 = AioMedicineTimeM.receive();  

  char *tempTime1 ;
  tempTime1 = (char *)getMedicineTime1;

 //Pointerdan char dizisine veri atimi
  MorningMedicineTime[0] = tempTime1[0];
  MorningMedicineTime[1] = tempTime1[1];
  MorningMedicineTime[3] = tempTime1[3];
  MorningMedicineTime[4] = tempTime1[4];
  MorningMedicineTime[6] = tempTime1[6];
  MorningMedicineTime[7] = tempTime1[7];
  delay(300);  
}

void sendAfternoonMedicineTime(){
  // Cloud'a ilac kullanim saati gonderildi
  AioMedicineTimeA.send("14:00:00");
  delay(500);

  //Cloud'dan ilac kullanim saati cekildi
  FeedData getMedicineTime2 = AioMedicineTimeA.receive();


  char *tempTime2 ;
  tempTime2 = (char *)getMedicineTime2;

 //Pointerdan char dizisine veri atimi
  AfternoonMedicineTime[0] = tempTime2[0];
  AfternoonMedicineTime[1] = tempTime2[1];
  AfternoonMedicineTime[3] = tempTime2[3];
  AfternoonMedicineTime[4] = tempTime2[4];
  AfternoonMedicineTime[6] = tempTime2[6];
  AfternoonMedicineTime[7] = tempTime2[7];
  delay(300);   
}

void sendEveningMedicineTime(){
  // Cloud'a ilac kullanim saati gonderildi
  AioMedicineTimeE.send("20:00:00");
  delay(500);

  //Cloud'dan ilac kullanim saati cekildi
  FeedData getMedicineTime3 = AioMedicineTimeE.receive();



  char *tempTime3 ;
  tempTime3 = (char *)getMedicineTime3;

 //Pointerdan char dizisine veri atimi
  EveningMedicineTime[0] = tempTime3[0];
  EveningMedicineTime[1] = tempTime3[1];
  EveningMedicineTime[3] = tempTime3[3];
  EveningMedicineTime[4] = tempTime3[4];
  EveningMedicineTime[6] = tempTime3[6];
  EveningMedicineTime[7] = tempTime3[7];
  delay(300);  
}
