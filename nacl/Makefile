
strip:
	ls *.nexe | while read name; do x86_64-nacl-strip $$name -o $$name; done

deploy:
	rsync -rvuzLR *.html *.nexe *.nmf js/ game/ examples/ leaf@leafo.net:www/aroma/demo

32:
	echo "CONFIG_NACL_ARCH=i686" >> ../tup.config
	echo "CONFIG_NACL_BIN_NAME=aroma_tmp" >> ../tup.config
	tup upd
	mv aroma_tmp_i686.nexe aroma_i686.nexe
	git checkout -- ../tup.config


