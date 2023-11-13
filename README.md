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



# 使い方 (暫定版)
## 必要なもの
- ソフト
  - TeraTermや[SerialMonitorEssential](https://github.com/771-8bit/SerialMonitorEssential)のようなシリアルモニタ

- ハード  
  - ロードセル
    - 基板上の端子台の表記を参考に接続する。色の意味は以下の通り。
        - 赤 : 印加電圧+
        - 黒 : 印加電圧-
        - 緑 : 出力信号+
        - 白 : 出力信号ｰ
  - USBケーブル
    - PCと接続する。電源もUSB経由で給電される。

## 使い方
1. LoadCell-Logger本体とPCをUSBで接続しシリアルモニタを起動する。  
   baudrate : 115200bps
2. シリアルモニタにカンマ区切りの12列のデータが自動で送られてくる。
   各データは左から1chから4chの順に並んでいる。
   - 1～4列目　ADC出力の生の値
   - 5～8列目　[gf]に換算した荷重　変換係数は[LoadCell-Logger.ino](src\LoadCell-Logger\LoadCell-Logger.ino)の先頭で定義されている。
   - 9～12列目　前回の表示から計測が更新された回数。1ならば正常。2以上の場合は読み飛ばしが発生している。
   - 更新間隔　100Hz(10ms)ごとに1回(1行)更新される。
3. Aボタンを押すとオフセット調整ができる。
   - Aボタンを押した瞬間の値が原点として認識される。
   - ロードセルを荷重0の状態にしてAボタンを押すことで原点の調整が完了する。
4. シリアルモニタの機能を使用してログを記録する。
   - カンマ区切りのデータが出力されるので、そのままcsvファイルとして保存できる。