const int anode_pins[] = {7, 8, 9, 10, 11, 12, 13};    // アノードに接続するArduinoのピン
const int cathode_pins[] = {19, 18, 17, 16, 15, 14};  // カソードに接続するArduinoのピン

const int number_of_anode_pins = sizeof(anode_pins) / sizeof(anode_pins[0]);
const int number_of_cathode_pins = sizeof(cathode_pins) / sizeof(cathode_pins[0]);
long numbers_to_display = 0; // LEDに表示する数字を保持する変数

long min10 = 6;
long min01 = 0;
long sec10 = 0;
long sec01 = 0;
long dotSec10 = 0;
long dotSec01 = 0;
long totalTime = 0;

const int digits[] = {

  0b01000000, // 0
  0b01111001, // 1
  0b00100100, // 2
  0b00110000, // 3
  0b00011001, // 4
  0b00010010, // 5
  0b00000010, // 6
  0b01011000, // 7
  0b00000000, // 8
  0b00010000, // 9

};

// 1桁の数字(n)を表示する
void display_number (long n) {
  for (int i = 0; i < number_of_anode_pins; i++) {
    digitalWrite(anode_pins[i], digits[n] & (1 << i) ? HIGH : LOW);
  }
}

// アノードをすべてLOWにする
void clear_segments() {
  for (int j = 0; j < number_of_anode_pins; j++) {
    digitalWrite(anode_pins[j], HIGH);
  }
}

void display_numbers() {
  long n = numbers_to_display;  // number_to_displayの値を書き換えないために変数にコピー
  for (int i = 0; i < number_of_cathode_pins; i++) {
    digitalWrite(cathode_pins[i], HIGH);
    display_number(n % 10); // 最後の一桁を表示する
    delayMicroseconds(10);
    // 全桁消灯
    clear_segments();
    // 一桁点灯
    digitalWrite(cathode_pins[i], LOW);
    n = n / 10; // 10で割る
  }
}

void set_numbers(long n) {
  numbers_to_display = n;
}
 
// setup()　は，最初に一度だけ実行される
void setup() {
Serial.begin(9600);
Serial.println("DEBUG DEBUG DEBUG");
  for (int i = 0; i < number_of_anode_pins; i++) {
    pinMode(anode_pins[i], OUTPUT);  // anode_pinsを出力モードに設定する
  }
  for (int i = 0; i < number_of_cathode_pins; i++) {
    pinMode(cathode_pins[i], OUTPUT);  // cathode_pinを出力モードに設定する
    digitalWrite(cathode_pins[i], HIGH);
  }

  // f = クロック周波数 / ( 2 * 分周比　*　( 1 + 比較レジスタの値))
  // 分周比=32, 比較レジスタの1値=255 -> f = 16000000 / (2 * 32 * 256) = 976 Hz
  OCR2A = 255; // 255クロックごとに割り込みをかける
  TCCR2B = 0b100; // 分周比を32に設定する
  bitWrite(TIMSK2, OCIE2A, 1); // TIMER2を許可する
  delay(10);
}

void loop () {

  // min10 count down
  if(sec01 <= 0 && sec10 <= 0 && min01 <= 0 && min10 > 0){
    min10--;
    min01 = 10;
  }

  // min01 count down
  if(sec01 <= 0 && sec10 <= 0 && dotSec01 <= 0 && dotSec10 <= 0 && min01 > 0){
    min01--;
    sec10 = 6;
  }

  // sec10 count down
  if(sec01 <= 0 && sec10 > 0){
    sec10--;
    sec01 = 10;
  }

  // sec01 count down
  if(dotSec01 <= 0 && dotSec10 <= 0 && sec01 > 0){
    sec01--;
    dotSec10 = 10;
  }

  // dotSec10 count down
  if(dotSec01 <= 0 && dotSec10 > 0){
    dotSec10--;
    dotSec01 = 10;
  }

  // dotSec01 count down
  if(min10 == 0 && min01 == 0 && sec10 == 0 && sec01 == 0 && dotSec10 == 0 && dotSec01 == 0){
  } else if(dotSec01 > 0){
   dotSec01--;
  }

  totalTime = dotSec01;
  totalTime += (dotSec10 * 10);
  totalTime += (sec01 * 100);
  totalTime += (sec10 * 1000);
  totalTime += (min01 * 10000);
  totalTime += (min10 * 100000);

  set_numbers(totalTime);

  delay(10);
//Serial.println(totalTime);
}

ISR(TIMER2_COMPA_vect) {
  display_numbers();
}
