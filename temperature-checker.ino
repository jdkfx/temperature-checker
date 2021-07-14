#include <M5StickC.h>
#include <time.h>
#include <WiFi.h>
#include "Config.h"
#define checkResetOn()  if(M5.Axp.GetBtnPress()==2){esp_restart();}

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
}

void loop() {
  // 日時の表示
  RTC_DateTypeDef rtc_d;
  M5.Rtc.GetData(&rtc_d);
  M5.Lcd.setCursor(0, 16);
  M5.Lcd.printf("Date %04d-%02d-%02d", rtc_d.Year, rtc_d.Month, rtc_d.Date);
  RTC_TimeTypeDef rtc_t;
  M5.Rtc.GetTime(&rtc_t);
  M5.Lcd.setCursor(0, 32);
  M5.Lcd.printf("Time %02d-%02d-%02d    ", rtc_t.Hours, rtc_t.Minutes, rtc_t.Seconds);
  
  // バッテリー状態の確認
  float v,a,t;
  v = M5.Axp.GetBatVoltage();   // 電圧
  a = M5.Axp.GetBatCurrent();   // 電流
  t = M5.Axp.GetTempInAXP192(); // 温度
  M5.Lcd.setCursor(0,64);
  M5.Lcd.printf("Bat %3.1fV %5.1fmA %4.1fC ",v,a,t);

  // 電源のリセット
  checkResetOn();
  delay(10);
}
