#from fastapi import FastAPI
#from pydantic import BaseModel
#import pyserial as ps
#
#class LedBrightness(BaseModel):
#    name: str
#    value: float
#
#app = FastAPI()
#
#@app.post('/')
#async def leds_brightness(led_value: LedValue):
#    if led_value < 0 or led_value > 255:
#        return None # cambiar por el valor que tiene actualmente el arduino
#    return {'message': 'Hello world!'}
#
#@app.post('/')
#async def led_13_switch():
#    return {'message': 'Hello world!'}
#
#@app.get('/')
#async def ldr_value():
#    return {'message': 'Hello world!'}

from fastapi import FastAPI, WebSocket
from fastapi.responses import HTMLResponse
import serial
import asyncio

app = FastAPI()

html = """
<!DOCTYPE html>
<html>
<head>
    <title>Arduino Dashboard</title>
</head>
<body>
    <h1>Arduino Dashboard</h1>
    
    <div class="container">
      <label for="led_quant_9">Led 9:</label>
      <form action="" onsubmit="sendMessage(event, 'led_quant_9', 'led_9', 'AW', '9')">
        <input type="number" id="led_quant_9" autocomplete="off"/>
        <button>Send</button>
      </form>
      <ul id='led_9'></ul>
    </div>
    
    <div class="container">
      <label for="led_quant_10">Led 10:</label>
      <form action="" onsubmit="sendMessage(event, 'led_quant_10', 'led_10', 'AW', '10')">
        <input type="number" id="led_quant_10" autocomplete="off"/>
        <button>Send</button>
      </form>
      <ul id='led_10'></ul>
    </div>
    
    <div class="container">
      <label for="led_quant_11">Led 11:</label>
      <form action="" onsubmit="sendMessage(event, 'led_quant_11', 'led_11', 'AW', '11')">
        <input type="number" id="led_quant_11" autocomplete="off"/>
        <button>Send</button>
      </form>
      <ul id='led_11'></ul>
    </div>

    <div class="container">
      <label for="toggleSwitch">Led 7:</label>
      <div class="switch-container">
        <input type="checkbox" id="toggleSwitch">
        <span class="slider"></span>
      </div>
    </div>
    
    <div class="container">
      <label for="panel">Lectura sensor:</label>
      <div id="panel" class="panel"></div>
    </div>

    <script>
        var ws = new WebSocket("ws://localhost:8000/ws");
        var toggleSwitch = document.getElementById("toggleSwitch");
        
        var ws_sensor = new WebSocket("ws://localhost:8000/monitor");

        function sendMessage(event, inputId, listId, action, pin) {
            var data = {
                action: action,
                pin: pin,
            }
            
            if (action === 'AW') {
                var input = document.getElementById(inputId);
                data.value = input.value
            }
            else {
                data.value = inputId
            }

            ws.send(JSON.stringify(data));

            event.preventDefault();

           // var messages = document.getElementById(listId);
           // var message = document.createElement('li');
           // var content = document.createTextNode(input.value);
           // message.appendChild(content);
           // messages.appendChild(message);
        }

        ws.onmessage = function(event) {
            //var messages = document.getElementById('led_9');
            //var message = document.createElement('li');
            //var content = document.createTextNode(event.data);
            //message.appendChild(content);
            //messages.appendChild(message);
            var message = event.data;
            panel.textContent = message;
        };

        ws_sensor.onmessage = function(event) {
            var message = event.data;
            panel.textContent = message;
        };

        toggleSwitch.addEventListener("change", function() {
            var state = this.checked ? "1" : "0"; //high:low
            sendMessage(null, state, null, 'SW', '7');
        });
        
    </script>
</body>
</html>
"""

arduino = serial.Serial(port='/dev/ttyACM0', baudrate=9600, timeout=.1)

async def send_read(data: dict) -> str:
    arduino.write(bytes(f"{data['action']}:{data['pin']},{data['value']}\n", "utf-8"))
    await asyncio.sleep(0.5)
    data = arduino.readline()
    return data

@app.get("/")
async def get():
    return HTMLResponse(html)

@app.websocket('/ws')
async def websocket_endpoint(websocket: WebSocket):
    await websocket.accept()
    while True:
        data = await websocket.receive_json()
        if data.get('action') is not None \
            and data.get('pin') is not None \
            and data.get('value') is not None:
                msg_ard = await send_read(data)
                print(msg_ard)
                await websocket.send_text(f"Message text was: {data}")

@app.websocket('/monitor')
async def ldr_value(websocket: WebSocket):
    while True:
        data = arduino.readline()
        separator = data.index(':')
        if data[:separator] == 'INFO':
            websocket.send_text(data[separator+1:])

#@app.post('/')
#async def leds_brightness(led_value: LedValue):
#    if led_value < 0 or led_value > 255:
#        return None # cambiar por el valor que tiene actualmente el arduino
#    return {'message': 'Hello world!'}
#
#@app.post('/')
#async def led_13_switch():
#    return {'message': 'Hello world!'}
#
