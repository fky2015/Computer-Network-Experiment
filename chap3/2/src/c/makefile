

byte_stuffing: byte-stuffing.out

bit_stuffing: zero-bit-stuffing.out

byte-stuffing.out: byte-stuffing.c basiclib.c byte-stuffing-lib.c
	gcc $^ -o $@
	./$@


zero-bit-stuffing.out: zero-bit-stuffing.c basiclib.c zero-bit-stuffing-lib.c
	gcc $^ -o $@
	./$@

clean:
	rm *.out
