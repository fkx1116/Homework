/*
 * 作业3：使用millis()函数控制LED产生SOS闪烁信息
 * SOS信号：短闪3次(● ● ●) + 长闪3次(■■■) + 短闪3次(● ● ●)
 * 
 * 时间参数：
 * - 短闪: 200ms 亮, 200ms 灭
 * - 长闪: 600ms 亮, 200ms 灭
 * - 字母间隔: 600ms
 * - 单词间隔: 2000ms
 * 
 * 使用状态机 + millis() 实现非阻塞控制
 */

const int LED_PIN = 2;

// 时间常量（毫秒）
const int SHORT_ON = 200;    // 短闪亮
const int SHORT_OFF = 200;   // 短闪灭
const int LONG_ON = 600;     // 长闪亮
const int LONG_OFF = 200;    // 长闪灭
const int LETTER_GAP = 600;  // 字母间隔
const int WORD_GAP = 2000;   // 单词间隔

// SOS状态机枚举
enum SOSState {
  S_SHORT_1,      // 短闪1
  S_SHORT_2,      // 短闪2
  S_SHORT_3,      // 短闪3
  S_GAP_AFTER_S,  // 短闪后间隔（字母间隔）
  S_LONG_1,       // 长闪1
  S_LONG_2,       // 长闪2
  S_LONG_3,       // 长闪3
  S_GAP_AFTER_O,  // 长闪后间隔（字母间隔）
  S_SHORT_4,      // 短闪4
  S_SHORT_5,      // 短闪5
  S_SHORT_6,      // 短闪6
  S_WORD_GAP      // 单词间隔
};

SOSState currentState = S_SHORT_1;
unsigned long stateStartTime = 0;
bool ledOn = false;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  Serial.println("=== 作业3: millis实现SOS信号 ===");
  Serial.println("SOS: 短闪3次 + 长闪3次 + 短闪3次");
  Serial.println("等待SOS信号开始...");
  
  stateStartTime = millis();
}

void loop() {
  unsigned long currentMillis = millis();
  unsigned long elapsed = currentMillis - stateStartTime;
  
  switch(currentState) {
    // ========== 短闪状态 ==========
    case S_SHORT_1:
    case S_SHORT_2:
    case S_SHORT_3:
    case S_SHORT_4:
    case S_SHORT_5:
    case S_SHORT_6:
      if (!ledOn) {
        // 点亮LED
        digitalWrite(LED_PIN, HIGH);
        ledOn = true;
        stateStartTime = currentMillis;
        Serial.print(".");
      } else if (elapsed >= SHORT_ON) {
        // 熄灭LED
        digitalWrite(LED_PIN, LOW);
        ledOn = false;
        stateStartTime = currentMillis;
        currentState = (SOSState)(currentState + 1);
      }
      break;
    
    // ========== 长闪状态 ==========
    case S_LONG_1:
    case S_LONG_2:
    case S_LONG_3:
      if (!ledOn) {
        digitalWrite(LED_PIN, HIGH);
        ledOn = true;
        stateStartTime = currentMillis;
        Serial.print("-");
      } else if (elapsed >= LONG_ON) {
        digitalWrite(LED_PIN, LOW);
        ledOn = false;
        stateStartTime = currentMillis;
        currentState = (SOSState)(currentState + 1);
      }
      break;
    
    // ========== 短闪后间隔（字母间隔） ==========
    case S_GAP_AFTER_S:
      if (elapsed >= LETTER_GAP) {
        stateStartTime = currentMillis;
        currentState = S_LONG_1;
        Serial.println("");  // 换行
        Serial.print("O: ");
      }
      break;
    
    // ========== 长闪后间隔（字母间隔） ==========
    case S_GAP_AFTER_O:
      if (elapsed >= LETTER_GAP) {
        stateStartTime = currentMillis;
        currentState = S_SHORT_4;
        Serial.println("");  // 换行
        Serial.print("S: ");
      }
      break;
    
    // ========== 单词间隔 ==========
    case S_WORD_GAP:
      if (elapsed >= WORD_GAP) {
        stateStartTime = currentMillis;
        currentState = S_SHORT_1;
        Serial.println("");
        Serial.println("=== SOS 完成 ===");
        Serial.println("等待下一个SOS信号...");
        Serial.print("S: ");
      }
      break;
  }
}