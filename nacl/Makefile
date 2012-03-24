
build:
	tup upd ../src/lib
	./scons aroma_x86_64.nexe

watch:
	coffee -c -w js/*.coffee

strip:
	ls *.nexe | while read name; do x86_64-nacl-strip $$name -o $$name; done

deploy: strip
	rsync -rvuzLR *.html *.nexe *.nmf js/ game/ examples/ leaf@leafo.net:www/aroma/demo


