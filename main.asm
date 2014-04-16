#include "addresses.asm"
* = main
         ldx #bufaddr
         ldy $421a
dataloop jsr read_controller_bytes
         jsr bufaddr
         tya
         bne dataloop
         jsr run_every_frame
         jmp @$90e695

read_controller_bytes
recloop  lda #1
         sta $4016
         stz $4016
         phy
         ldy #$10
bitloop  lda $4016
         lsr
         rol 0,x
         lsr
         rol 4,x
         lda $4017
         lsr
         rol 2,x
         lsr
         rol 6,x
         dey
         bne bitloop
         ply
         txa
         clc
         adc #8
         tax
         dey
         bne recloop
         rts
