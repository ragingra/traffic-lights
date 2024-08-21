const char MAIN_page[] PROGMEM = R"=====( 
<html>
<head>
    <script src="https://unpkg.com/htmx.org@1.9.12"></script>
    <script src="https://unpkg.com/htmx.org@1.9.12/dist/ext/ws.js"></script>
    <title>Live Traffic Lights</title>

    <style>
      .trafficLightsWrapper {
        display: grid;
        grid-template-columns: repeat(4, 1fr);
        margin-top: 20px;
        gap: 20px;
        height: 500;

      }

      .tl-body, .tl-filter, tl-light {
        position: absolute;
      }

      .trafficLight {
        /* background: red; */
        position: relative;
        margin: auto;
        top: 0;
        bottom: 0;
        left: 0;
        right: 0;
        width: 130px;
        height: 180px;
        display: flex;
        flex-direction: column;
        justify-content: space-between;
        align-items: center;
      }

      .tl-body {
        background-color: black;
        width: 50px;
        height: 150px;
        display: flex;
        flex-direction: column;
        justify-content: space-between;
        align-items: center;
        padding: 5px 5px;
        border-radius: 10px;
        margin: 10px;
        z-index: 5;
      }

      .tl-body.right {
        right: 0px;
      }
      .tl-body.left{
        left: 0px;
      }

      .tl-pole.left {
        left: 30px;
      }

      .tl-pole.right {
        right: 30px;
      }
      

      .tl-filter {
        position: absolute;
        background-color: rgb(0, 0, 0);
        width: 70px;
        height: 45px;
        display: flex;
        flex-direction: column;
        justify-content: space-between;
        padding: 5px 5px;
        border-radius: 10px;
        margin: 10px;
        bottom: 0px;
        z-index: 0;
      }

      .tl-filter.right  {
        align-items: flex-end;
        left: 30px;
      }

      .tl-filter.left {
        align-items: flex-start;
        left: 0px;
      }
      
      .tl-light {
        width: 45px;
        height: 45px;
        border-radius: 100%;
        background-color: gray;
      }

      .tl-light.filter {
        position: absolute;
        bottom: 5px;
      }

      .tl-pole {
        position: absolute;
        width: 15px;
        height: 200px;
        background-color: black;
        top: 170px;
      }

      .tl-pedPanel {
        position: absolute;
        width: 50px;
        height: 70px;
        background-color: black;
        margin: 10px;
        border-radius: 10px;
        top: 80px;
        left: -28px;
      }

      .tl-pedPanel-screen {
        position: relative;
        width: 40px;
        height: 60px;
        background-color: white;
        margin: 5px;
        border-radius: 5px;
      }

      .on.red {
        background-color: red;
      }

      .on.orange {
        background-color: orange;
      }

      .on.green, .on.filter {
        background-color: green;
      }

      .tl-pedPanel-screen.wait {
        background-color: orange;
      }

      .tl-pedPanel-screen.walk {
        background-color: green;
      }

      .tl-pedPanel-screen.stop {
        background-color: red;
      }

      #light1 {
        grid-column: 1;
        grid-row: 1;
      }
      
      #light2 {
        grid-column: 4;
        grid-row: 2;
      }

      #light3 {
        grid-column: 2;
        grid-row: 4;
      }


    </style>
</head>
<body>
  <div hx-ext="ws" ws-connect="ws://WEBSOCKET_IP:81/">
    <div id="pin_data"></div>
  </div>

  <h1>Traffic Lights</h1>
  <div class="trafficLightsWrapper">
  <div class="trafficLight" id="light1">
    <div class="tl-body left">
      <div class="tl-light red on" id="light1_red"></div>
      <div class="tl-light orange on" id="light1_orange"></div>
      <div class="tl-light green on" id="light1_green"></div>
    </div>
    <div class="tl-filter right">
      <div class="tl-light filter" id="light1_filter"></div>
    </div>
    <div class="tl-pole left">
      <div class="tl-pedPanel">
        <div class="tl-pedPanel-screen" id="ped1"></div>
      </div>
    </div>
  </div>

  <div class="trafficLight" id="light2">
    <div class="tl-body">
      <div class="tl-light red on" id="light2_red"></div>
      <div class="tl-light orange on" id="light2_orange"></div>
      <div class="tl-light green on" id="light2_green"></div>
    </div>
    <div class="tl-pole">
      <div class="tl-pedPanel">
        <div class="tl-pedPanel-screen" id="ped2"></div>
      </div>
    </div>
  </div>


  <div class="trafficLight" id="light3">
    <div class="tl-body right">
      <div class="tl-light red on" id="light3_red"></div>
      <div class="tl-light orange on" id="light3_orange"></div>
      <div class="tl-light green on" id="light3_green"></div>
    </div>
    <div class="tl-filter left">
      <div class="tl-light filter" id="light3_filter"></div>
    </div>
    <div class="tl-pole right">
      <div class="tl-pedPanel">
        <div class="tl-pedPanel-screen" id="ped3"></div>
      </div>
    </div>
  </div>

  <script>
    function updateTrafficLights(event) {
        const data = JSON.parse(event.detail.message);
        const pinData = data.pin_data;

        const lights = [
            { id: 'light1_green', pinIndex: 1 },
            { id: 'light1_orange', pinIndex: 2 },
            { id: 'light1_red', pinIndex: 3 },
            { id: 'light1_filter', pinIndex: 4 },
            { id: 'light2_green', pinIndex: 5 },
            { id: 'light2_orange', pinIndex: 6 },
            { id: 'light2_red', pinIndex: 7 },
            { id: 'light3_green', pinIndex: 9 },
            { id: 'light3_orange', pinIndex: 10 },
            { id: 'light3_red', pinIndex: 11 },
            { id: 'light3_filter', pinIndex: 12 },
        ];

        lights.forEach(light => {
            const element = document.getElementById(light.id);
            if (pinData[light.pinIndex] === 1) {
                element.classList.add('on');
            } else {
                element.classList.remove('on');
            }
        });

            const green = pinData[13];
            const red = pinData[14];

            const ped1 = document.getElementById('ped1');
            const ped2 = document.getElementById('ped2');
            const ped3 = document.getElementById('ped3');

            if (green === 1 && red === 1) {
                ped1.classList.add('wait');
                ped2.classList.add('wait');
                ped3.classList.add('wait');
            } else if (green === 1) {
                ped1.classList.add('walk');
                ped2.classList.add('walk');
                ped3.classList.add('walk');
            } else if (red === 1) {
                ped1.classList.add('stop');
                ped2.classList.add('stop');
                ped3.classList.add('stop');
            } else {
                ped1.classList.remove('wait', 'walk', 'stop');
                ped2.classList.remove('wait', 'walk', 'stop');
                ped3.classList.remove('wait', 'walk', 'stop');
            }
    }

    document.body.addEventListener("htmx:wsAfterMessage", () => {
      updateTrafficLights(event);
    });
  </script>
</body>
</html>
)=====";
