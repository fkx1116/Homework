// 双LED引脚：对应开发板丝印 D18、D19
const int ledAPin = 18;
const int ledBPin = 19;

// 渐变速度参数
const int fadeInterval = 5;    // 亮度更新间隔，数值越小交替越快
const int maxBrightness = 255; // 最大亮度，无电阻应急可改小（如100）
int brightness = 0;
int fadeStep = 1;
unsigned long lastFadeTime = 0;

void setup() {
  pinMode(ledAPin, OUTPUT);
  pinMode(ledBPin, OUTPUT);
  analogWrite(ledAPin, 0);
  analogWrite(ledBPin, maxBrightness);
  Serial.begin(115200);
  Serial.println("双路反相呼吸灯启动");
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - lastFadeTime >= fadeInterval) {
    lastFadeTime = currentTime;

    brightness += fadeStep;
    // 到达亮度边界时反转方向
    if (brightness <= 0 || brightness >= maxBrightness) {
      fadeStep = -fadeStep;
    }

    // 反相核心逻辑：A亮则B暗，完全同步反向
    analogWrite(ledAPin, brightness);
    analogWrite(ledBPin, maxBrightness - brightness);
  }
}
