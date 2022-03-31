#!/usr/bin/env python2

html="""
<head>
  <title>Peri Web Server</title>
</head>
<body>
LEDS:<br/>
<form method="POST" action="led.py">
  <input name="val" cols="20"></input>
  <input type="submit" value="Entrer">
</form>
</body>
"""

print html
