dts-test:
	dtc -I dts -O dtb -o dts-test.dtbo dts-test.dts
	sudo cp dts-test.dtbo /boot/overlays/
	# @cat /boot/config.txt | grep dts-test || \
	# if [ $$? -ne 0 ]; then \
	# 	sudo chmod 666 /boot/config.txt; \
    # 	sudo echo dtoverlay=dts-test >> /boot/config.txt; \
	# fi

clean:
	-rm dts-test.dtbo