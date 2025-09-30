#ifndef LOG_PAGE_H
#define LOG_PAGE_H

const char LOG_PAGE[] PROGMEM = R"=====( 
<!DOCTYPE html>
<html lang="ru">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Журнал UART — АЧТ</title>
  <style>
    body {
      background-color: #cccccc;
      font-family: Arial, Helvetica, sans-serif;
      color: #000088;
      margin: 0;
      padding: 20px;
      text-align: center;
    }
    .wrap {
      max-width: 760px;
      margin: 12px auto;
      padding: 20px;
      background: white;
      border-radius: 10px;
      box-shadow: 0 4px 6px rgba(0,0,0,0.1);
      text-align: left;
    }
    h1 {
      font-size: 20px;
      margin: 0 0 10px 0;
      text-align: center;
    }
    pre {
      background: #001f3f;
      color: #dff7ff;
      padding: 10px;
      border-radius: 6px;
      height: 420px;
      overflow: auto;
      white-space: pre-wrap;
      word-break: break-word;
    }
    .meta {
      font-size: 13px;
      color: #444;
      margin-bottom: 8px;
      text-align: center;
    }
    .controls {
      display: flex;
      gap: 8px;
      align-items: center;
      margin-top: 8px;
    }
    .button {
      padding: 8px 12px;
      border-radius: 6px;
      border: none;
      background: #007bff;
      color: #fff;
      cursor: pointer;
    }
    .button.secondary {
      background: #e6eefc;
      color: #0056b3;
    }

    .centerAlign {
      text-align: center;
      margin-top: 12px;
    }

    .buttonLink {
      display: inline-block;
      padding: 8px 16px;
      border: none;
      border-radius: 5px;
      background-color: #007BFF;
      color: white;
      text-decoration: none;
      font-size: 14px;
      cursor: pointer;
    }
    .buttonLink:hover {
      background-color: #0056b3;
    }

    a.back {
      margin-left: auto;
      color: #0056b3;
      text-decoration: none;
      font-weight: 600;
    }
  </style>
</head>
<body>
  <div class="wrap">
    <h1>Журнал UART</h1>
    <pre id="log">(ожидание данных...)</pre>
    <div class="controls">
      <button class="button" onclick="clearLog()" id="clearBtn">Очистить</button>
      <button class="button secondary" onclick="downloadLog()" id="downloadBtn">Скачать</button>
    </div>

    <div class="centerAlign">
      <a href="/" class="buttonLink">Вернуться на главную</a>
    </div>

    <div style="font-size:12px;color:#667;margin-top:8px;">Последнее обновление: <span id="ts">-</span></div>
  </div>

<script>
let autoTimer = null;

function update() {
  fetch('/uart')
    .then(r => r.text())
    .then(txt => {
      const pre = document.getElementById('log');
      pre.textContent = txt || '(пусто)';
      pre.scrollTop = pre.scrollHeight;
      document.getElementById('ts').textContent = new Date().toLocaleTimeString();
      // remove focus from buttons if any
      const cb = document.getElementById('clearBtn'); if (cb) cb.blur();
      const db = document.getElementById('downloadBtn'); if (db) db.blur();
    })
    .catch(err => {
      document.getElementById('log').textContent = '(ошибка fetching) ' + err;
    });
}

function clearLog() {
  fetch('/uart_clear', { method: 'POST' })
    .then(r => { if (r.ok) update(); })
    .finally(() => { const cb = document.getElementById('clearBtn'); if (cb) cb.blur(); });
}

function downloadLog() {
  fetch('/uart')
    .then(r => r.text())
    .then(txt => {
      const blob = new Blob([txt], { type: 'text/plain' });
      const url = URL.createObjectURL(blob);
      const a = document.createElement('a');
      a.href = url; a.download = 'uart_log.txt';
      document.body.appendChild(a); a.click(); a.remove();
      URL.revokeObjectURL(url);
    })
    .finally(() => { const db = document.getElementById('downloadBtn'); if (db) db.blur(); });
}

window.onload = function() {
  update();
  autoTimer = setInterval(update, 1000);
};
</script>
</body>
</html>
)=====";

#endif
