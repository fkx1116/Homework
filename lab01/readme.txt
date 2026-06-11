实验一：Arduino IDE 安装与 ESP32 环境配置

一、实验环境
- 操作系统：Windows
- 开发板：ESP32
- IDE版本：Arduino IDE 2.3.10

二、实验内容
1. Arduino IDE 安装
2. ESP32 开发板支持包配置
3. CH340 驱动安装
4. 测试程序编写与上传
5. 串口监视器验证

三、测试代码
#define LED_PIN 2

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  Serial.println("Hello ESP32!");
  digitalWrite(LED_PIN, HIGH);
  delay(1000);
  digitalWrite(LED_PIN, LOW);
  delay(1000);
}

四、实验结果
- 板载 LED 每秒闪烁 1 次
- 串口监视器每秒输出 "Hello ESP32!"

五、思考题答案

问题1：串口监视器的波特率设置和代码中不一致会发生什么？为什么？

答案：会显示乱码。因为波特率是串口通信的速率，发送方和接收方必须以相同的速率才能正确解析数据。速率不一致时，接收方无法正确识别每个比特位，导致数据解码错误。

问题2：修改 delay(1000) 数值，LED闪烁频率如何变化？

答案：delay值越大，LED亮灭时间越长，闪烁越慢；delay值越小，LED亮灭时间越短，闪烁越快。
- delay(500)：亮0.5秒灭0.5秒，完整周期1秒，闪烁更快
- delay(2000)：亮2秒灭2秒，完整周期4秒，闪烁更慢

六、遇到的问题及解决
- 问题：上传时提示 Failed to connect to ESP32
- 解决：按住 BOOT 按钮，点击上传，出现 "Connecting..." 时松开