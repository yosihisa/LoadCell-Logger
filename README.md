# LoadCell-Logger
4ch load cell logger board using Raspberry Pi Pico and HX711.  (Wakame Edition)

# ロードセルロガー
HX711とRaspberry Pi Picoを使用した4チャンネルロードセルロガーです。


# ソフトウェア開発環境

## 必要なもの
- Arduino IDE  
- 追加ボード [earlephilhower/arduino-pico](https://github.com/earlephilhower/arduino-pico) 

 Arduino公式のライブラリではマルチコアを使えないためこちら使用する。
 

## 設定方法 (環境構築)

### ①ボードの準備
ArduinoIDEの「ファイル」>「環境設定」から「追加のボードマネージャのURL」を開き以下のURLを追加する。
> https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json

ArduinoIDEの「ツール」> 「ボード」> 「ボードマネージャ」から `Raspberry Pi Pico/RP2040` をインストールする。

### ②ボードの設定  
ArduinoIDEから次のボードを設定する。  
`ボード` > `Raspberry Pi RP2040 Boards` > `Raspberry Pi Pico`  

コンパイルオプションは標準のままでよい。  
(Flash Size :2MB(noFS) , CPU Speed :125MHz , 各種デバッグ: 無効 , USB Stack: Pico SDK)


