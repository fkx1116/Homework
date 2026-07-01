#include <WiFi.h>
#include <WebServer.h>

// ========== 请修改为你的路由器WiFi信息 ==========
const char* ssid     = "hid12345";
const char* password = "55555555";

// 引脚定义
const int ledPin = 2;      // 报警LED引脚
const int touchPin = 4;    // 触摸检测引脚

// 触摸检测参数
const int touchThreshold = 30;
const unsigned long debounceDelay = 50;
unsigned long lastDebounceTime = 0;
bool lastTouchReading = false;
bool stableTouchState = false;

// 系统状态变量
bool systemArmed = false;     // 系统是否布防：false=撤防，true=布防
bool alarmTriggered = false;  // 是否触发报警

// LED高频闪烁参数（非阻塞）
const int blinkInterval = 100;  // 报警闪烁间隔，单位ms，数值越小闪得越快
unsigned long lastBlinkTime = 0;
bool ledState = LOW;

WebServer server(80);


// ========== 1. 根路径：返回安防控制网页 ==========
void handleRoot() {
  String html = R"HTML(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>安防报警器控制台</title>
    <style>
        body { font-family: sans-serif; max-width: 420px; margin: 60px auto; text-align: center; }
        h1 { color: #333; margin-bottom: 40px; }
        .btn {
            display: block;
            width: 100%;
            padding: 18px 0;
            margin: 20px 0;
            font-size: 20px;
            font-weight: bold;
            border: none;
            border-radius: 10px;
            cursor: pointer;
            color: white;
        }
        .arm-btn { background-color: #e74c3c; }
        .disarm-btn { background-color: #27ae60; }
        .status {
            margin-top: 30px;
            font-size: 18px;
            padding: 15px;
            border-radius: 8px;
            background: #f0f0f0;
        }
    </style>
</head>
<body>
    <h1>安防报警器控制台</h1>
    <button class="btn arm-btn" onclick="setMode('arm')">布 防</button>
    <button class="btn disarm-btn" onclick="setMode('disarm')">撤 防</button>
    <div class="status" id="statusText">当前状态：已撤防</div>

    <script>
        const statusText = document.getElementById('statusText');

        function setMode(mode) {
            fetch(`/${mode}`)
                .then(res => res.text())
                .then(msg => {
                    statusText.textContent = msg;
                })
                .catch(err => {
                    statusText.textContent = '请求失败，请检查连接';
                });
        }
    </script>
</body>
</html>
  )HTML";

  server.send(200, "text/html", html);
}


// ========== 2. 布防接口 ==========
void handleArm() {
  systemArmed = true;
  alarmTriggered = false;
  digitalWrite(ledPin, LOW);  // 布防后LED熄灭，等待触发
  server.send(200, "text/plain", "当前状态：已布防");
}


// ========== 3. 撤防接口 ==========
void handleDisarm() {
  systemArmed = false;
  alarmTriggered = false;
  digitalWrite(ledPin, LOW);  // 撤防后关闭报警
  // 重置触摸状态，防止撤防时残留的触摸信号误触发
  lastTouchReading = false;
  stableTouchState = false;
  server.send(200, "text/plain", "当前状态：已撤防");
}


void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  // 连接WiFi
  Serial.print("正在连接WiFi ");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi连接成功！");
  Serial.print("控制台地址：http://");
  Serial.println(WiFi.localIP());

  // 注册网页路由
  server.on("/", handleRoot);
  server.on("/arm", handleArm);
  server.on("/disarm", handleDisarm);

  server.begin();
  Serial.println("安防报警器已启动");
}


void loop() {
  unsigned long currentTime = millis();
  server.handleClient();  // 处理网页请求

  // ========== 1. 触摸检测 + 防抖 + 上升沿触发 ==========
  int touchValue = touchRead(touchPin);
  bool currentReading = (touchValue < touchThreshold);

  // 瞬时状态变化时重置防抖计时器
  if (currentReading != lastTouchReading) {
    lastDebounceTime = currentTime;
  }

  // 状态稳定后确认有效触摸
  if (currentTime - lastDebounceTime > debounceDelay) {
    if (currentReading != stableTouchState) {
      stableTouchState = currentReading;

      // 仅在「已布防 + 未触发报警 + 触摸按下瞬间」三个条件同时满足时，触发报警
      if (stableTouchState == true && systemArmed == true && alarmTriggered == false) {
        alarmTriggered = true;
        Serial.println("警告：检测到入侵，报警已触发！");
      }
    }
  }
  lastTouchReading = currentReading;


  // ========== 2. 报警LED高频闪烁逻辑（非阻塞） ==========
  if (alarmTriggered == true) {
    // 到达间隔时间就翻转LED电平，实现高频狂闪
    if (currentTime - lastBlinkTime >= blinkInterval) {
      lastBlinkTime = currentTime;
      ledState = !ledState;
      digitalWrite(ledPin, ledState);
    }
  } else {
    // 未报警状态下LED保持熄灭
    digitalWrite(ledPin, LOW);
    ledState = LOW;
  }
}
