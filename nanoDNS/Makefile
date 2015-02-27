CC=gcc

nanomake: nanodns.c
	$(CC) -o nanodns nanodns.c -std=gnu99

openwrt: nanodns.c
	export STAGING_DIR=$$PWD; \
`find /home -name mipsel-openwrt-linux-gcc -print -quit -print 2>/dev/null` -o nanodns -Os -s nanodns.c

.PHONY: clean

clean:
	rm -f *.o *.exe nanodns
	
