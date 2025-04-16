# momefilo for micropython
def info():
# Diese Klasse demonstriert die Benutzung des i2c_slave_Moduls auf einem rpi_pico_w
# das die vier Werte der AD-Wandler und den Zustand eines gpio's aus liest,
# sowie den Zustand eines weiteren gpio's schaltet. Die Klasse verwendet die Onboard-LED als
# Zustandsanzeige die leuchtet wenn der Slave bereit ist.
# Mit einem RPi als Master am i2c-Bus können durch aufruf
# folgender Befehle die Funktion dieser Slave-Modul Klasse getestet werden
#
# ->i2cdetect -y 1
#   Der Master sucht auf dem Standart-i2c-Bus 1 nach Slaves
#   und gibt deren Adressen hexadezimal zurück.
#   Wenn die Onboard-Led des pico leuchtet sollte dies einmalig aufgerufen sein
#   nicht nur um die korrekte Adresse des pico zu überprüfen, sondern auch weil
#   sonst das erste Datenbyte vom pico falsch als 0xFF gelesen wird
#
# ->i2cget -y 1 0x47 0x00 i 2
#   Der Master sendet das Adressbyte '0x47' gefolgt von einem Datenbyte '0x00'
#   auf den i2c-Bus 1 und übergibt der callback-Funktion dieser Klasse 'I2C_SLAVE_RECEIVE' als handler.
#   Aufgrund dessen wird dort das erste Datenbyte '0x00' eingelesen und danch sendet der Master ein
#   'I2C_SLAVE_FINISH' als handler an die callback-Funktion zurück
#   Danach wartet der Master mit einem 'I2C_SLAVE_REQUEST' als handler darauf zwei Bytes zu empfangen.
#   Die callback-Funktion sendet daraufhin zu erst die oberen MSB dann die unteren LSB als zwei
#   einzelne Bytes des 16-Bit umfassenden ADC(0)-Registers auf den i2c-Bus zurück.
#   Das geht auf dem pico_w bis zu fünf ADC(0) bis ADC(4)
#   ->i2cget -y 1 0x47 0x01 i 2
#   ->i2cget -y 1 0x47 0x02 i 2
#   ->i2cget -y 1 0x47 0x03 i 2
#   ->i2cget -y 1 0x47 0x04 i 2
#
# ->i2cget -y 1 0x47 0x05 i 1
#   Hier geschieht das gleiche nur das wir den Zustand des gpio-pin abfragen den wir der Klasse
#   beim initialisieren als gpio_in mitgeben und nur ein Byte als Antwort bekommen wollen
#
# ->i2cset -y 1 0x47 0x06 0x00
#   Hiermit setzten wir den Zustand des gpio-pin den wir der Klasse beim initialisieren
#   als gpio_out mitgeben
  pass

from machine import Pin, ADC
import i2c_slave

# Diese Variablen müssen an die konkrete beschaltung angepasst werden
GPIO_OUT = 14
GPIO_IN = 13
I2C_BUS = 0
SDA_PIN = 4
SCL_PIN = 5
FREQUENZ = (400 * 1000)
ADDRESS = 0x47

class I2c_ctr:
    def __init__(self, gpio_in, gpio_out):
        self.gpio_in = gpio_in
        self.gpio_out = gpio_out
        self.register = -1
        self.send_value = None
        self.led = Pin("LED", Pin.OUT)
        self.led.on()
        
    def callback(self, i2c, handler):
        if handler == "I2C_SLAVE_RECEIVE":
            # Ein Byte einlesen. Es müssen alle vom Master gesendeten Bytes gelesen werden,
            # sonst hängt die Kommunikation an dieser stelle
            receive = i2c_slave.readByte(i2c)
            # Das erste Byte nach nach dem Adressbyte einlesen
            if self.register == -1:
                # ein ADC wird gelesen
                if receive < 5:
                    val = ADC(receive).read_u16()
                    self.send_value = [((val & 0xFF00) >> 8), (val & 0x00FF)]
                # oder der Zustand des gpio_in wird gelesen
                elif receive == 5:
                    self.send_value = self.gpio_in.value()
                    self.register = receive
                # oder der gpio_out wird mit dem nächsten Byte gesetzt
                elif receive > 5:
                    self.register = receive
            # oder das zweite Byte das den gpio_out setzt einlesen
            else:
                self.gpio_out.value(receive)
                self.register = -1
                
        elif handler == "I2C_SLAVE_REQUEST":
            # Es müssen alle vom Master geforderten
            # Bytes gesendeten werden sonst hängt die Kommunikation an dieser stelle
            # zwei Bytes eines ADC senden
            if self.register < 5:
                i2c_slave.writeBlock(i2c, self.send_value, 2)
            # oder ein Byte des gpio_in senden
            else:
                i2c_slave.writeByte(i2c, self.send_value)
                self.register = -1
                
        elif handler == "I2C_SLAVE_FINISH":
            # Wird hier nicht benötigt
            pass

# Die GPIO's initialisieren
gpio_in = Pin(GPIO_IN, Pin.IN, Pin.PULL_DOWN)
gpio_out = Pin(GPIO_OUT, Pin.OUT)

# Die Klasse mit den GPIO's initialisieren
i2c_ctr = I2c_ctr(gpio_in, gpio_out)

# Das i2c_slave-Modul mit der callback-Funktion der Klasse  initialisieren
# In einem Hintergrund-Prozess wird die callback-Funktion durch Master-Nachrichten aufgerufen
i2c_slave.init(I2C_BUS, SDA_PIN, SCL_PIN, FREQUENZ, ADDRESS, i2c_ctr.callback)
