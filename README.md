# jdkfx/temperature-checker

M5StickCで気温・湿度を監視し，指定値を上回るとアラートでお知らせをする

## 環境について

- 基本デバイス : M5StickC
- 追加デバイス :
    - 小型スピーカー
    - 温度・湿度・気圧センサー
    - リモコン型イルミネーションライト

## Getting Started

### リポジトリをクローン

```
$ git clone
```

### Config.h の作成
```
// Config.h

// WiFi
const char* ssid = "SSID";
const char* password = "password";
```

### M5StickC端末に書き込み