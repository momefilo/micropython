/* momefilo for micropython
 * Dieses Modul gibt die Tonleiter in drei Oktaven
 * auf einen am Pin angeschlossenen Passiv-Buzzer
 * mit einer Ganznotendauer von 1,5 Sekunden wieder
*/
#include <stdio.h>
#include "hardware/pwm.h"
#include "hardware/clocks.h"
#include "hardware/gpio.h"
#include "py/runtime.h"
#include "py/obj.h"

const uint16_t FreqArray[]={1047,1109,1175,1245,1319,1397,1480,1568,1661,1761,1865,1976};
uint Slice_num;
const uint16_t Dauer = 1500;
uint8_t Pin;

static mp_obj_t buzzer_info(){
    printf("buzzer spielt drei Okraven auf einem am Pin angeschlossenen Passiv-Buzzer\n");
    printf("Vor Benutzung ist das Modul zu initialisieren:\n");
    printf("buzzer.init(buzzer_pin)\n\n");
    printf("buzzer.play(note, dauer, oktave)\n");
    printf("note c=0 bis b=11\n");
    printf("dauer = 1,5 Sekunden Tondauer/dauer\n");
    printf("oktave = 0 bis 2 (Grundoktave bis drei Oktaven hoeher)\n");
    printf("buzzer.play(9, 1, 0) gibt den Ton 'a'\n");
    printf("fuer1,5 Sekunden in der Grundoktave auf dem buzzer aus\n");
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(buzzer_info_obj, buzzer_info);

static mp_obj_t buzzer_init(mp_obj_t _pin){
    Pin = mp_obj_get_int(_pin);
    gpio_set_function(Pin, GPIO_FUNC_PWM);
    Slice_num = pwm_gpio_to_slice_num(Pin);
    pwm_set_clkdiv(Slice_num, 80.f);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(buzzer_init_obj, buzzer_init);

static mp_obj_t buzzer_play(mp_obj_t _note, mp_obj_t _dauer, mp_obj_t _oktave){
    uint8_t note = mp_obj_get_int(_note);
    uint8_t dauer = mp_obj_get_int(_dauer);
    uint8_t oktave = mp_obj_get_int(_oktave);
    
    pwm_set_enabled(Slice_num, true);
	if(oktave > 1){ pwm_set_clkdiv(Slice_num, 40.f); }
	else if(oktave == 1){ pwm_set_clkdiv(Slice_num, 80.f); }
	else{ pwm_set_clkdiv(Slice_num, 160.f); }
	pwm_set_wrap( Slice_num, (3125*1000)/FreqArray[note]);
	pwm_set_chan_level(Slice_num, PWM_CHAN_B, (3125*1000)/(FreqArray[note]*2));
	sleep_ms(Dauer/dauer);
	pwm_set_enabled(Slice_num, false);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_3(buzzer_play_obj, buzzer_play);

static const mp_rom_map_elem_t buzzer_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_buzzer) },
    { MP_ROM_QSTR(MP_QSTR_info), MP_ROM_PTR(&buzzer_info_obj) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&buzzer_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_play), MP_ROM_PTR(&buzzer_play_obj) },
};
static MP_DEFINE_CONST_DICT(buzzer_module_globals, buzzer_module_globals_table);

const mp_obj_module_t buzzer_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&buzzer_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_buzzer, buzzer_user_cmodule);
