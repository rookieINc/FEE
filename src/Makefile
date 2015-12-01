FEE_PATH=$(subst $(prefix),,$(shell cd ..; pwd))
KDK_PATH=$(subst $(prefix),,$(shell cd ../../KDK; pwd))
INC_PATH=-I$(FEE_PATH)/inc -I$(KDK_PATH)/inc
LIB_PATH=-L$(FEE_PATH)/lib -I$(KDK_PATH)/lib

SO_FILE=libfee.so
H_FILE=flow_parse.h
C_FILE=flow_parse.c

.PHONY:ALL
ALL:inc_install $(SO_FILE) lib_install

inc_install:
	cp $(H_FILE) $(FEE_PATH)/inc/

lib_install:
	cp $(SO_FILE) $(FEE_PATH)/lib/

libfee.so:$(C_FILE)
	gcc -o libfee.so -fpic -shared $(C_FILE) $(INC_PATH)

