;==============================================================================
; Wave HDMA Table Rebuild — Hand-optimized 65816 Assembly
;
; Called from C: waveRebuildHdmaTable(u8 *hdma_buf, WaveParams *wp)
;
; tcc-65816 passes ALL pointers as 4-byte far pointers on the stack:
;   [addr_lo][addr_hi][bank_lo][bank_hi]
;
; WaveParams struct layout:
;   +0 = amp   (u16)
;   +2 = freq  (u16)
;   +4 = phase (u16)
;   +6 = dir   (u8)
;==============================================================================

.include "hdr.asm"
.accu 16
.index 16
.16bit

.BASE $C0

.SECTION ".waveRebuildAsmText" SUPERFREE

waveRebuildHdmaTable:
    phb                         ; save data bank (+1 on stack)

    rep #$30
.accu 16
.index 16

    ;------------------------------------------------------------------
    ; Allocate 10 bytes of locals on stack
    ;------------------------------------------------------------------
    tsa
    sec
    sbc #10
    tas

    ;------------------------------------------------------------------
    ; Stack layout (all S-relative):
    ;
    ;  1,s ..  2,s  = lut_row_offset  (local)
    ;  3,s ..  4,s  = phase_acc       (local)
    ;  5,s ..  6,s  = freq            (local)
    ;  7,s ..  8,s  = output_ptr      (local)
    ;  9,s .. 10,s  = counter         (local)
    ; 11,s          = saved B         (PHB)
    ; 12,s .. 14,s  = return address  (JSL, 3 bytes)
    ; 15,s .. 16,s  = hdma_buf addr   (param 1)
    ; 17,s .. 18,s  = hdma_buf bank   (param 1)
    ; 19,s .. 20,s  = wp addr         (param 2)
    ; 21,s .. 22,s  = wp bank         (param 2)
    ;------------------------------------------------------------------

    ;------------------------------------------------------------------
    ; Read WaveParams struct via far pointer
    ; Load wp into tcc__r0/tcc__r0h for indirect long addressing
    ; (D = $0000, so tcc__r0 is at its correct ZP address)
    ;------------------------------------------------------------------
    lda 19,s                    ; wp addr
    sta.b tcc__r0
    lda 21,s                    ; wp bank
    sta.b tcc__r0h

    ; amp_idx = wp->amp & 0x1F → row_offset = amp_idx * 512
    lda.b [tcc__r0]             ; read wp->amp (offset 0)
    and #$001F
    xba                         ; * 256
    asl a                       ; * 512 (bytes per LUT row)
    sta 1,s                     ; → lut_row_offset

    ; phase = wp->phase (offset 4)
    ldy #4
    lda.b [tcc__r0],y
    sta 3,s                     ; → phase_acc

    ; freq = wp->freq (offset 2)
    ldy #2
    lda.b [tcc__r0],y
    sta 5,s                     ; → freq

    ; output_ptr = hdma_buf addr
    lda 15,s
    sta 7,s                     ; → output_ptr

    ; counter = 224
    lda #224
    sta 9,s                     ; → counter

    ;------------------------------------------------------------------
    ; Main loop: 224 scanlines
    ;------------------------------------------------------------------
@loop:
    ; --- Compute LUT index ---
    ; angle = (phase_acc >> 8) & 0xFF
    ; lut_index = lut_row_offset + angle * 2
    lda 3,s                     ; phase_acc
    xba                         ; high byte → low (= >>8)
    and #$00FF                  ; mask to 0-255
    asl a                       ; * 2 (each entry is 2 bytes)
    clc
    adc 1,s                     ; + lut_row_offset
    tax                         ; X = byte offset into sine_lut_2d

    ; --- Read displacement from LUT (long indexed) ---
    lda.l sine_lut_2d,x         ; 16-bit signed displacement
    sta.b tcc__r0               ; temp in ZP (D=0, so this is correct)

    ; --- Write 3-byte HDMA entry ---
    lda 7,s                     ;
    tax                         ; X = output buffer pointer (WRAM addr)

    sep #$20
.accu 8
    lda #$01
    sta.l $7E0000,x             ; byte 0: scanline count = 1
    lda.b tcc__r0               ; displacement low byte
    sta.l $7E0001,x             ; byte 1: scroll_lo
    lda.b tcc__r0+1             ; displacement high byte
    sta.l $7E0002,x             ; byte 2: scroll_hi
    rep #$20
.accu 16

    ; --- Advance output pointer by 3 ---
    lda 7,s
    clc
    adc #3
    sta 7,s

    ; --- Advance phase accumulator ---
    lda 3,s                     ; phase_acc
    clc
    adc 5,s                     ; + freq
    sta 3,s

    ; --- Decrement counter ---
    lda 9,s
    dec a
    sta 9,s
    bne @loop

    ;------------------------------------------------------------------
    ; Write HDMA terminator ($00)
    ;------------------------------------------------------------------
    lda 7,s                     ;
    tax                         ; final output pointer position
    sep #$20
.accu 8
    lda #$00
    sta.l $7E0000,x             ; terminator byte
    rep #$20
.accu 16

    ;------------------------------------------------------------------
    ; Cleanup: deallocate locals, restore bank, return
    ;------------------------------------------------------------------
    tsa
    clc
    adc #10                     ; remove 10 bytes of locals
    tas
    plb                         ; restore data bank
    rtl

.ENDS

; Required empty sections for WLA-DX linker compatibility
.BASE $00
.RAMSECTION "ram.wave_rebuild_asm.data" APPENDTO "globram.data"
.ENDS

.SECTION ".wave_rebuild_asm.data" APPENDTO "glob.data"
.ENDS

.SECTION ".wave_rebuild_asm.rodata" SEMIFREE ORG $8000
.ENDS

.BASE $00
.RAMSECTION ".wave_rebuild_asm.bss" BANK $7e SLOT 2
.ENDS
