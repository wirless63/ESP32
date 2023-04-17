const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE HTML>
<html>
	<head>
	  <title>ESP32 ESP-NOW & WEB SERVER (CONTROLLER)</title>
	  <meta name="viewport" content="width=device-width, initial-scale=1">
		<style>
		  html {font-family: Arial; display: inline-block; text-align: center;}
		  p {font-size: 1.2rem;}
		  body {margin: 0;}
		  .topnav {overflow: hidden; background-color: #41132b; color: white; font-size: 1.5rem;}
		  .content {padding: 20px; }
		  .card {background-color: white; box-shadow: 0px 0px 10px 1px rgba(140,140,140,.5); border: 1px solid #3d85c6; border-radius: 7px;}
		  .card.header {background-color: #41132b; color: white; border-bottom-right-radius: 0px; border-bottom-left-radius: 0px; border-top-right-radius: 7px; border-top-left-radius: 7px;}
		  .cards {max-width: 700px; margin: 0 auto; display: grid; grid-gap: 2rem; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));}
		  .LEDColor {color: #183153;}
		  
		  /* Sq Toggle Switch */
		  .switch {
			position: relative;
			display: inline-block;
			width: 70px;
			height: 34px;
		  }
		  .switch input {display:none;}
		  .sliderTS {
			position: absolute;
			cursor: pointer;
			top: 0;
			left: 0;
			right: 0;
			bottom: 0;
			background-color: #D3D3D3;
			-webkit-transition: .4s;
			transition: .4s;
		  }
		  .sliderTS:before {
			position: absolute;
			content: "";
			height: 26px;
			width: 26px;
			left: 4px;
			bottom: 4px;
			background-color: #f7f7f7;
			-webkit-transition: .4s;
			transition: .4s;
		  }
		  input:checked + .sliderTS {
			background-color: #41132b;
		  }
		  input:focus + .sliderTS {
			box-shadow: 0 0 1px #2196F3;
		  }
		  input:checked + .sliderTS:before {
			-webkit-transform: translateX(26px);
			-ms-transform: translateX(26px);
			transform: translateX(36px);
		  }
		  .sliderTS:after {
			content:'LOW';
			color: white;
			display: block;
			position: absolute;
			transform: translate(-50%,-50%);
			top: 50%;
			left: 70%;
			font-size: 10px;
			font-family: Verdana, sans-serif;
		  }
		  input:checked + .sliderTS:after {  
			left: 25%;
			content:'HIGH';
		  }    
		</style>
	</head>

	<body>
		<div class="topnav">
		 <h3>ESP32 ESP-NOW & WEB SERVER CONTROLLER (Encrypted)</h3>
    </div>
		<br>

		<div class="content">
			<div class="cards">
         
				<div class="card">
				  <div class="card header">
				  <h3>ESP32 RCVR #1</h3>
				  </div>
					<h4 class="LEDColor">GPIO 2</h4> 
					<label class="switch">
					  <input type="checkbox" id="togLED1" onclick="send_LED_State_Cmd('ESP32Rcvr1','togLED1','2')">
					  <div class="sliderTS"></div>
					</label>
					<br><br>
				</div>
			 
				<div class="card">
					<div class="card header">
					<h3>ESP32 RCVR #2</h3>
					</div>
				    <h4 class="LEDColor">GPIO 2</h4> 
					<label class="switch">
					  <input type="checkbox" id="togLED2" onclick="send_LED_State_Cmd('ESP32Rcvr2','togLED2','2')">
					  <div class="sliderTS"></div>
					</label>
				    <br><br>
				</div>
			 
				<div class="card">
					<div class="card header">
					<h3>ESP32 RCVR #3</h3>
					</div>
					<h4 class="LEDColor">GPIO 2</h4> 
					<label class="switch">
					  <input type="checkbox" id="togLED3" onclick="send_LED_State_Cmd('ESP32Rcvr3','togLED3','2')">
					  <div class="sliderTS"></div>
					</label>
					<br><br>
				</div>
			</div>
		</div>
    
		<script>
		  //The function called by "Toggle Switch" to control the GPIO.
		  function send_LED_State_Cmd(board,id,gpio) {
			var tgLEDFlash = document.getElementById(id);
			var tgState;

			if (tgLEDFlash.checked == true) tgState = 1;
			if (tgLEDFlash.checked == false) tgState = 0;

			send_cmd(board,gpio,tgState);
		  }
		  
		  // XMLHttpRequest to submit data.
		  function send_cmd(board,gpio,value) {
			var xhr = new XMLHttpRequest();
			xhr.open("GET", "set_LED?board="+board+"&gpio_output="+gpio+"&val="+value, true);
			xhr.send();
		  }
		</script>
  </body>
</html>
)=====";