<html><head><title>DHT</title>
<link rel="stylesheet" type="text/css" href="style.css">
</head>
<body>
<div id="main">
<h1>DHT 11 sensor</h1>
<p>
	Sensor Check: A sensor is <b>%sensor_present%</b>  at the configured PIN <b>%dht_pin%</b>. 
</p>
<p>
temperature = <b>%temperature%</b> *C
</p>
<p>
humidity = <b>%humidity%</b>
</p>
<br/>
<p>
	If there's a DS18B20 connected on the configured PIN <b>%ds18b20_pin%</b>, it is reading
</p>
<p>
DS18B20 = <b>%ds18b20_temperature%</b>
</p>
<button onclick="location.href = '/';" class="float-left submit-button" >Home</button>
</div>
</body></html>
