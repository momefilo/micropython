# momefilo for micropython
import ws2812
import time

# Strip initialisieren
pin = 16
anzahl = 144
bright = 160
liste = []
ws2812.init(pin, anzahl)
# Farben fuer den Strip definieren
r = bright
g = 0
b = 0
pos = 0
for led in range(0, anzahl, 1):
  ws2812.set(led, [r,g,b])# Farbe in das Array schreiben
  liste.append([r, g, b])
  # Farbe aendern
  if pos < anzahl / 3:
    g = g + int(bright / (anzahl / 3))
    r = r - int(bright / (anzahl / 3))
  elif pos < ((anzahl / 3) * 2):
    b = b + int(bright / (anzahl / 3))
    g = g - int(bright / (anzahl / 3))
  else:
    r = r + int(bright / (anzahl / 3))
    b = b - int(bright / (anzahl / 3))
  if r < 0:
    r = 0
  if g < 0:
    g = 0
  if b < 0:
    b = 0
  pos = pos + 1
  
# Farben auf den Strip schreiben
ws2812.write()
# Die Farben auf dem Strip verschieben
def turnon():
  count = 0
  speed = 150
  minspeed = 50
  maxspeed = 100
  speedcount = 0
  richtung = 1
  speed = minspeed
  while True:
    ws2812.move(richtung)
    time.sleep(1/speed)
    count = count + 1
    if count == anzahl:
      count = 0
      if richtung == 1:
        richtung = 1
      else:
        richtung = 1
      if speedcount == 0 and speed < maxspeed:
        speed = speed + 10
      elif speedcount == 0 and speed > maxspeed - 1:
        speedcount = 1
        speed = speed - 10
      elif speedcount == 1 and speed > minspeed:
        speed = speed - 10
      elif speedcount == 1 and speed < minspeed + 1:
        speedcount = 0
        speed = speed + 10

def turn():
  while True:
    global anzahl  
    tmp = liste[0]
    for x in range(0, anzahl - 1):
      liste[x] = liste[x + 1]
      ws2812.set(x, liste[x])
    liste[anzahl - 1] = tmp
    ws2812.set(anzahl - 1, liste[anzahl - 1], 1)
#    ws2812.write()
    
