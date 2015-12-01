FEE_PATH=$(subst $(prefix),,$(shell cd ..; pwd))
KDK_PATH=$(subst $(prefix),,$(shell cd ../../KDK; pwd))
INC_PATH=-I$(FEE_PATH)/inc -I$(KDK_PATH)/inc
LIB_PATH=-L$(FEE_PATH)/lib -L$(KDK_PATH)/lib

SO_FILE=libfee.so
H_FILE=flow_parse.h
C_FILE=flow_parse.c
BIN_FILE=test

.PHONY:ALL
ALL:inc_install $(SO_FILE) lib_install $(BIN_FILE)

inc_install:
	cp $(H_FILE) $(FEE_PATH)/inc/

lib_install:
	cp $(SO_FILE) $(FEE_PATH)/lib/

libfee.so:$(C_FILE)
	gcc -o libfee.so -fpic -shared $(C_FILE) $(INC_PATH)

test:test.c
	gcc -o test test.c $(INC_PATH) $(LIB_PATH) -lfee -lkdk
