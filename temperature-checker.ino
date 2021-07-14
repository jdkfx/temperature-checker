#include <M5StickC.h>
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
  M5.Lcd.setCursor(0,0);
  M5.Lcd.printf("Hello, world.");
}

void loop() {
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
