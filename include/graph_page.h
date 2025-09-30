#ifndef GRAPH_PAGE_H
#define GRAPH_PAGE_H

const char GRAPH_PAGE[] PROGMEM = R"=====( 
<!DOCTYPE HTML>
<html>
  <head>
    <meta charset="utf-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>График температур</title>
    <style>
      body {
        background-color: #cccccc;
        font-family: Arial, Helvetica, sans-serif;
        color: #000088;
        margin: 0;
        padding: 20px;
        text-align: center;
      }
      .container {
        max-width: 400px;
        margin: auto;
        padding: 20px;
        background: white;
        border-radius: 10px;
        box-shadow: 0 4px 6px rgba(0,0,0,0.1);
        text-align: left;
      }
      h1 {
        font-size: 20px;
        margin-bottom: 10px;
        text-align: center;
      }
      .centerAlign {
        text-align: center;
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
      /* контейнер для SVG — пусть растягивается */
      #svgContainer { width: 100%; }
    </style>
    <script>
      function updateGraph() {
        fetch('/graph_svg')
          .then(response => response.text())
          .then(data => {
            document.getElementById('svgContainer').innerHTML = data;
          })
          .catch(()=>{ /* молча игнорируем ошибки */ });
      }
      // Обновляем только график: интервал 1000 мс (можешь поставить 2000)
      setInterval(updateGraph, 1000);
      window.onload = function() {
        updateGraph();
      };
    </script>
  </head>
  <body>
    <div class="container">
      <h1>Динамика температуры центрального датчика</h1>
      <div id="svgContainer"></div>

      <div class="centerAlign" style="margin-top:12px;">
        <a href="/" class="buttonLink">Вернуться на главную</a>
      </div>
    </div>
  </body>
</html>
)=====";

#endif
