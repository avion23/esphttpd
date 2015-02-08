<html>
<head><title>Esp8266 web server</title>
<link rel="stylesheet" type="text/css" href="style.css">
</head>
<body>
<div id="main">
<h1>It Works</h1>
<p>
If you see this, it means the tiny li'l website in your ESP8266 does actually work. Fyi, this page has
been loaded <b>%counter%</b> times.
<ul>
<li>If you haven't connected this device to your WLAN network now, you can <a href="/wifi">do so.</a></li>
<li>You can also control the <a href="led.tpl">LED</a>.</li>
<li>here is a link to the dht22 page <a href="dht22.tpl">DHT22</a>.</li>
<li>You can download the raw <a href="flash.bin">contents</a> of the SPI flash rom</li>
<li>You can read out the internal <a href="adc.tpl">ADC Sensor</a>.</li>
<li>You can also restart the device </li>
<form method="post" action="restart.cgi">
<input type="submit" name="restart_button" value="restart now">
</form>
</ul>
</p>


</div>
</body></html>
