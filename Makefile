all:
	make -C src
	make -C xpidl

clean:
	make -C src clean
	make -C xpidl clean
