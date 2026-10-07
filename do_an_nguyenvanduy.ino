#define BLYNK_TEMPLATE_ID "TMPL66swkFr5o"
#define BLYNK_TEMPLATE_NAME "Smart Farm"
#define BLYNK_AUTH_TOKEN "ZYlq_tuPGVOlJoIzjaNvoCwJ0CF42NiO"

#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <BH1750.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_Sensor.h>
#include <RTClib.h>
#include "wifiConfig.h"
//NÚT CHẾ ĐỘ
#define NUT1 5
int ttnut;
unsigned long hientai = 0;
unsigned long thoigian;
int trangthai = 0;
int aa = 0;
bool blynkConnect=0;
// #define NUT2 35
int THNUT1, THNUT2;
// CẢM BIẾN ẨM
#define amdat 26
int luong_am_dat;
// CẢM BIẾN MƯA
#define mua 25
int luong_nuoc;
// CÁC RELAY
#define RL1 2   //dc
#define RL2 4   // dc
#define RL3 18  //dc
#define RL4 19  //dc
#define RL5 23  // đèn
#define RL6 13  // quạt
#define RL7 14  // sương
#define RL8 27  // máy bơm

BLYNK_CONNECTED(){
  Blynk.syncVirtual(V3, V4, V5, V6); // ham tu dong chay khi ket noi thanh cong
}
BLYNK_WRITE(V3){
  int p = param.asInt();
  digitalWrite(RL6, p); // dieu khien relay6 quat qua chan ao V3
}
BLYNK_WRITE(V4){
  int p = param.asInt();
  digitalWrite(RL8, p); // dieu khien relay8 may bom qua chan ao V3
}
BLYNK_WRITE(V5){
  int p = param.asInt();
  digitalWrite(RL7, p); // dieu khien relay7 phun suong qua chan ao V3
}
BLYNK_WRITE(V6){
  int p = param.asInt();
  digitalWrite(RL5, p); // dieu khien relay5 den qua chan ao V3
}

// CÔNG TẮC HÀNH TRÌNH DC
#define ht1 32
#define ht2 33
int THHT1, THHT2;
// LCD
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Cảm biến
BH1750 lightMeter;
Adafruit_BMP280 bmp;
RTC_DS3231 rtc;

void setup() {
  wifiConfig.begin();
  // NÚT
  pinMode(NUT1, INPUT_PULLUP);
  // pinMode(NUT2, INPUT);
  // CÔNG TẮC
  pinMode(ht1, INPUT_PULLUP);
  pinMode(ht2, INPUT_PULLUP);
  // CẢM BIẾN
  pinMode(amdat, INPUT);
  pinMode(mua, INPUT);
  // RELAY
  pinMode(RL1, OUTPUT);
  pinMode(RL2, OUTPUT);
  pinMode(RL3, OUTPUT);
  pinMode(RL4, OUTPUT);
  pinMode(RL5, OUTPUT);
  pinMode(RL6, OUTPUT);
  pinMode(RL7, OUTPUT);
  pinMode(RL8, OUTPUT);
  Blynk.config(BLYNK_AUTH_TOKEN, "blynk.cloud", 80);
  digitalWrite(RL1, HIGH);
  digitalWrite(RL2, HIGH);
  digitalWrite(RL3, HIGH);
  digitalWrite(RL4, HIGH);
  digitalWrite(RL5, HIGH);
  digitalWrite(RL6, HIGH);
  digitalWrite(RL7, HIGH);
  digitalWrite(RL8, HIGH);
  //
  Wire.begin(21, 22);  // SDA = GPIO21, SCL = GPIO22
  Serial.begin(115200);

  // LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Dang khoi dong...");

  // BH1750
  if (lightMeter.begin()) {
    Serial.println("BH1750 OK");
  } else {
    Serial.println("BH1750 ERROR");
    lcd.setCursor(0, 1);
    lcd.print("BH1750 ERROR");
  }

  // BMP280
  if (!bmp.begin(0x76)) {  // Nếu không được, thử 0x77
    Serial.println("BMP280 ERROR");
    lcd.setCursor(0, 2);
    lcd.print("BMP280 ERROR");
  }

  // RTC DS3231
  if (!rtc.begin()) {
    Serial.println("DS3231 ERROR");
    lcd.setCursor(0, 3);
    lcd.print("DS3231 ERROR");
  }
  rtc.adjust(DateTime(2025, 8, 1, 16, 21, 0));  // Năm, tháng, ngày, giờ, phút, giây


  delay(2000);
  lcd.clear();
}

void loop() {
  wifiConfig.run();
  if(WiFi.status()==WL_CONNECTED){
    if(blynkConnect==0){
      Serial.println("Connecting to blynk cloud...!");
      if(Blynk.connect(5000)){ 
        Serial.println("Connected to blynk cloud!");
        blynkConnect=1;
      }else{
        Serial.println("Connection failed. Try again later.");
      }
    }
    if (!Blynk.connected()) blynkConnect=0;
    Blynk.run();
  }

  // Đọc cảm biến
  float lux = lightMeter.readLightLevel();
  float temp = bmp.readTemperature();
  float pres = bmp.readPressure() / 100.0F;  // hPa

  // Đọc thời gian từ DS3231
  DateTime now = rtc.now();
  char timeStr[9];
  sprintf(timeStr, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());

  // Hiển thị LCD
  lcd.setCursor(0, 0);
  lcd.print("Nhiet do: ");
  lcd.print(temp, 1);
  lcd.print(" C");

  lcd.setCursor(0, 1);
  lcd.print("Ap suat : ");
  lcd.print(pres, 0);
  lcd.print(" hPa");

  lcd.setCursor(0, 2);
  lcd.print("Anh sang: ");
  lcd.print((int)lux);
  lcd.print(" lx     ");

  lcd.setCursor(0, 3);
  lcd.print("Time: ");
  lcd.print(timeStr);


  luong_am_dat = digitalRead(amdat);
  luong_nuoc = digitalRead(mua);
  THNUT1 = digitalRead(NUT1);

  THHT1 = digitalRead(ht1);
  THHT2 = digitalRead(ht2);
  Serial.print("CTAC: ");
  Serial.print(THHT1);
  Serial.print("___");
  Serial.print(THHT2);
  Serial.print("|");
  Serial.print("CBIEN: ");
  Serial.print(luong_am_dat);
  Serial.print("___");
  Serial.print(luong_nuoc);
  Serial.print("|");
  Serial.print("NUT: ");
  Serial.print(THNUT1);
  Serial.println(".");
  // delay(1000); // Cập nhật mỗi giây
  if (lux < 20) {
    digitalWrite(RL5, LOW);
  } else {
    digitalWrite(RL5, HIGH);
  }
  if (luong_am_dat == 1) {
    digitalWrite(RL7, LOW);
  } else {
    digitalWrite(RL7, HIGH);
  }  // BẬT PHUN SƯƠNG
  if (temp > 20) {
    digitalWrite(RL6, LOW);
  } else {
    digitalWrite(RL6, HIGH);
  }
  // CHẾ ĐỘ KÉO MÁI
  if (luong_nuoc == 0) {
    if (THHT1 == 1) {
      digitalWrite(RL1, LOW);
      digitalWrite(RL2, LOW);
      digitalWrite(RL3, HIGH);
      digitalWrite(RL4, HIGH);
    } else {
      digitalWrite(RL1, HIGH);
      digitalWrite(RL2, HIGH);
      digitalWrite(RL3, HIGH);
      digitalWrite(RL4, HIGH);
    }
  } else {
    if (THHT2 == 1) {
      digitalWrite(RL3, LOW);
      digitalWrite(RL4, LOW);
      digitalWrite(RL1, HIGH);
      digitalWrite(RL2, HIGH);
    } else {
      digitalWrite(RL1, HIGH);
      digitalWrite(RL2, HIGH);
      digitalWrite(RL3, HIGH);
      digitalWrite(RL4, HIGH);
    }
  }
  // CHẾ ĐỘ CÀI ĐẶT THỜI GIAN
  if (THNUT1 == 0) {
    trangthai = 1;
  }
  if (trangthai == 1) {
    hientai = millis();
    trangthai = 2;
  }
  if (trangthai == 2) {
    thoigian = millis();
    if (thoigian - hientai < 1000) {
      lcd.setCursor(16, 3);
      lcd.print("1s ");
    } else if (thoigian - hientai < 2000) {
      lcd.setCursor(16, 3);
      lcd.print("2s ");
    } else if (thoigian - hientai < 3000) {
      lcd.setCursor(16, 3);
      lcd.print("3s ");
    } else if (thoigian - hientai < 4000) {
      lcd.setCursor(16, 3);
      lcd.print("4s ");
    } else if (thoigian - hientai < 5000) {
      lcd.setCursor(16, 3);
      lcd.print("5s ");
    } else if (thoigian - hientai < 6000) {
      lcd.setCursor(16, 3);
      lcd.print("6s ");
    } else if (thoigian - hientai < 7000) {
      lcd.setCursor(16, 3);
      lcd.print("7s ");
    } else if (thoigian - hientai < 8000) {
      lcd.setCursor(16, 3);
      lcd.print("8s ");
    } else if (thoigian - hientai < 9000) {
      lcd.setCursor(16, 3);
      lcd.print("9s ");
    } else if (thoigian - hientai < 10000) {
      lcd.setCursor(16, 3);
      lcd.print("10s");
    } else if (thoigian - hientai < 11000) {
      lcd.setCursor(16, 3);
      lcd.print("11s");
    } else if (thoigian - hientai < 12000) {
      lcd.setCursor(16, 3);
      lcd.print("12s");
    } else if (thoigian - hientai < 13000) {
      lcd.setCursor(16, 3);
      lcd.print("13s");
    } else if (thoigian - hientai < 14000) {
      lcd.setCursor(16, 3);
      lcd.print("14s");
    } else if (thoigian - hientai < 15000) {
      lcd.setCursor(16, 3);
      lcd.print("15s");
    } else if (thoigian - hientai < 16000) {
      lcd.setCursor(16, 3);
      lcd.print("16s");
    } else if (thoigian - hientai < 17000) {
      lcd.setCursor(16, 3);
      lcd.print("17s");
    } else if (thoigian - hientai < 18000) {
      lcd.setCursor(16, 3);
      lcd.print("18s");
    } else if (thoigian - hientai < 19000) {
      lcd.setCursor(16, 3);
      lcd.print("19s");
    } else if (thoigian - hientai < 20000) {
      lcd.setCursor(16, 3);
      lcd.print("20s");
    } else if (thoigian - hientai < 21000) {
      lcd.setCursor(16, 3);
      lcd.print("21s");
    } else if (thoigian - hientai < 22000) {
      lcd.setCursor(16, 3);
      lcd.print("22s");
    } else if (thoigian - hientai < 23000) {
      lcd.setCursor(16, 3);
      lcd.print("23s");
    } else if (thoigian - hientai < 24000) {
      lcd.setCursor(16, 3);
      lcd.print("24s");
    } else if (thoigian - hientai < 25000) {
      lcd.setCursor(16, 3);
      lcd.print("25s");
    } else if (thoigian - hientai < 26000) {
      lcd.setCursor(16, 3);
      lcd.print("26s");
    } else if (thoigian - hientai < 27000) {
      lcd.setCursor(16, 3);
      lcd.print("27s");
    } else if (thoigian - hientai < 28000) {
      lcd.setCursor(16, 3);
      lcd.print("28s");
    } else if (thoigian - hientai < 29000) {
      lcd.setCursor(16, 3);
      lcd.print("29s");
    } else if (thoigian - hientai < 30000) {
      lcd.setCursor(16, 3);
      lcd.print("30s");
    } else if (thoigian - hientai < 31000) {
      lcd.setCursor(16, 3);
      lcd.print("31s");
    } else if (thoigian - hientai < 32000) {
      lcd.setCursor(16, 3);
      lcd.print("32s");
    } else if (thoigian - hientai < 33000) {
      lcd.setCursor(16, 3);
      lcd.print("33s");
    } else if (thoigian - hientai < 34000) {
      lcd.setCursor(16, 3);
      lcd.print("34s");
    } else if (thoigian - hientai < 35000) {
      lcd.setCursor(16, 3);
      lcd.print("35s");
    } else if (thoigian - hientai < 36000) {
      lcd.setCursor(16, 3);
      lcd.print("36s");
    } else if (thoigian - hientai < 37000) {
      lcd.setCursor(16, 3);
      lcd.print("37s");
    } else if (thoigian - hientai < 38000) {
      lcd.setCursor(16, 3);
      lcd.print("38s");
    } else if (thoigian - hientai < 39000) {
      lcd.setCursor(16, 3);
      lcd.print("39s");
    } else if (thoigian - hientai < 40000) {
      lcd.setCursor(16, 3);
      lcd.print("40s");
    } else if (thoigian - hientai < 41000) {
      lcd.setCursor(16, 3);
      lcd.print("41s");
    } else if (thoigian - hientai < 42000) {
      lcd.setCursor(16, 3);
      lcd.print("42s");
    } else if (thoigian - hientai < 43000) {
      lcd.setCursor(16, 3);
      lcd.print("43s");
    } else if (thoigian - hientai < 44000) {
      lcd.setCursor(16, 3);
      lcd.print("44s");
    } else if (thoigian - hientai < 45000) {
      lcd.setCursor(16, 3);
      lcd.print("45s");
    } else if (thoigian - hientai < 46000) {
      lcd.setCursor(16, 3);
      lcd.print("46s");
    } else if (thoigian - hientai < 47000) {
      lcd.setCursor(16, 3);
      lcd.print("47s");
    } else if (thoigian - hientai < 48000) {
      lcd.setCursor(16, 3);
      lcd.print("48s");
    } else if (thoigian - hientai < 49000) {
      lcd.setCursor(16, 3);
      lcd.print("49s");
    } else if (thoigian - hientai < 50000) {
      lcd.setCursor(16, 3);
      lcd.print("50s");
    } else if (thoigian - hientai < 51000) {
      lcd.setCursor(16, 3);
      lcd.print("51s");
    } else if (thoigian - hientai < 52000) {
      lcd.setCursor(16, 3);
      lcd.print("52s");
    } else if (thoigian - hientai < 53000) {
      lcd.setCursor(16, 3);
      lcd.print("53s");
    } else if (thoigian - hientai < 54000) {
      lcd.setCursor(16, 3);
      lcd.print("54s");
    } else if (thoigian - hientai < 55000) {
      lcd.setCursor(16, 3);
      lcd.print("55s");
    } else if (thoigian - hientai < 56000) {
      lcd.setCursor(16, 3);
      lcd.print("56s");
    } else if (thoigian - hientai < 57000) {
      lcd.setCursor(16, 3);
      lcd.print("57s");
    } else if (thoigian - hientai < 58000) {
      lcd.setCursor(16, 3);
      lcd.print("58s");
    } else if (thoigian - hientai < 59000) {
      lcd.setCursor(16, 3);
      lcd.print("59s");
    } else if (thoigian - hientai < 60000) {
      lcd.setCursor(16, 3);
      lcd.print("60s");
    }

    else if (thoigian - hientai < 61000) {
      lcd.setCursor(16, 3);
      lcd.print("1s");
      digitalWrite(RL8, LOW);
    } else if (thoigian - hientai < 62000) {
      lcd.setCursor(16, 3);
      lcd.print("2s");
      digitalWrite(RL8, LOW);
    } else if (thoigian - hientai < 63000) {
      lcd.setCursor(16, 3);
      lcd.print("3s");
      digitalWrite(RL8, LOW);
    } else if (thoigian - hientai < 64000) {
      lcd.setCursor(16, 3);
      lcd.print("4s");
      digitalWrite(RL8, LOW);
    } else if (thoigian - hientai < 65000) {
      lcd.setCursor(16, 3);
      lcd.print("5s");
      digitalWrite(RL8, LOW);
    } else if (thoigian - hientai < 66000) {
      lcd.setCursor(16, 3);
      lcd.print("6s");
      digitalWrite(RL8, LOW);
    } else if (thoigian - hientai < 67000) {
      lcd.setCursor(16, 3);
      lcd.print("7s");
      digitalWrite(RL8, LOW);
    } else if (thoigian - hientai < 68000) {
      lcd.setCursor(16, 3);
      lcd.print("8s");
      digitalWrite(RL8, LOW);
    } else if (thoigian - hientai < 69000) {
      lcd.setCursor(16, 3);
      lcd.print("9s");
      digitalWrite(RL8, LOW);
    } else if (thoigian - hientai < 70000) {
      lcd.setCursor(16, 3);
      lcd.print("10s");
      digitalWrite(RL8, LOW);
    } else if (thoigian - hientai < 71000) {
      lcd.setCursor(16, 3);
      lcd.print("11s");
      digitalWrite(RL8, LOW);
    } else if (thoigian - hientai < 72000) {
      lcd.setCursor(16, 3);
      lcd.print("12s");
      digitalWrite(RL8, LOW);
    } else if (thoigian - hientai < 73000) {
      lcd.setCursor(16, 3);
      lcd.print("13s");
      digitalWrite(RL8, LOW);
    } else if (thoigian - hientai < 74000) {
      lcd.setCursor(16, 3);
      lcd.print("14s");
      digitalWrite(RL8, LOW);
    } else if (thoigian - hientai < 75000) {
      lcd.setCursor(16, 3);
      lcd.print("15s");
      digitalWrite(RL8, LOW);
    } else if (thoigian - hientai < 76000) {
      lcd.setCursor(16, 3);
      lcd.print("16s");
      digitalWrite(RL8, LOW);
    } else if (thoigian - hientai < 77000) {
      lcd.setCursor(16, 3);
      lcd.print("17s");
      digitalWrite(RL8, LOW);
    } else if (thoigian - hientai < 78000) {
      lcd.setCursor(16, 3);
      lcd.print("18s");
      digitalWrite(RL8, LOW);
    } else if (thoigian - hientai < 79000) {
      lcd.setCursor(16, 3);
      lcd.print("19s");
      digitalWrite(RL8, LOW);
    } else if (thoigian - hientai < 80000) {
      lcd.setCursor(16, 3);
      lcd.print("20s");
      digitalWrite(RL8, LOW);
    } else if (thoigian - hientai < 81000) {
      lcd.setCursor(16, 3);
      lcd.print("OFF");
      digitalWrite(RL8, HIGH);
    }
    // else if(thoigian - hientai < 82000){lcd.setCursor(16, 3);Serial.println("OFF");digitalWrite(RL8, LOW);}
    else { trangthai = 3; }
  }
}

