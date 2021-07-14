#include <M5StickC.h>
#include <time.h>
#include <WiFi.h>
#include "Config.h"
#include <Adafruit_SHT31.h>
Adafruit_SHT31 Sht = Adafruit_SHT31();
#include <Adafruit_BMP280.h>
Adafruit_BMP280 Bmp;
#include <WebServer.h>
#include <ESPmDNS.h>
#define checkResetOn()  if(M5.Axp.GetBtnPress()==2){esp_restart();}

// WEBサーバー
const char* Host = "localhost";
WebServer Srv(80);
char SrvMsg[256] = "hostname,vol,temp";

// ブザー
const int spPin = 0;
const int spPWMch = 0;

void setup() {
  // 文字列の表示
  M5.begin();
  M5.Lcd.print("Hello, world.");

  // ディスプレイの表示方法
  M5.Axp.ScreenBreath(9);
  M5.Lcd.setRotation(3);
  M5.Lcd.fillScreen(BLUE);
  M5.Lcd.setTextColor(YELLOW,BLUE);
  M5.Lcd.setTextFont(2);
  M5.Lcd.setTextSize(1);

  // ブザー
  ledcSetup(spPWMch,5000,13);
  ledcAttachPin(spPin,spPWMch);

  // WiFiとの接続
  M5.Lcd.setCursor(0,0);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  uint8_t retry = 0;
  while(WiFi.status() != WL_CONNECTED && retry++ < 30){
    delay(100);
  }
  if (retry == 26) {
    M5.Lcd.print("Not Connected");
    while (1) checkResetOn();
  }
  M5.Lcd.print(WiFi.localIP());

  // 時間の表示
  configTime(9*3600, 0, "ntp.nict.jp");
  struct tm _tm;
  if(getLocalTime(&_tm)){
    RTC_TimeTypeDef ts; RTC_DateTypeDef ds;
    ts.Hours = _tm.tm_hour; ts.Minutes = _tm.tm_min; ts.Seconds = _tm.tm_sec;
    ds.Year = _tm.tm_year + 1900; ds.Month = _tm.tm_mon+1; ds.Date = _tm.tm_mday;
    M5.Rtc.SetTime(&ts); M5.Rtc.SetData(&ds);
  }

  // WEBサーバー
  MDNS.begin(Host);
  Srv.on("/", HTTP_GET, [](){
    Srv.sendHeader("Access-Control-Allow-Origin", "*");
    Srv.send(200, "text/plain", SrvMsg);
  });
  Srv.begin();
  MDNS.addService("http", "tcp", 80);

  // 環境センサー
  Sht.begin(0x44);
  Bmp.begin(0x76);
}

void loop() {
  // ブザー
  M5.update();
  if(M5.BtnB.wasPressed()){
    ledcWriteNote(spPWMch,NOTE_B,5);
    delay(100);
    ledcWriteNote(spPWMch,NOTE_E,6);
    delay(500);
    ledcWrite(spPWMch,0);
  }
  
  // 日時の表示
  RTC_DateTypeDef rtc_d;
  M5.Rtc.GetData(&rtc_d);
  M5.Lcd.setCursor(0, 16);
  M5.Lcd.printf("Date %04d-%02d-%02d", rtc_d.Year, rtc_d.Month, rtc_d.Date);
  RTC_TimeTypeDef rtc_t;
  M5.Rtc.GetTime(&rtc_t);
  M5.Lcd.setCursor(0, 32);
  M5.Lcd.printf("Time %02d-%02d-%02d    ", rtc_t.Hours, rtc_t.Minutes, rtc_t.Seconds);

  // 環境状態の確認
  float tp = Sht.readTemperature();    // 温度
  float hm = Sht.readHumidity();      // 湿度
  float ps = Bmp.readPressure()*0.01;   // 気圧 hPa = Pa *0.01
  M5.Lcd.setCursor(0, 48);
  M5.Lcd.printf("%4.1f%cC %4.1f%% %6.1fhPa ", tp,0xf7,hm,ps);
  
  // バッテリー状態の確認
  float v,a,t;
  v = M5.Axp.GetBatVoltage();   // 電圧
  a = M5.Axp.GetBatCurrent();   // 電流
  t = M5.Axp.GetTempInAXP192(); // 温度
  M5.Lcd.setCursor(0,64);
  M5.Lcd.printf("Bat %3.1fV %5.1fmA %4.1fC ",v,a,t);

  // WEBサーバー
  sprintf(SrvMsg,"%s,%4.1f%cC %4.1f%% %6.1fhPa", Host,tp,0xf7,hm,ps);
  Srv.handleClient();

  // 電源のリセット
  checkResetOn();
  delay(10);
}
