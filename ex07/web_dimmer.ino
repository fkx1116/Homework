#include <WiFi.h>
#include <WebServer.h>

// ========== 仅修改双引号内的WiFi名称和密码 ==========
const char* ssid     = "hid12345";
const char* password = "55555555";

// LED引脚定义
const int ledPin = 2;  // 板载LED对应GPIO2 / D2

WebServer server(80);  // Web服务器，监听80端口


// ========== 1. 根路径：返回调光网页 ==========
void handleRoot() {
  String html = R"HTML(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32无极调光器</title>
    <style>
        body { font-family: sans-serif; max-width: 420px; margin: 60px auto; text-align: center; }
        h1 { color: #333; }
        .slider { width: 100%; height: 24px; margin: 30px 0; cursor: pointer; }
        .value { font-size: 28px; font-weight: bold; color: #2c7be5; }
    </style>
</head>
<body>
    <h1>LED 无极调光</h1>
    <input type="range" min="0" max="255" value="0" class="slider" id="brightnessSlider">
    <div class="value">当前亮度：<span id="valueText">0</span></div>

    <script>
        const slider = document.getElementById('brightnessSlider');
        const valueText = document.getElementById('valueText');

        // 监听滑动条拖动事件，实时发送亮度请求
        slider.addEventListener('input', function() {
            const brightness = this.value;
            valueText.textContent = brightness;
            
            // 异步发送请求，页面无刷新，保证拖动流畅
            fetch(`/set?brightness=${brightness}`)
                .catch(err => console.error('调光请求失败:', err));
        });
    </script>
</body>
</html>
  )HTML";

  server.send(200, "text/html", html);
}


// ========== 2. 调光接口：解析网页传来的亮度值并输出PWM ==========
void handleSetBrightness() {
  if (server.hasArg("brightness")) {
    int brightness = server.arg("brightness").toInt();
    brightness = constrain(brightness, 0, 255);
    
    analogWrite(ledPin, brightness);
    server.send(200, "text/plain", "亮度已设置: " + String(brightness));
  } else {
    server.send(400, "text/plain", "参数错误：缺少brightness参数");
  }
}


void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  analogWrite(ledPin, 0);

  Serial.print("正在连接WiFi ");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi连接成功！");
  Serial.print("请在浏览器打开：http://");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/set", handleSetBrightness);
  server.begin();
  Serial.println("Web服务器已启动");
}


void loop() {
  server.handleClient();
}
