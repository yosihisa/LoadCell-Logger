//---------------------------------------------------//
// ロードセル　シングルポイント（ ビーム型）　ＳＣ６０１　１２０ｋＧ [P-12035]
//---------------------------------------------------//
//#define OUT_VOL   0.001f      //定格出力 [V]
//#define LOAD      120000.0f   //定格容量 [g]

//---------------------------------------------------//
// ロードセル　シングルポイント（ ビーム型）　ＳＣ１３３　２０ｋＧ [P-12034]
//---------------------------------------------------//
#define OUT_VOL 0.002f  //定格出力 [V]
#define LOAD 20000.0f   //定格容量 [g]

//---------------------------------------------------//
// ロードセル　シングルポイント（ビーム型）　ＳＣ６１６Ｃ　５００ｇ[P-12532]
//---------------------------------------------------//
//#define OUT_VOL   0.0007f   //定格出力 [V]
//#define LOAD      500.0f    //定格容量 [g]


#define HX711_AVDD 4.000f // ( 1.25*(( 22k + 10k )/ 10 ) )
#define HX711_ADC1bit ((HX711_AVDD / 16777216))
#define HX711_SCALE (OUT_VOL * HX711_AVDD / LOAD * 128)

#define OFFSET_AVERAGING 100

//----------ピン定義----------
#define CH1_DATA  14
#define CH1_SCK   15
#define CH2_DATA  12
#define CH2_SCK   13
#define CH3_DATA  10
#define CH3_SCK   11
#define CH4_DATA  8
#define CH4_SCK   9
#define LED1      18
#define LED2      19
#define LED3      20
#define LED4      21
#define SW_A      22
#define SW_B      26
//----------ピン定義----------

#define NUM_CH  4
#define DOUT    0
#define SCK     1

int hx711_pin[NUM_CH][2] = {
    {CH1_DATA, CH1_SCK},
    {CH2_DATA, CH2_SCK},
    {CH3_DATA, CH3_SCK},
    {CH4_DATA, CH4_SCK}};

enum HX711_State {
  HX711_RESET = 0,
  HX711_WAITING,
  HX711_READY
};

enum HX711_State hx711_state[NUM_CH];

enum State {
  READ = 0,
  SET_OFFSET
};

enum State state;

int32_t raw[NUM_CH] = {0};     //計測結果 生データ
int32_t offset[NUM_CH] = {0};  //オフセット
uint32_t read[NUM_CH] = {0};   //読み出し完了フラグ

void setup() {
  Serial.begin(115200);
  delay(1000);

  for (int i = 0; i < NUM_CH;i++){
    pinMode(hx711_pin[i][DOUT], INPUT);
    pinMode(hx711_pin[i][SCK], OUTPUT);
  }

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(PIN_LED, OUTPUT);

  pinMode(SW_A, INPUT_PULLUP);
  pinMode(SW_B, INPUT_PULLUP);


  // HX711 SCKをHIGHにしてリセット状態にする
  for (int i = 0; i < NUM_CH; i++) {
    digitalWrite(hx711_pin[i][SCK], HIGH);
  }

  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
  digitalWrite(LED3, HIGH);
  digitalWrite(LED4, HIGH);

  state = READ;
}

void loop() {
  int32_t data[4];
  double gram[4];

  switch (state) {
    case READ:
      digitalWrite(PIN_LED, HIGH);
      if ((read[0] != 0) & (read[1] != 0) & (read[2] != 0) & (read[3] != 0)) {
        for (int i = 0; i < NUM_CH; i++) {
          data[i] = raw[i] - offset[i];
          gram[i] = (double)data[i] * HX711_ADC1bit / HX711_SCALE;
        }

        Serial.printf("%+8d,%+8d,%+8d,%+8d,  %+12.5f,%+12.5f,%+12.5f,%+12.5f,  %d,%d,%d,%d\n",
                      data[0], data[1], data[2], data[3],
                      gram[0], gram[1], gram[2], gram[3],
                      read[0], read[1], read[2], read[3]);

        read[0] = 0;
        read[1] = 0;
        read[2] = 0;
        read[3] = 0;
      }

      if (digitalRead(SW_A) == LOW) {
        state = SET_OFFSET;
      }
      break;

    case SET_OFFSET:
      digitalWrite(PIN_LED, LOW);
      Serial.println("SET_OFFSET");
      for (int ch = 0; ch < NUM_CH; ch++) {
        offset[ch] = 0;
      }

      for (int i = 0; i < OFFSET_AVERAGING; i++) {
        for (int ch = 0; ch < NUM_CH; ch++) {
          offset[ch] += raw[ch];
        }
        delay(10);
      }

      for (int ch = 0; ch < NUM_CH; ch++) {
        offset[ch] = offset[ch] / OFFSET_AVERAGING;
      }

      state = READ;
      break;

    default:
      digitalWrite(PIN_LED, LOW);
      break;
  }
}



// core1は計測に専念させる
void setup1() {

  //core 0の初期化処理待ち
  delay(2000);
  

  // HX711 初期化
  // SCKをLOWにして計測を開始する
  for (int ch = 0; ch < NUM_CH; ch++) {
    read[ch] = 0;
    hx711_state[ch] = HX711_RESET;
    digitalWrite(hx711_pin[ch][SCK], LOW);
  }

  Serial.println("HX711 start!");
}

void loop1() {

  for (int ch = 0; ch < NUM_CH; ch++) {
    int32_t data = 0;

    switch (hx711_state[ch]) {

      // 初期状態 初回の計測が開始したら計測待機中へ
      // データシートには書かれてないが計測する瞬間にDOUTがHIGHになる
      case HX711_RESET:
        if (digitalRead(hx711_pin[ch][DOUT]) == HIGH) {
          //Serial.printf("%d start! \n",ch);
          hx711_state[ch] = HX711_WAITING;
        }
        break;

      // 計測待機中
      // DOUTがLOWになったら計測完了
      case HX711_WAITING:
        if (digitalRead(hx711_pin[ch][DOUT]) == LOW) {
          hx711_state[ch] = HX711_READY;
        }
        break;

      // 計測完了
      // データを読みだして次の計測待機へ
      case HX711_READY:

        for (int i = 0; i < 24; i++) {
          digitalWrite(hx711_pin[ch][SCK], HIGH);
          data = (data << 1) | (digitalRead(hx711_pin[ch][DOUT]));
          digitalWrite(hx711_pin[ch][SCK], LOW);
        }
        digitalWrite(hx711_pin[ch][SCK], HIGH);
        digitalWrite(hx711_pin[ch][SCK], LOW);

        raw[ch] = data ^ 0x800000;
        read[ch]++;
        hx711_state[ch] = HX711_WAITING;

        break;

      default:
        Serial.println("Error!");
        break;
    }
  }
}