#define LED_BULITIN 25

#define MAX7219_DIN 0
#define MAX7219_CS  1
#define MAX7219_CLK 2

void setup() {
  pinMode(LED_BULITIN, OUTPUT);

  MAX7216_init();
}

void setup1() {
}

void loop() {
  // LED のテスト用コード:
  LED_OUT(0x1, random(10));
  LED_OUT(0x2, random(10));
  LED_OUT(0x3, random(10));
  LED_OUT(0x4, random(10));
  LED_OUT(0x5, random(10));
  LED_OUT(0x6, random(10));
  LED_OUT(0x7, random(10));
  LED_OUT(0x8, random(10));

  delay(1000);
}

void loop2() {
}

void MAX7216_init(void) {
  // GPIO のモード設定:
  pinMode(MAX7219_CS,  OUTPUT);
  pinMode(MAX7219_CLK, OUTPUT);
  pinMode(MAX7219_DIN, OUTPUT);
  
  // GPIO の初期状態設定:
  digitalWrite(MAX7219_CS,  HIGH);  // Low Active:
  digitalWrite(MAX7219_CLK,  LOW);
  digitalWrite(MAX7219_DIN,  LOW);

  // MAX7216 の初期設定:
  LED_OUT(0x9, 0xff);    //7セグでデコードするビットに1を立てる:
  LED_OUT(0xA,    5);    //輝度を設定, 0-15:
  LED_OUT(0xB,    7);    //使用する桁数を指定, 桁数-1:
  LED_OUT(0xC,    1);    //特にいじる必要なし:
  LED_OUT(0xF,    0);    //同上:
}

// MAX7216 にデータを書き込む関数:
void LED_OUT(uint8_t addr, uint8_t dat){
  digitalWrite(MAX7219_CS, LOW);
  shiftOut(MAX7219_DIN, MAX7219_CLK, MSBFIRST, addr);
  shiftOut(MAX7219_DIN, MAX7219_CLK, MSBFIRST, dat);
  digitalWrite(MAX7219_CS, HIGH);
}
