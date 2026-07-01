// 引脚定义
const int ledPin = 2;                  // 板载LED引脚 GPIO2
const int touchPin = 4;                // 触摸引脚 GPIO4（T0通道）

// 触摸检测参数
const int touchThreshold = 30;         // 触摸阈值，低于该值判定为有效触摸
const unsigned long debounceDelay = 50;// 软件防抖时长，单位ms

// 触摸状态变量
unsigned long lastDebounceTime = 0;
bool lastTouchReading = false;
bool stableTouchState = false;

// 档位与呼吸灯参数
int speedLevel = 1;                    // 当前速度档位：1/2/3档，初始1档（最慢）
// 三档亮度更新间隔（单位ms），数值越小呼吸越快
// 1档≈2.5秒一个呼吸周期，2档≈1秒，3档≈0.5秒
const int fadeInterval[3] = {5, 2, 1};

int brightness = 0;                    // 当前LED亮度 0-255
int fadeStep = 1;                      // 亮度变化步长：+1渐亮，-1渐暗
unsigned long lastFadeTime = 0;        // 上次更新亮度的时间戳


void setup() {
  pinMode(ledPin, OUTPUT);
  analogWrite(ledPin, brightness);     // 初始化LED亮度
  Serial.begin(115200);
  Serial.println("三档位触摸调速呼吸灯（提速版）初始化完成");
  Serial.print("当前档位：");
  Serial.println(speedLevel);
}


void loop() {
  unsigned long currentTime = millis();

  // ========== 1. 触摸检测 + 防抖 + 上升沿档位切换 ==========
  int touchValue = touchRead(touchPin);
  bool currentReading = (touchValue < touchThreshold);

  // 瞬时状态变化时重置防抖计时器
  if (currentReading != lastTouchReading) {
    lastDebounceTime = currentTime;
  }

  // 状态稳定超过防抖时长，确认有效状态
  if (currentTime - lastDebounceTime > debounceDelay) {
    if (currentReading != stableTouchState) {
      stableTouchState = currentReading;

      // 仅在「从未触摸 → 触摸」的上升沿切换档位
      if (stableTouchState == true) {
        speedLevel++;
        if (speedLevel > 3) {
          speedLevel = 1; // 超过3档回到1档，循环
        }
        Serial.print("切换到档位：");
        Serial.println(speedLevel);
      }
    }
  }
  lastTouchReading = currentReading;


  // ========== 2. 非阻塞式PWM呼吸灯 ==========
  // 根据当前档位，到达间隔时间就更新一次亮度
  if (currentTime - lastFadeTime >= fadeInterval[speedLevel - 1]) {
    lastFadeTime = currentTime;

    brightness += fadeStep;
    // 亮度到边界时反转方向
    if (brightness <= 0 || brightness >= 255) {
      fadeStep = -fadeStep;
    }

    analogWrite(ledPin, brightness); // 输出PWM
  }
}
