include Makefile.d

all: server

server: prepare $(MODULES)
	@$(MAKE) -f Makefile.$@ -C ./output/
	@cp ./conf/server.conf ./output/conf -v
	@mv ./output/$@ ./output/bin -v

prepare:
	@if [ ! -d ./output ]; then mkdir ./output;	fi
	@if [ ! -d ./output/bin ]; then mkdir ./output/bin;	fi
	@if [ ! -d ./output/conf ]; then mkdir ./output/conf;	fi
	@if [ ! -d ./output/lib ]; then mkdir ./output/lib;	fi
	@cp -v Makefile.server ./output/ -v

$(MODULES):
	@$(MAKE) -f Makefile.$@ -C $(SRCDIR)/$@
	@mv $(SRCDIR)/$@/*.o ./output/

clean:
	rm -rf ./output/ -v
	rm -rf $(SRCDIR)/*/*.o -v