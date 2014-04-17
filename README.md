The recent discovery of arbitrary code execution in Super Metroid opens up
the possibility for entertaining playaround TASes of the game that reprogram
the game on the fly. However, quite a bit of bootstrapping is necessary before
that can be done, and tools for doing that are quite lacking. When I started
trying to implement this, I found that to set up the main resident code, I had
to fist write it in assembly, then assemble it, then write new assembly code
for loading those bytes, then assemble that code, and convert the result into
lsnes input. That's not something you want to do manually if you can avoid it!

So instead of doing things that way, I wrote a lsnes input file pre-processor
that allows you to insert assembly code inline in input files using a simple
macro language. As a simple example, the end of the current any% movie
looks like

F.|........AX......|................|................|................
F.|........AX......|................|................|................
F.|........AX......|................|................|................
F.|......l.AX......|................|................|................
F.|........AX......|................|................|................
F.|.........X......|................|................|................
F.|...Su...A.L.0..3|....ud...XL.0...|B...ud.r.X....2.|.Ys......XLR0.2.
F.|................|................|................|................
F.|..s..dl.A.L.0..3|....u..rA..R0...|B...ud.r........|.........XL.....
F.|................|................|................|................
F.|B.......A.L.0..3|BY.Su.....L....3|B...udlr........|.Ys......XLR012.
F.|................|................|................|................
F.|BY..udlrA.L.0..3|....u..rAX..0...|B...ud.r...R..2.|.........XL.....
F.|................|................|................|................
F.|.Y......A..R01..|................|.Ys.........01..|................

The normal controller input ends on line 6 - the rest corresponds to
assembly code. This can be generated using

F.|........AX......|................|................|................
F.|........AX......|................|................|................
F.|........AX......|................|................|................
F.|......l.AX......|................|................|................
F.|........AX......|................|................|................
F.|.........X......|................|................|................
@inline 1
 lda #$4218
 sta $0c68
 ply
@inline 1
 lda #$26
 sta $998
@inline 1
 lda #$80
 sta $7ed821
@inline 1
 lda #$12cf
 sta $09c8
@inline 1
 stz $0c40

For something as simple as this, there isn't much of an improvement. A
more realistic example is

F.|........AX......|................|................|................
F.|........AX......|................|................|................
F.|........AX......|................|................|................
F.|........AX......|................|................|................
F.|........AX......|................|................|................
F.|......l.AX......|................|................|................
F.|........AX......|................|................|................
F.|.........X......|................|................|................
@global
 main            = $f440
 main_long       = $7ef440
 bufaddr         = $f500
 run_every_frame = $f600
 tbuffer         = $01df
# Make sure we're called repeatedly
@inline 1
 lda #$4218
 sta $0c68
 ply
# Bootstrap the following code, placing it at the previously defined
# main_long, which is a region of free space in ram.
# This will be our main loop later.
@boot main_long 1
 ldx #bufaddr
 ldy $421a
 dataloop
 jsr read_controller_bytes
 jsr bufaddr
 tya
 bne dataloop
 jsr run_every_frame
 jmp @$90e695

 read_controller_bytes
 recloop
 lda #1
 sta $4016
 stz $4016
 phy
 ldy #$10
 bitloop
 lda $4016
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
# Set up our per-frame jump to the main loop
@boot tbuffer 1
 jmp main_long
@inline 1
 lda #tbuffer
 sta $a42
# At this point we're fully bootstrapped. We can now return control to the
# game (but it will now call our resident function every frame)
@inline 0
 stz $0c40
# We can now play the game normally...
F.|.........X......|................|................|................
F.|B......r........|................|................|................
F.|B......r........|................|................|................
F.|B......r........|................|................|................
F.|B......r........|................|................|................
F.|B......r........|................|................|................
# And run large amounts of code any frame we want, at a far greater
# speed than the bootstrapping code. This does not interfer with normal
# input during this frame. For example, this code jumps to the end of the
# game. But since we're now dealing with subframe input, we can easily
# do much more now.
@code
 lda #$26
 sta $998
