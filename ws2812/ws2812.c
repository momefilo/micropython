// ws2812B
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "asm.pio.h"
#include "hardware/clocks.h"
#include "hardware/dma.h"
#include "py/runtime.h"
#include "py/obj.h"
// execfile("ws2812_demo.py")
uint Pio_offset_asm;// Die Speicheradresse des PIO-Programms "asm.pio"
static uint32_t *Buf_asm;// Die Daten welche von der Statemaschine verarbeitet werden
uint Dma_asm;// Der Kanal zum Speicherbereich der Daten
PIO Pio_asm = pio0;// Die PIO-Bank (pio0,pio1)
int Sm_asm = 0;// Die Statemaschine (0,1,2,3)
uint Bufsize_asm;// Anzahl led's. Der Umfang der 24bit-Daten in uint32
uint Baudrate_asm = 800 * 1000;// Die Baudrate des Kanals 800KHz 1250ns Bitdauer

//static mp_obj_t zeiger;

static mp_obj_t ws2812b_info(){
    printf("ws2812 steuert nur einen ws2812-LED-Strip\n");
    printf("aber schneller. Die Funktionen sind:\n\n");
    printf("ws2812.init(Pin, Laenge)\n");
    printf("ws2812.set(num_led, [r, g, b], optional_set)\n");
    printf("#ohne optionel_set muss ws2812.write() aufgerufen werden\n");
    printf("ws2812.write() #schreibt das Array auf den Strip\n");
    printf("ws2812.move(Richtung)\n");
    printf("shiftet den gesamten Strip um eine LED in Richtung\n");
    printf("Richtung = 0 = rueckwaerts, Richtung != 0 = vorwaerts\n");
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(ws2812b_info_obj, ws2812b_info);

static void __isr __not_in_flash_func(asm_handle)(void){
	dma_hw->ints0 = 1u << Dma_asm;// clear the irq
	//Leseadesse auf Datenanfang setzten
    dma_channel_set_read_addr(Dma_asm, &Buf_asm[0], true);
//    dma_channel_set_read_addr(Dma_asm, (volatile void *)(uint32_t)zeiger, true);
	// Reset-Einsprungpunkt zur Statemaschine aufrufen
	pio_sm_exec(Pio_asm, Sm_asm, asm_set_bits(Pio_offset_asm));
}

static mp_obj_t ws2812b_init(mp_obj_t _pin, mp_obj_t _len){
    uint8_t pin = mp_obj_get_int(_pin);
    uint16_t len = mp_obj_get_int(_len);
    // Datenspeicher allokieren 
    Bufsize_asm = len;
    Buf_asm = m_malloc(sizeof(uint32_t)*len);
//    zeiger = MP_OBJ_FROM_PTR(&Buf_asm[0]);
    MP_REGISTER_ROOT_POINTER(uint32_t *Buf_asm);
    // Datenkanal-Transfer an Datenumfang anpassen
    Dma_asm = dma_claim_unused_channel(true);
    dma_channel_set_trans_count(Dma_asm, len, true);
    // set up PIO
    Pio_offset_asm = pio_add_program(Pio_asm, &asm_program);
    asm_program_init(Pio_asm, Sm_asm, Pio_offset_asm, pin, Baudrate_asm);
    // setup the dma
    dma_channel_config dma_cfg = dma_channel_get_default_config(Dma_asm);
    // Die Bitbreite eines Transfers
    channel_config_set_transfer_data_size(&dma_cfg, DMA_SIZE_32);// default
    // Keine automatische Rrhoehung des Zieladesse nach einen Transfer
    channel_config_set_write_increment(&dma_cfg, false);// default
    // Den Interrupt auf den Eingang der PIO-Statemaschine setzten
    channel_config_set_dreq(&dma_cfg, DREQ_PIO0_TX0);
    dma_channel_configure(Dma_asm, &dma_cfg,
        &pio0_hw->txf[0],//destination
        NULL,			//source
        len,		    //counts before call irq
        false			//don't start yet
    );
    // Interrupt 0 setzen
    dma_channel_set_irq0_enabled(Dma_asm, true);
    irq_set_exclusive_handler(DMA_IRQ_1, asm_handle);
    irq_set_enabled(DMA_IRQ_1, true);
    //start the DMA to pull data from Buf_asm to pin
    asm_handle();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(ws2812_init_obj, ws2812b_init);

static mp_obj_t ws2812b_set(size_t n_args, const mp_obj_t *args){
    if(n_args < 2) {
        mp_raise_NotImplementedError("Min. 2 Parameter:\nws2812.set(LEDnr, [r, g, b]) or\n ws2812.set(LEDnr, [g, r, b], now)");
        return mp_const_none;
    }
    uint16_t led = mp_obj_get_int(args[0]);
    mp_obj_iter_buf_t momefilo_buf;
    mp_obj_t item, iterable = mp_getiter(args[1], &momefilo_buf);
    uint32_t color = 0;
    uint8_t i = 3;
    while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
        uint8_t val = mp_obj_get_int(item);
        color  =  (color | (val << (8 * i)));
        i--;
    }
//    uint32_t *p = MP_OBJ_TO_PTR(zeiger);
//    p[led] = color;
    Buf_asm[led] = color;
    if(n_args > 2)asm_handle();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ws2812_set_obj, 0, 3, ws2812b_set);

static mp_obj_t ws2812b_write(){
    asm_handle();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(ws2812b_write_obj, ws2812b_write);

static mp_obj_t ws2812b_move(mp_obj_t _dir){
    if( mp_obj_get_int(_dir) > 0){
        uint32_t tmp = Buf_asm[Bufsize_asm-1];
        for(int i=Bufsize_asm-1; i>0; i--){
            Buf_asm[i] = Buf_asm[i-1];
        }
        Buf_asm[0] = tmp;
    }
    else {
        uint32_t tmp = Buf_asm[0];
        for(int i=0; i<Bufsize_asm-1; i++){
            Buf_asm[i] = Buf_asm[i+1];
        }
        Buf_asm[Bufsize_asm-1] = tmp;
    }
    asm_handle();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(ws2812b_move_obj, ws2812b_move);

// Export zu micropython
static const mp_rom_map_elem_t ws2812_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ws2812) },
    { MP_ROM_QSTR(MP_QSTR_info), MP_ROM_PTR(&ws2812b_info_obj) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&ws2812_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_set), MP_ROM_PTR(&ws2812_set_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&ws2812b_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_move), MP_ROM_PTR(&ws2812b_move_obj) },
};
static MP_DEFINE_CONST_DICT(ws2812_module_globals, ws2812_module_globals_table);

const mp_obj_module_t ws2812_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&ws2812_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_ws2812, ws2812_user_cmodule);
