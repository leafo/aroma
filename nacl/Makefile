
build:
	./scons aroma_x86_64.nexe 

strip:
	ls *.nexe | while read name; do x86_64-nacl-strip $$name -o $$name; done

