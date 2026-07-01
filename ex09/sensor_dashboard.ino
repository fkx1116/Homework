#include <WiFi.h>
#include <WebServer.h>

// ========== 仅修改双引号内的WiFi名称和密码 ==========
const char* ssid     = "hid12345";
const char* password = "55555555";

const int touchPin = 4;  // 触摸引脚 GPIO4 / D4
WebServer server(80);


// ========== 1. 首页：返回仪表盘网页 ==========
void handleRoot() {
  String html = R"HTML(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>触摸传感器实时仪表盘</title>
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body {
            font-family: sans-serif;
            background: #f0f2f5;
            display: flex;
            justify-content: center;
            align-items: center;
            min-height: 100vh;
        }
        .dashboard {
            background: white;
            padding: 60px 80px;
            border-radius: 16px;
            box-shadow: 0 8px 24px rgba(0,0,0,0.1);
            text-align: center;
            min-width: 350px;
        }
        h1 {
            color: #333;
            font-size: 22px;
            margin-bottom: 40px;
        }
        .value-display {
            font-size: 96px;
            font-weight: bold;
            color: #2c7be5;
            margin-bottom: 16px;
            font-family: "Courier New", monospace;
        }
        .label {
            color: #666;
            font-size: 16px;
        }
        .bar-container {
            width: 100%;
            height: 12px;
            background: #e8e8e8;
            border-radius: 6px;
            overflow: hidden;
            margin-top: 30px;
        }
        .bar-fill {
            height: 100%;
            background: #2c7be5;
            width: 0%;
            transition: width 0.2s ease;
        }
    </style>
</head>
<body>
    <div class="dashboard">
        <h1>触摸传感器实时监测</h1>
        <div class="value-display" id="touchValue">--</div>
        <div class="label">触摸采样值</div>
        <div class="bar-container">
            <div class="bar-fill" id="valueBar"></div>
        </div>
    </div>

    <script>
        const valueEl = document.getElementById('touchValue');
        const barEl = document.getElementById('valueBar');
        const baseValue = 100; // 无触摸时的基准值，用于进度条适配

        // 拉取最新传感器数值并更新页面
        function refreshValue() {
            fetch('/api/touch')
                .then(res => res.text())
                .then(val => {
                    const num = parseInt(val);
                    valueEl.textContent = num;
                    // 进度条逻辑：数值越小（触摸越深），进度条越满
                    const percent = Math.max(0, Math.min(100, (1 - num / baseValue) * 100));
                    barEl.style.width = percent + '%';
                })
                .catch(err => console.error('数据获取失败:', err));
        }

        // 每200毫秒刷新一次，实现实时跳动效果
        setInterval(refreshValue, 200);
        refreshValue(); // 页面加载立即刷新一次
    </script>
</body>
</html>
  )HTML";
  server.send(200, "text/html", html);
}


// ========== 2. 数据接口：返回当前触摸传感器数值 ==========
void handleGetTouch() {
  int touchValue = touchRead(touchPin);
  server.send(200, "text/plain", String(touchValue));
}


void setup() {
  Serial.begin(115200);

  // 连接WiFi
  Serial.print("正在连接WiFi ");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi连接成功！");
  Serial.print("仪表盘地址：http://");
  Serial.println(WiFi.localIP());

  // 注册路由
  server.on("/", handleRoot);
  server.on("/api/touch", handleGetTouch);

  server.begin();
  Serial.println("实时仪表盘已启动");
}


void loop() {
  server.handleClient();
}
