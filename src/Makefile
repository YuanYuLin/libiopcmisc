
TARGET = libiopcmisc.so

CFLAGS += -I$(SDKSTAGE)/usr/include/liblua
CFLAGS += -I$(SDKSTAGE)/usr/include/libjsonc
CFLAGS += -I$(SDKSTAGE)/usr/include/
CFLAGS += -I./include/
CFLAGS += 

LDFLAGS += -L$(SDKSTAGE)/lib/ 
LDFLAGS += -L$(SDKSTAGE)/usr/lib/ 
LDFLAGS += -lc -lpthread -ldl
LDFLAGS += -ljson-c
#LDFLAGS += -lluajit-5.1

SRC = 
SRC += ops_misc.c
SRC += ops_dbutil.c
SRC += ops_log.c
SRC += ops_mq.c

include Makefile.include.lib
