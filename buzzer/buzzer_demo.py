# momefilo for micropython
import buzzer
import _thread
buzzer.init(15)
c  = 0
cs = 1
d  = 2
ds = 3
e  = 4
f  = 5
fs = 6
g  = 7
gs = 8
a  = 9
h  = 10
b  = 11
oktave = 1

def sound_PeerGynt():
	buzzer.play(g,4,oktave)
	buzzer.play(e,4,oktave)
	buzzer.play(d,4,oktave)
	buzzer.play(c,4,oktave)
	buzzer.play(d,4,oktave)
	buzzer.play(e,4,oktave)
	buzzer.play(g,4,oktave)
	buzzer.play(e,4,oktave)
	buzzer.play(d,4,oktave)
	buzzer.play(c,4,oktave)
	buzzer.play(d,8,oktave)
	buzzer.play(e,8,oktave)
	buzzer.play(d,8,oktave)
	buzzer.play(e,8,oktave)
	buzzer.play(g,4,oktave)
	buzzer.play(e,4,oktave)
	buzzer.play(g,4,oktave)
	buzzer.play(a,4,oktave)
	buzzer.play(e,4,oktave)
	buzzer.play(a,4,oktave)
	buzzer.play(g,4,oktave)
	buzzer.play(e,4,oktave)
	buzzer.play(d,4,oktave)
	buzzer.play(c,1,oktave)

_thread.start_new_thread(sound_PeerGynt, ())
