/*
 * 作业2：使用millis()函数控制LED以1Hz频率稳定闪烁
 * 1Hz = 每秒闪烁1次（亮500ms，灭500ms）
 * 非阻塞方式，不占用CPU等待时间
 */

const int LED_PIN = 2;

// millis时间管理变量
unsigned long previousMillis = 0;
const long INTERVAL = 500;  // 500ms间隔

bool ledState = LOW;

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  
  Serial.println("=== 作业2: millis实现1Hz闪烁 ===");
  Serial.println("LED将以1Hz频率稳定闪烁");
}

void loop() {
  unsigned long currentMillis = millis();
  
  // 检查是否到达间隔时间（非阻塞）
  if (currentMillis - previousMillis >= INTERVAL) {
    previousMillis = currentMillis;
    
    // 切换LED状态
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
    
    // 串口输出调试信息
    Serial.print("时间: ");
    Serial.print(currentMillis);
    Serial.print(" ms, LED状态: ");
    Serial.println(ledState ? "ON" : "OFF");
  }
  
  // 这里可以同时执行其他任务（演示非阻塞）
  // 例如：读取传感器、处理数据等
}