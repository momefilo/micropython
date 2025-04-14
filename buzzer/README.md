## Ein micropython- Buzzer modul mit dem drei Oktaven auf einem Passiv-Buzzer wiedergegeben werden
Das Modul wird mit 
```
import buzzer
```
importiert und vor Benutzung folgend initialisiert
```
buzzer.init(Buzzer-Pin)
```
Das modul bieten nur eine Funktion
```
buzzer.play(note, dauer, oktave)
```
note: 0=c bis 11=b\
dauer: teilt die 1,5 Sekunden Ganznotendauer in 1,5/dauer\
okteve: 0=Grundoktave, 1=eine-, 2= zwei Oktaven hoeher\

Siehe [buzzer_demo.py](buzzer_demo.py)
