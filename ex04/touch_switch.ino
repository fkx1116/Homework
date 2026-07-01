// 引脚定义
const int ledPin = 2;                // 手动指定板载LED引脚GPIO2，替代LED_BUILTIN
const int touchPin = 4;              // 触摸通道T0，对应GPIO4

// 触摸阈值：touchRead返回值低于该值判定为有效触摸
const int touchThreshold = 30;

// 软件防抖参数
const unsigned long debounceDelay = 50;  // 防抖时长，单位毫秒
unsigned long lastDebounceTime = 0;      // 上次状态变化的时间戳

// 状态变量
bool lastTouchReading = false;  // 上一轮瞬时读取的触摸状态
bool stableTouchState = false;  // 防抖确认后的稳定触摸状态
bool ledState = LOW;            // LED当前状态，初始为熄灭


void setup() {
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);  // 初始化LED状态
  Serial.begin(115200);
  Serial.println("触摸自锁开关初始化完成");
}


void loop() {
  unsigned long currentTime = millis();
  
  // 1. 读取触摸值，转换为布尔型触摸状态
  int touchValue = touchRead(touchPin);
  bool currentReading = (touchValue < touchThreshold);

  // 2. 瞬时状态发生变化时，重置防抖计时器
  if (currentReading != lastTouchReading) {
    lastDebounceTime = currentTime;
  }

  // 3. 状态稳定超过防抖时长，执行边缘检测与状态翻转
  if (currentTime - lastDebounceTime > debounceDelay) {
    // 稳定状态发生变化时才处理
    if (currentReading != stableTouchState) {
      stableTouchState = currentReading;
      
      // 仅检测「从未触摸 → 触摸」的上升沿（按下瞬间），翻转LED状态
      if (stableTouchState == true) {
        ledState = !ledState;
        digitalWrite(ledPin, ledState);
        Serial.print("LED状态翻转，当前：");
        Serial.println(ledState ? "点亮" : "熄灭");
      }
    }
  }

  // 4. 保存本次瞬时读取状态，供下一轮对比
  lastTouchReading = currentReading;
}
