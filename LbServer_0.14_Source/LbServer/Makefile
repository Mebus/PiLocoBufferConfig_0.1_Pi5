#############################################################################
# Makefile for building: LbServer
# $Id: Makefile 1299 2023-04-02 12:06:03Z pischky $
# 
# current version testet on:
#    RaspiOS bullseye (2022-09-22-raspios-bullseye-arm64-lite)
# previous versions have been tested on:
#    openSuSE 11.3 (Make 3.81, gcc 4.5.0)
#    openSuSE 12.3 (Make 3.82, gcc 4.7.2)
#    Raspian 2013-07-26 (Make 3.81, gcc (Debian 4.6.3-14+rpi1))
#    W2K using minGW (gcc version 3.4.2, GNU Make version 3.79.1)
#    SuSE 7 (gcc version 2.95.2, GNU Make version 3.79.1)
#############################################################################

####### check for target operating system (we assume same as host)

ifdef COMSPEC
	target_os   = win32
	target_ext  = .exe
else
	target_os   = linux
endif

objects_dir     = .obj
release_dir     = Release
debug_dir       = Debug
os_dir          = $(target_os)
target          = LbServer

####### Compiler, tools and options

CXX             = g++
# do we really use exceptions (-fexceptions) ?
CXXFLAGS        = -pipe -Wall -W -O2 -g -fexceptions -D_REENTRANT
CPPFLAGS        = -I. -Ishared/$(os_dir) -Ishared -Ishared-lbserver -Wno-unused-parameter -Wformat-overflow=0
#TARGET_ARCH     = -march=i386
OUTPUT_OPTION   = -o $@
LINK.o          = $(CXX) $(LDFLAGS) $(TARGET_ARCH)
#LDFLAGS         = -static
#LDLIBS          = -lm
STRIP           = strip --strip-all

# for linux target we assume the use of the pthreads library
ifeq "linux" "$(target_os)"
    #LDLIBS += -lpthread
    LDFLAGS += -pthread
    CPPFLAGS += -pthread
    # use journald:
    # pkg-config --libs libsystemd    ==>  '-lsystemd'
    # pkg-config --cflags libsystemd  ==>  ''
    CPPFLAGS += -DWITH_JOURNALD $(shell pkg-config --cflags libsystemd)
    LDLIBS += $(shell pkg-config --libs libsystemd)
endif

# on windows it is required to link wsock32.lib (or ws2_32.lib)
ifeq "win32" "$(target_os)"
    LDLIBS += -lwsock32
endif

# this test is ok for gcc 2.95 but not for gcc>=3
ifeq "g++" "$(CXX)"
    gcc_major_version = $(shell gcc --version | cut -d. -f1)
    ifneq "2" "$(gcc_major_version)"
        # additional flags for gcc version 3, 4, ..
        # (not supported in gcc 2.95)
        CXXFLAGS += -fno-use-cxa-atexit
    endif
endif

COPY            := cp -f
COPY_FILE       := $(COPY) -p
COPY_DIR        := $(COPY) -pR
DEL_FILE        := rm -f
DEL_DIR         := rmdir
MKDIR           := mkdir -p
MOVE            := mv -f
SYMLINK         := ln -sf

####### Files

headers = \
	DisplayLcd.h \
	DisplayNormal.h \
	DisplayVerbose.h \
	LbServerConfig.h \
	LcdTopics.h \
	LocoBuffer.h \
	Version.h \
	shared-lbserver/AbstractLoconetDevice.h \
	shared-lbserver/ClientConnection.h \
	shared-lbserver/DisplayFrontend.h \
	shared-lbserver/IDisplay.h \
	shared-lbserver/LoconetServer.h \
	shared-lbserver/TcpServer.h \
	shared/LoconetMessage.h \
	shared/ReceiveLineBuffer.h \
	shared/$(os_dir)/AbstractThread.h \
	shared/$(os_dir)/OsSpecific.h \
	shared/$(os_dir)/SerialPort.h \
	shared/$(os_dir)/SyncObjects.h \

sources = \
	DisplayLcd.cpp \
	LcdTopics.cpp \
	LbServer.cpp \
	LbServerConfig.cpp \
	LocoBuffer.cpp \
	shared-lbserver/AbstractLoconetDevice.cpp \
	shared-lbserver/ClientConnection.cpp \
	shared-lbserver/DisplayFrontend.cpp \
	shared-lbserver/LoconetServer.cpp \
	shared-lbserver/TcpServer.cpp \
	shared/LoconetMessage.cpp \
	shared/ReceiveLineBuffer.cpp \
	shared/$(os_dir)/AbstractThread.cpp \
	shared/$(os_dir)/SerialPort.cpp \
	shared/$(os_dir)/SyncObjects.cpp \

objects = \
	$(objects_dir)/DisplayLcd.o \
	$(objects_dir)/LcdTopics.o \
	$(objects_dir)/LbServer.o \
	$(objects_dir)/LbServerConfig.o \
	$(objects_dir)/LocoBuffer.o \
	$(objects_dir)/AbstractLoconetDevice.o \
	$(objects_dir)/ClientConnection.o \
	$(objects_dir)/DisplayFrontend.o \
	$(objects_dir)/LoconetServer.o \
	$(objects_dir)/TcpServer.o \
	$(objects_dir)/LoconetMessage.o \
	$(objects_dir)/ReceiveLineBuffer.o \
	$(objects_dir)/AbstractThread.o \
	$(objects_dir)/SerialPort.o \
	$(objects_dir)/SyncObjects.o \

# on windows we use the resource file to set icon and version 
# information for the excutable
ifeq "win32" "$(target_os)"
	objects += $(objects_dir)/Resources.o
	headers += resource.h
endif

### implicit rules from GNU Make version 3.79.1 (just for reference)
#CXX = g++
#COMPILE.cc = $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
#COMPILE.cpp = $(COMPILE.cc)
#%.o: %.cpp
#	$(COMPILE.cpp) $(OUTPUT_OPTION) $<
#CC = gcc
#LINK.o = $(CC) $(LDFLAGS) $(TARGET_ARCH)
#%: %.o
#	$(LINK.o) $^ $(LOADLIBES) $(LDLIBS) -o $@

####### Build rules

all: Makefile $(release_dir)/$(target)$(target_ext)

.PHONY: check-os
check-os:
	@echo "Running on $(target_os)"

.PHONY: check-root
check-root:
	@# check for permission
	@if [ "$(shell id -u)" != "0" ]; then \
	  echo "You are not root, run this target as root please!" 2>&1; exit 1; \
	fi

.PHONY: release
release: $(release_dir)/$(target)$(target_ext)

.PHONY: debug
debug: $(debug_dir)/$(target)$(target_ext)

####### Make dirs

$(objects): $(objects_dir)

$(objects_dir):
	$(MKDIR) $(objects_dir)

$(release_dir):
	$(MKDIR) $(release_dir)

$(debug_dir):
	$(MKDIR) $(debug_dir)
	
####### Link

$(debug_dir)/$(target)$(target_ext): $(objects) $(debug_dir)
	$(LINK.o) $(objects) $(LOADLIBES) $(LDLIBS) -o $@

# for minGW we should strip the executable
# (see minGW faq "Why are C++ programs so large?"
# http://www.mingw.org/mingwfaq.shtml#faq-C++size ,
# also see http://www.mingw.org/MinGWiki/index.php/executables%20large )
$(release_dir)/$(target)$(target_ext): $(debug_dir)/$(target)$(target_ext) $(release_dir)
	$(STRIP) $< -o $@

####### Compile

$(objects_dir)/Resources.o: Recourses.rc LoconetOverTcp.ico $(headers)
	windres -i $< -o $@

$(objects_dir)/DisplayLcd.o: DisplayLcd.cpp $(headers)
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<

$(objects_dir)/LcdTopics.o: LcdTopics.cpp $(headers)
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<

$(objects_dir)/LbServer.o: LbServer.cpp $(headers)
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<

$(objects_dir)/LocoBuffer.o: LocoBuffer.cpp $(headers)
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<

$(objects_dir)/LbServerConfig.o: LbServerConfig.cpp $(headers)
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<

$(objects_dir)/AbstractLoconetDevice.o: shared-lbserver/AbstractLoconetDevice.cpp $(headers)
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<

$(objects_dir)/ClientConnection.o: shared-lbserver/ClientConnection.cpp $(headers)
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<

$(objects_dir)/DisplayFrontend.o: shared-lbserver/DisplayFrontend.cpp $(headers)
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<

$(objects_dir)/LoconetServer.o: shared-lbserver/LoconetServer.cpp $(headers)
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<

$(objects_dir)/TcpServer.o: shared-lbserver/TcpServer.cpp $(headers)
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<

$(objects_dir)/LoconetMessage.o: shared/LoconetMessage.cpp $(headers)
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<

$(objects_dir)/ReceiveLineBuffer.o: shared/ReceiveLineBuffer.cpp $(headers)
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<

$(objects_dir)/AbstractThread.o: shared/$(os_dir)/AbstractThread.cpp $(headers)
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<

$(objects_dir)/SerialPort.o: shared/$(os_dir)/SerialPort.cpp $(headers)
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<

$(objects_dir)/SyncObjects.o: shared/$(os_dir)/SyncObjects.cpp $(headers)
	$(COMPILE.cpp) $(OUTPUT_OPTION) $<

####### Clean

clean:
	-$(DEL_FILE) $(objects)
	-$(DEL_DIR)  $(objects_dir)
	-$(DEL_FILE) *~ core *.core

distclean: clean
	-$(DEL_FILE) $(debug_dir)/$(target)
	-$(DEL_FILE) $(debug_dir)/$(target).[Ee][Xx][Ee]
	-$(DEL_DIR)  $(debug_dir)
	-$(DEL_FILE) $(release_dir)/$(target)
	-$(DEL_FILE) $(release_dir)/$(target).[Ee][Xx][Ee]
	-$(DEL_DIR)  $(release_dir)

ifeq "linux" "$(target_os)"

####### Install

.PHONY: install
install: check-root all
	# copy LbServer to /usr/local/bin/
	cp $(release_dir)/LbServer /usr/local/bin/
	chown root:root /usr/local/bin/LbServer
	chmod u=rwx,go=rx /usr/local/bin/LbServer
	# copy LbServer.exe.conf to /etc/lbserver.conf
	cp LbServer.exe.conf /etc/lbserver.conf
	if which dos2unix >/dev/null; then \
	  dos2unix /etc/lbserver.conf; \
	fi
	chown root:root /etc/lbserver.conf
	chmod u=rw,go=r /etc/lbserver.conf

.PHONY: uninstall
uninstall: check-root
	-rm /usr/local/bin/LbServer
	-rm /etc/lbserver.conf

####### Install Service

.PHONY: install-service
install-service: check-root install
	# add user and group 'lbserver' to run daemon
	groupadd --force --system lbserver
	id -u lbserver 2>&1 >/dev/null || \
	  useradd --system --comment "user to run LbServer demaon" --create-home \
	          --shell "/bin/false" --gid lbserver --groups dialout lbserver
	# copy lbserver.service to /etc/systemd/system
	cp lbserver.service /etc/systemd/system/lbserver.service
	chown root:root /etc/systemd/system/lbserver.service
	chmod u=rw,go=r /etc/systemd/system/lbserver.service
	# reload the systemd manager configuration
	systemctl daemon-reload
	# enable service
	systemctl enable lbserver.service
	@echo "Service installed successfully."
	@echo "Edit configuration with:"
	@echo "    sudo vi /etc/lbserver.conf"
	@echo "Start service with:"
	@echo "    sudo systemctl start lbserver.service"
	@echo "Watch log with:"
	@echo "    sudo journalctl -u lbserver.service"

.PHONY: uninstall-service
uninstall-service: check-root
	# stop service
	-systemctl stop lbserver.service
	# disable service
	-systemctl disable lbserver.service
	# delete lbserver.service
	-rm /etc/systemd/system/lbserver.service
	# reload the systemd manager configuration
	-systemctl daemon-reload
	# remove user and group
	-userdel --remove lbserver
	-groupdel lbserver
	
endif

#.PHONY: dist
#dist:
#	@mkdir -p $(objects_dir)/LbServer && $(COPY_FILE) --parents $(sources) $(headers) $(FORMS) $(DIST) $(objects_dir)/LbServer/ && ( cd `dirname $(objects_dir)/LbServer` && $(TAR) LbServer.tar LbServer && $(GZIP) LbServer.tar ) && $(MOVE) `dirname $(objects_dir)/LbServer`/LbServer.tar.gz . && $(DEL_FILE) -r $(objects_dir)/LbServer
