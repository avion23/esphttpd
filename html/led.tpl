<html><head> <script src="http://ajax.googleapis.com/ajax/libs/jquery/1.7.0/jquery.min.js"></script>
    <title>Anastasia's LED</title>  
        <meta name=viewport content=width=device-width, initial-scale=1.0, user-scalable=yes>  
        <meta name=apple-mobile-web-app-capable content=yes>  
        <meta name=apple-mobile-web-app-status-bar-style content=default>  
        <link rel=apple-touch-icon href=https://o9y6s4d6dauiccov.myfritz.net/nas/filelink.lua?id=cd5f3e033c67538a>  
        <meta name=apple-mobile-web-app-status-bar-style content=black>
<link rel="stylesheet" type="text/css" href="style.css">
</head>
<body>
<div id="main">
<h1>The LED</h1>
<p>
If there's a LED connected to <b>%led0_pin%</b>, it's now <b>%stateLed0%</b>. You can change that using the buttons below.
</p>
<form method="post" action="led.cgi">
<input type="submit" name="led0" value="1">
<input type="submit" name="led0" value="0">
</form>
</div>
</body></html>
