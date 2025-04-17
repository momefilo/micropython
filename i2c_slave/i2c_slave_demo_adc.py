# momefilo for micropython
# Das ist ein Demoprogramm für das i2c_slave-Modul  (getestet auf pico_w)
# Das die vier ADC's auslest und die onboard-LED schaltet
# Verbinde die pico-i2c-pins mit einem RPi-i2c-Bus und führe volgende Befehle aus
# Zuerst wird das Highbyte und dann das Lowbyte der ADC(0) bis ADC(4) empfangen
# Mit 'i2cget -y 1 0x47 0x00 i 2' ADC(0) bis
# Mit 'i2cget -y 1 0x47 0x04 i 2' ADC(4) nur pico_w
# Mit 'i2cset -y 1 0x47 0x05 0x01' wird die Onboard-led ein -und
# mit 'i2cset -y 1 0x47 0x05 0x00' ausgeschaltet

from machine import Pin, ADC
import i2c_slave

led = Pin("LED", Pin.OUT)# an pico ohne w anzupassen
data = 0# Aktueller ADC-Wert
receive = -1# Der led-Schaltung benoetigte Variable zur Trennung von Adress- und Datenbyte

def set_led(val):
  if val == 0:
    led.off()
  else:
    led.on()

def get_adc(val):
  global data
  data = ADC(val).read_u16()

# Diese Funktion wird ausgeführt wenn der Master eine
# read- oder write-Nachricht an unsere Adresse sendet
# i2c ist der i2c-Bus
# handler sind "I2C_SLAVE_RECEIVE", "I2C_SLAVE_REQUEST" und "I2C_SLAVE_FINISH"
def callback(i2c, handler):
  if handler == "I2C_SLAVE_RECEIVE":
    global receive
    rec = i2c_slave.readByte(i2c)# das erste Byte nach dem Adressbyte einlesen
    if (rec == 5 and receive == -1):# Das nächste Byte schaltet die led
      receive = rec
    elif receive == 5:# Das Byte welches die led schaltet
      set_led(rec)
      receive = -1
    else:# oder die ADC's werden gelesen
      get_adc(rec)
      
  if handler == "I2C_SLAVE_REQUEST":
    i2c_slave.writeBlock(i2c, [((data & 0xFF00) >> 8), (data & 0x00FF)], 2)
    
  if handler == "I2C_SLAVE_FINISH":
    pass

# i2c_slave.init(i2c-bus, sda, scl, freq, addr, callback)
i2c_slave.init(0, 4, 5, 400*1000, 0x47, callback)
