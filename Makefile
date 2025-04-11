# $Id: Makefile 1288 2023-03-29 13:20:14Z pischky $
#
# Makefile to install all PiLocoBuffer requirements
#

FIRMWARE_VER ?= 2023-03-05
firmware_url = https://sourceforge.net/projects/loconetovertcp/files/PiLocoBuffer/firmware/
firmware     = PiLocoBuffer_$(FIRMWARE_VER)

# override on command line like 'make LBSERVER_VER=0.14 install-lbserver'
LBSERVER_VER ?= 0.13
lbserver_url = https://sourceforge.net/projects/loconetovertcp/files/LbServer/$(LBSERVER_VER)/
lbserver     = LbServer_$(LBSERVER_VER)_Source

default : install

install :
	@echo "please run targes separately:"
	@echo "    'make install-lcd'"
	@echo "or  'make install-serial'"
	@echo "or  'make disable-bt' (give /dev/ttyAMA0 back)"
	@echo "or  'make install-lbserver'"
	@echo "or  'make LBSERVER_VER=0.13 install-lbserver' (use version 0.13)"
	@echo "or  'make avr-flash'"
	@echo "or  'make FIRMWARE_VER=2023-03-05 avr-flash' (use version 2023-03-05)"
	@echo "or  'make avr-fuses'"

all : check-root install-lcd install-serial disable-bt avr-flash avr-fuses install-lbserver

# uninstall : uninstall-lcd

check-avrdude :
	@# check that avrdude is available
	@if which avrdude >/dev/null; then \
	  echo "Avrdude is installed."; \
	else \
	  echo "Avrddude is not installed."; \
	  echo "run 'apt install avrdude' as root and start make again."; \
	  exit 1; \
	fi

check-srecord :
	@# check that srecord tools are available
	@if which srec_cat >/dev/null; then \
	  echo "srecord tools are installed."; \
	else \
	  echo "srecord tools are not installed."; \
	  echo "run 'apt install srecord' as root and start make again."; \
	  exit 1; \
	fi

check-root :
	# check for permission
	@if [ "$(shell id -u)" != "0" ]; then \
	  echo "You are not root, run this target as root please!" 2>&1; exit 1; \
	fi

# target to install power button without DT in AVR
install-power : check-root
	# copy config.power.txt to /boot
	cp config.power.txt /boot/config.power.txt
	# change owner to root and permission to 0755
	chown root:root /boot/config.power.txt
	chmod u+rwx,g+rx,o+rx /boot/config.power.txt
	# edit /boot/config.txt and add 'include config.power.txt'
	cp /boot/config.txt /boot/config.txt.bak
	sed -E '/^[# ]*include[ ]+config.power.txt[ ]*(|#.*)$$/ d' /boot/config.txt.bak | sed -e '$$ a\include config.power.txt\n' >/boot/config.txt

uninstall-power : check-root
	# delete /boot/config.power.txt
	rm -f /boot/config.power.txt
	# edit /boot/config.txt and remove 'include config.power.txt'
	cp /boot/config.txt /boot/config.txt.bak
	sed -E '/^[# ]*include[ ]+config.power.txt[ ]*(|#.*)$$/ d' /boot/config.txt.bak >/boot/config.txt

# target that requires DT in AVR
install-lcd : check-root
	# copy hd44780-lcd.rules to /etc/udev/rules.d
	cp hd44780-lcd.rules /etc/udev/rules.d/hd44780-lcd.rules
	# change owner to root and permission to 0644
	chown root:root /etc/udev/rules.d/hd44780-lcd.rules
	chmod u+rw,g+r,o+r /etc/udev/rules.d/hd44780-lcd.rules

uninstall-lcd : check-root 
	# delete /etc/udev/rules.d/hd44780-lcd.rules
	rm -f /etc/udev/rules.d/hd44780-lcd.rules

# target to install lcd without DT in AVR
install-lcd2 : check-root install-lcd
	# copy config.lcd.txt to /boot
	cp config.lcd.txt /boot/config.lcd.txt
	# change owner to root and permission to 0755
	chown root:root /boot/config.lcd.txt
	chmod u+rwx,g+rx,o+rx /boot/config.lcd.txt
	# edit /boot/config.txt and add 'include config.lcd.txt'
	cp /boot/config.txt /boot/config.txt.bak
	sed -E '/^[# ]*include[ ]+config.lcd.txt[ ]*(|#.*)$$/ d' /boot/config.txt.bak | sed -e '$$ a\include config.lcd.txt\n' >/boot/config.txt

uninstall-lcd2 : check-root uninstall-lcd
	# delete /boot/config.lcd.txt
	rm -f /boot/config.lcd.txt
	# edit /boot/config.txt and remove 'include config.lcd.txt'
	cp /boot/config.txt /boot/config.txt.bak
	sed -E '/^[# ]*include[ ]+config.lcd.txt[ ]*(|#.*)$$/ d' /boot/config.txt.bak >/boot/config.txt

disable-bt : check-root
	@# see: https://forums.raspberrypi.com/viewtopic.php?t=258661
	@# see: https://linuxhint.com/disable-bluetooth-raspberry-pi/
	# edit config.txt and add: dtoverlay=disable-bt
	cp /boot/config.txt /boot/config.txt.bak
	sed -E '/^[# ]*dtoverlay[ ]*=[ ]*(pi3-){0,1}disable-bt[ ]*(|#.*)$$/ d' \
	    /boot/config.txt.bak | \
	sed -e '$$ a\dtoverlay=disable-bt\n' >/boot/config.txt
	# disable hciuart.service
	systemctl disable hciuart.service

enable-bt : check-root
	@# see: https://forums.raspberrypi.com/viewtopic.php?t=258661
	@# see: https://linuxhint.com/disable-bluetooth-raspberry-pi/
	# edit config.txt and remove: 'dtoverlay=disable-bt' or 'dtoverlay=pi3-disable-bt'
	cp /boot/config.txt /boot/config.txt.bak
	sed -E '/^[# ]*dtoverlay[ ]*=[ ]*(pi3-){0,1}disable-bt[ ]*(|#.*)$$/ d' \
	    /boot/config.txt.bak >/boot/config.txt
	# enable hciuart.service
	systemctl enable hciuart.service

install-serial : check-root
	@# see: https://www.abelectronics.co.uk/kb/article/1035/serial-port-setup-in-raspberry-pi-os
	@# see: 'sudo raspi-config' 3, I6: 'no', 'yes'
	@# see: https://github.com/RPi-Distro/raspi-config/blob/master/raspi-config (line 1020)
	@# see: https://forums.raspberrypi.com/viewtopic.php?t=322786
	# modify /boot/cmdline.txt (remove: "console=serial0,115200")
	# modify /boot/config.txt (add/edit: "enable_uart=1")
	raspi-config do_serial 2 nonint

uninstall-serial : check-root
	# modify /boot/cmdline.txt (add: "console=serial0,115200")
	# modify /boot/config.txt (add/edit: "enable_uart=1")
	raspi-config do_serial 0 nonint

$(lbserver).zip :
	@echo "Trying to download '$(lbserver).zip':"
	@echo "curl --fail --location $(lbserver_url)$(lbserver).zip --output $(lbserver).zip"
	@curl --fail --location $(lbserver_url)$(lbserver).zip --output $(lbserver).zip \
	|| (echo "Could not download '$(lbserver).zip'."; \
	    echo "You may try 'make LBSERVER_VER=0.13' or download manually."; \
	    exit 1)

make-lbserver : $(lbserver).zip
	unzip -o $(lbserver).zip
	make -C $(lbserver)/LbServer/ all

install-lbserver : check-root make-lbserver
	make -C $(lbserver)/LbServer/ install-service
	cp lbserver.conf /etc/lbserver.conf
	chown root:root /etc/lbserver.conf
	chmod u=rw,go=r /etc/lbserver.conf

uninstall-lbserver : check-root make-lbserver
	make -C $(lbserver)/LbServer/ uninstall-service
	make -C $(lbserver)/LbServer/ uninstall

$(firmware).zip :
	@echo "Trying to download '$(firmware).zip':"
	@echo "curl --fail --location $(firmware_url)$(firmware).zip --output $(firmware).zip"
	@curl --fail --location $(firmware_url)$(firmware).zip --output $(firmware).zip \
	|| (echo "Could not download '$(firmware).zip'."; \
	    echo "You may try 'make FIRMWARE_VER=2023-03-05' or download manually."; \
	    exit 1)

PiLocoBuffer.hex : $(firmware).zip
	unzip -o $(firmware).zip
	cp $(firmware)/PiLocoBuffer.hex .

avr-flash : check-avrdude check-root PiLocoBuffer.hex
	avrdude -c PI_LB_01 -C +PI_LB_avrdude.conf -p atmega328pb -U flash:w:PiLocoBuffer.hex:i

avr-fuses : check-avrdude check-root
	avrdude -c PI_LB_01 -C +PI_LB_avrdude.conf -p atmega328pb -U efuse:w:0xF7:m
	avrdude -c PI_LB_01 -C +PI_LB_avrdude.conf -p atmega328pb -U hfuse:w:0xD9:m
	avrdude -c PI_LB_01 -C +PI_LB_avrdude.conf -p atmega328pb -U lfuse:w:0xFF:m

.PHONY : default all install uninstall check-srecord check-avrdude check-root
.PHONY : install-power uninstall-power install-lcd uninstall-lcd 
.PHONY : make-lbserver install-lbserver avr-flash avr-fuses

