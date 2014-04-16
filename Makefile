%: %.c
	gcc -o $@ $<
%.o: %.asm
	./enas $< $@
bootstrap.asm: main.o copy.o jump.o addresses.asm loadbytes bootstrap.sh
	./bootstrap.sh $< > $@
