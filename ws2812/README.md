## Das Modul steuert nur einen ws2812-LED Strip, aber schneller
das Modul bietet drei Funktionen und wird mit
```
import ws2812
```
importiert, und mit
```
ws2812.init(Pin, anzahl_leds)
```
initialisiert. die beiden Anderen sind:
```
ws2812.set(led, [rot, gruen, blau], optional_write)
```
led: 0 bis (anzahl_leds - 1)\
[rot, gruen, blau]: sind drei Integerwerte zwischen 0 und 255\
optional_write: mit einem optionalem dritten Parameter werden alle Farbwerte auf die leds geschrieben
```
ws2812.write)
```
schreibt alle Farbwerte auf die leds\
siehe [ws2812_demo.py](ws2812_demo.py)
