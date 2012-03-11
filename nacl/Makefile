
build: nacl.lua.h
	./scons aroma_x86_64.nexe 

nacl.lua.h: lib/nacl.moon
	moonc lib
	xxd -i lib/nacl.lua > $@

strip:
	ls *.nexe | while read name; do x86_64-nacl-strip $$name -o $$name; done

