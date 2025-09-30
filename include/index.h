#ifndef INDEX_H
#define INDEX_H

const char INDEX_PAGE[] PROGMEM = R"=====( 
<!DOCTYPE HTML>
<html lang="ru">
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>Управление АЧТ</title>
    <style>
      body {
        background-color: #cccccc;
        font-family: Arial, Helvetica, sans-serif;
        color: #000088;
        margin: 0;
        padding: 20px;
      }
      .container {
        position: relative;
        max-width: 400px;
        margin: auto;
        padding: 20px;
        padding-bottom: 50px; /* 🔧 добавляем место под иконку */
        background: white;
        border-radius: 10px;
        box-shadow: 0px 4px 6px rgba(0,0,0,0.1);
      }
      h1 {
        margin: 0;
        font-size: 22px;
        text-align: center;
        margin-bottom: 20px;
      }
      label {
        display: block;
        margin-bottom: 5px;
        font-weight: bold;
      }
      input[type='number'], input[type='text'], input[type='password'] {
        width: 100%;
        padding: 10px;
        font-size: 16px;
        border: 1px solid #ccc;
        border-radius: 5px;
        box-sizing: border-box;
        margin-bottom: 15px;
      }
      /* Стиль для основной кнопки "Установить" */
      input[type='submit'] {
        width: 100%;
        padding: 12px;
        font-size: 16px;
        color: white;
        background-color: #007BFF;
        border: none;
        border-radius: 5px;
        cursor: pointer;
        box-sizing: border-box;
        font-family: inherit;
        font-weight: 400;
        line-height: 1.2;
      }
      input[type='submit']:hover {
        background-color: #0056b3;
      }

      /* Сделаем ссылки-кнопки (Журнал UART, График) визуально такими же, как кнопка Установить */
      .buttonLink {
        display: block; /* чтобы точно совпадала по блочной модели */
        width: 100%;
        padding: 12px;
        border: none;
        border-radius: 5px;
        background-color: #007BFF;
        color: white;
        text-decoration: none;
        font-size: 16px;
        cursor: pointer;
        box-sizing: border-box;
        text-align: center;
        font-family: inherit; /* тот же шрифт */
        font-weight: 400;     /* тот же вес */
        line-height: 1.2;     /* тот же межстрочный интервал */
      }
      .buttonLink:hover {
        background-color: #0056b3;
      }

      /* Контейнер для кнопок */
      .buttonContainer {
        position: relative;
        display: flex;
        justify-content: center;
        margin: 15px 0;
        padding-top: 0;
      }

      /* Внутренний обёртчик: колонки с кнопками, растягиваем до ширины контейнера */
      .buttonGroup {
        width: 100%;
        display: flex;
        flex-direction: column;
        gap: 8px;
        align-items: stretch;
        max-width: 100%;
      }

      .buttonIcon {
        display: inline-block;
        width: 40px;
        height: 40px;
        border-radius: 50%;
        background-color: #007BFF;
        color: white;
        text-align: center;
        line-height: 40px;
        font-size: 20px;
        cursor: pointer;
        transition: background-color 0.3s;
        box-sizing: border-box;
        z-index: 10;
      }
      .buttonIcon:hover {
        background-color: #0056b3;
      }

      /* Иконка настроек — располагаем в правом нижнем углу всего .container */
      .container #settingsIcon {
        position: absolute;
        right: 12px;
        bottom: 12px;
        top: auto;
      }

      .info-block { margin-top:12px; }
      .info-block p { margin: 6px 0; }
      .info-block .label { font-weight:700; display:block; margin-bottom:4px; }
      .info-block .value { font-size:18px; color:#003366; }

      /* ---------- стили для компактной модалки и кнопок в ней ---------- */
      .modal-overlay {
        display: none;
        position: fixed;
        left: 0; top: 0;
        width: 100%; height: 100%;
        background: rgba(0,0,0,0.4);
        z-index: 50;
        align-items: center;
        justify-content: center;
        padding: 16px;
        box-sizing: border-box;
      }

      .modal {
        background: #fff;
        border-radius: 10px;
        box-shadow: 0 6px 18px rgba(0,0,0,0.18);
        max-width: 320px;
        width: 100%;
        padding: 14px;
        box-sizing: border-box;
        color: #003366;
        font-family: inherit;
      }

      /* Заголовок модалки */
      .modal h3 {
        margin: 0 0 8px 0;
        font-size: 16px;
        color: #003366;
      }

      /* Поля внутри модалки — делаем по стилю основного input */
      .modal input[type="text"],
      .modal input[type="password"] {
        width: 100%;
        padding: 8px 10px;
        font-size: 15px;
        border: 1px solid #ccc;
        border-radius: 6px;
        box-sizing: border-box;
        margin-bottom: 10px;
      }

      /* Actions — две кнопки рядом, выглядят как основные, но меньшие */
      .modal-actions {
        display: flex;
        gap: 8px;
        margin-top: 6px;
      }

      /* Основная кнопка (Выглядит как ваши .buttonLink / input[type=submit]) */
      .modalButton {
        flex: 1;
        padding: 8px 10px;
        font-size: 14px;
        color: white;
        background-color: #007BFF;
        border: none;
        border-radius: 6px;
        cursor: pointer;
        text-align: center;
        box-sizing: border-box;
        font-family: inherit;
        text-decoration: none;
      }

      /* Вариант "вторичная" — светлый фон, тёмный текст */
      .modalButton.secondary {
        background-color: #e9eef7;
        color: #003366;
      }

      /* hover */
      .modalButton:hover { background-color: #0056b3; }
      .modalButton.secondary:hover { background-color: #d6e0f3; }

      /* Небольшая иконка закрытия справа вверху (опционально) */
      .modal-close {
        position: absolute;
        right: 18px;
        top: 18px;
        font-size: 18px;
        cursor: pointer;
        color: #666;
      }

      /* Уменьшим круговую иконку настроек (если захотим более "плоский" вид) */
      .container #settingsIcon {
        width: 36px;
        height: 36px;
        line-height: 36px;
        font-size: 18px;
        border-radius: 6px; /* <-- не круглая, а прямоугольная с закруглениями */
      }

      /* небольшой локальный стиль для подсказок (toast) */
      .toast {
        position: fixed;
        left: 50%;
        bottom: 28px;
        transform: translateX(-50%) translateY(12px);
        background: rgba(0,0,0,0.85);
        color: #fff;
        padding: 10px 14px;
        border-radius: 8px;
        font-size: 14px;
        box-shadow: 0 6px 18px rgba(0,0,0,0.25);
        opacity: 0;
        pointer-events: none;
        transition: opacity 260ms ease, transform 260ms ease;
        z-index: 9999;
        max-width: 92%;
        text-align: center;
      }
      .toast.show {
        opacity: 1;
        transform: translateX(-50%) translateY(0);
        pointer-events: auto;
      }

      /* небольшая корректировка для меток — теперь они не кликабельны */
      .field-label {
        display: block;
        font-weight: 600;
        font-size: 13px;
        margin-bottom: 6px;
        color: #003366;
        user-select: none;
        pointer-events: none; /* <-- делает текст неинтерактивным */
      }
    </style>

    <script>
      let isFetching = false;

      function updateMac() {
        fetch('/get_mac')
          .then(r => r.json())
          .then(j => {
            if (j && j.mac) {
              const el = document.getElementById('macAddr');
              if (el) el.textContent = '(' + j.mac + ')';
            }
          })
          .catch(()=> {
          });
      }

      document.addEventListener('DOMContentLoaded', function() {
        updateAll();
        updateMac();            // <-- запрашиваем MAC при загрузке
        setInterval(updateAll, 1000);
      });


      function updateAll() {
        updateSetpoint();
        updateCentralTemp();
      }

      function updateSetpoint() {
        if (isFetching) return;
        isFetching = true;
        fetch('/get_setpoint')
          .then(r => r.json())
          .then(j => {
            if (j && typeof j.setpoint !== 'undefined') {
              let inp = document.getElementById('tempInput');
              if (document.activeElement !== inp) {
                inp.value = parseFloat(j.setpoint).toFixed(1);
              }
            }
            if (j && typeof j.lastCmd !== 'undefined') {
              document.getElementById('lastCmd').innerText = j.lastCmd;
            }
          })
          .catch(()=>{})
          .finally(()=> { isFetching = false; });
      }

      async function updateCentralTemp() {
        try {
          let r = await fetch('/get_temp');
          if (r.ok) {
            let j = await r.json();
            if (j && typeof j.temp !== 'undefined') {
              document.getElementById('currentTemp').innerText = j.temp;
              return;
            }
          }
        } catch(e){}
      }

      function setTemperature(event) {
        event.preventDefault();
        let tempValue = document.getElementById('tempInput').value;
        if (!tempValue) return;
        const formatted = parseFloat(tempValue).toFixed(1);
        const btn = document.getElementById('setBtn');
        fetch('/set_temp', {
          method: 'POST',
          headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
          body: 'temp=' + encodeURIComponent(tempValue)
        })
        .then(response => response.text())
        .then(() => {
          document.getElementById('lastCmd').innerText = 'ESP: Sent T=' + formatted;
          // remove focus from button so color returns
          if (btn && typeof btn.blur === 'function') btn.blur();
          // update after stm32 reply
          setTimeout(updateAll, 600);
        });
      }

      document.addEventListener('DOMContentLoaded', function() {
        updateAll();
        setInterval(updateAll, 1000);
      });
    </script>
  </head>
  <body>
    <div class="container">
      <h1>Управление АЧТ <span id="macAddr" style="font-size:14px; color:#003366;">(...) </span></h1>
    
      <form onsubmit="setTemperature(event)">
        <label for="tempInput">Установить температуру:</label>
        <input type="number" id="tempInput" name="temp" value="%SETPOINT%" step="0.1" min="-20" max="100" required>
        <input type="submit" id="setBtn" value="Установить">
      </form>

      <div class="info-block">
        <p><span class="label">Текущая температура:</span>
           <span class="value" id="currentTemp">%CURRENTTEMP%</span> °C
        </p>
        <p><span class="label">Последняя команда:</span>
           <span id="lastCmd">%LASTCMD%</span>
        </p>
      </div>

      <div class="buttonContainer">
        <div class="buttonGroup">
          <a href="/log" class="buttonLink">Журнал UART</a>
          <a href="/graph" class="buttonLink">График</a>
        </div>
      </div>

      <!-- Иконка настроек (оставляем как есть, теперь открывает компактную модалку) -->
      <div id="settingsIcon" class="buttonIcon" title="Настройки" onclick="openSettings()">🔧</div>

      <!-- Компактная модалка -->
      <div id="settingsModal" class="modal-overlay" onclick="onOverlayClick(event)">
        <div class="modal" role="dialog" aria-modal="true" aria-labelledby="modalTitle" onclick="event.stopPropagation()">
          <div style="position:relative;">
            <span id="modalClose" class="modal-close" onclick="closeSettings()">✕</span>
            <h3 id="modalTitle">Настройки точки доступа</h3>
          </div>

          <form id="settingsForm" onsubmit="saveSettings(event)" autocomplete="off">
            <!-- Обратите внимание: заменили <label> на .field-label — они теперь НЕ кликабельны -->
            <span class="field-label" for="cfg_ssid">SSID</span>
            <input type="text" id="cfg_ssid" name="ssid" required>

            <span class="field-label" for="cfg_pass">PASSWORD</span>
            <input type="password" id="cfg_pass" name="pass">

            <div class="modal-actions">
              <button type="submit" class="modalButton" id="saveCfgBtn">Сохранить</button>
              <button type="button" class="modalButton secondary" id="cancelCfgBtn" onclick="closeSettings()">Отмена</button>
            </div>
          </form>
        </div>
      </div>

      <!-- Toast элемент (всегда в DOM, чтобы showToast мог найти его) -->
      <div id="toast" class="toast" aria-live="polite" aria-atomic="true"></div>

    </div>

    <script>
    /* Открытие/закрытие модалки */
    function openSettings() {
      const overlay = document.getElementById('settingsModal');
      if (!overlay) return;
      overlay.style.display = 'flex';

      fetch('/get_config')
        .then(r => r.json())
        .then(j => {
          document.getElementById('cfg_ssid').value = j.ssid || '';
          document.getElementById('cfg_pass').value = j.pass || '';
        })
        .catch(()=>{});

      // фокус только на input (будет работать при открытии)
      setTimeout(()=>{ 
        const el = document.getElementById('cfg_ssid');
        if(el) el.focus();
      }, 120);
    }
    function closeSettings() {
      const overlay = document.getElementById('settingsModal');
      if (!overlay) return;
      overlay.style.display = 'none';
    }
    function onOverlayClick(e) {
      if (e.target && e.target.id === 'settingsModal') closeSettings();
    }

    /* Toast: показать сообщение и автоскрытие
       Элемент #toast присутствует в DOM — используем его.
       Если вдруг его нет — создаём (на всякий случай). */
    function showToast(text, ms = 2200) {
      let t = document.getElementById('toast');
      if (!t) {
        t = document.createElement('div');
        t.id = 'toast';
        t.className = 'toast';
        t.setAttribute('aria-live', 'polite');
        t.setAttribute('aria-atomic', 'true');
        document.body.appendChild(t);
      }
      t.textContent = text;
      t.classList.add('show');
      clearTimeout(t._hideTimer);
      t._hideTimer = setTimeout(()=> {
        t.classList.remove('show');
      }, ms);
    }

    /* Сохранить настройки */
    function saveSettings(e) {
      e.preventDefault();
      const ssid = document.getElementById('cfg_ssid').value.trim();
      const pass = document.getElementById('cfg_pass').value;

      if (!ssid) {
        showToast('SSID не может быть пустым', 1800);
        return;
      }

      const saveBtn = document.getElementById('saveCfgBtn');
      const cancelBtn = document.getElementById('cancelCfgBtn');
      if (saveBtn) saveBtn.disabled = true;
      if (cancelBtn) cancelBtn.disabled = true;

      fetch('/set_config', {
        method: 'POST',
        headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
        body: 'ssid=' + encodeURIComponent(ssid) + '&pass=' + encodeURIComponent(pass)
      }).then(r => {
        if (r.ok) {
          // показываем уведомление — пользователь должен вручную переключиться на новую сеть,
          // потому что при автоматическом reload браузер может потерять соединение раньше, чем увидит сообщение.
          showToast('Сохранено — переключитесь на сеть «' + ssid + '» и откройте панель управления', 4200);

          // закрыть модалку (чтобы пользователь видел toast)
          closeSettings();

        } else {
          showToast('Ошибка сохранения (сервер вернул ошибку)', 2000);
        }
      }).catch(()=> {
        showToast('Ошибка сети — попробуйте ещё раз', 2000);
      }).finally(()=> {
        if (saveBtn) saveBtn.disabled = false;
        if (cancelBtn) cancelBtn.disabled = false;
      });
    }
    </script>
  </body>
</html>
)=====";

#endif
