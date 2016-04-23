FEE_PATH=$(subst $(prefix),,$(shell cd ..; pwd))
KDK_PATH=$(subst $(prefix),,$(shell cd ../../KDK; pwd))
INC_PATH=-I$(FEE_PATH)/inc -I$(KDK_PATH)/inc
LIB_PATH=-L$(FEE_PATH)/lib -L$(KDK_PATH)/lib
C_FLAG=-g -Wall

SO_FILE=libfee.so
H_FILE=flow_parse.h flow_load.h flow_runtime.h module_parse.h module_load.h log_load.h engine_exec.h fee_daemon.h
C_FILE=flow_parse.c flow_load.c flow_runtime.c module_parse.c module_load.c log_load.c engine_exec.c
BIN_FILE=test fee_daemon

.PHONY:ALL
ALL:inc_install $(SO_FILE) lib_install $(BIN_FILE) bin_install $(TEST_SO_FILE)

inc_install:
	cp $(H_FILE) $(FEE_PATH)/inc/

lib_install:
	cp $(SO_FILE) $(FEE_PATH)/lib/

bin_install:
	cp $(BIN_FILE) $(FEE_PATH)/bin/

libfee.so:$(C_FILE)
	gcc -o $@ -fpic -shared $(C_FILE) $(INC_PATH) $(LIB_PATH) -lkdk -lm


test:test.c $(C_FILE)
	gcc -o $@ test.c $(INC_PATH) $(LIB_PATH) -lfee -lkdk -ldl -lm

fee_daemon:fee_daemon.c
	gcc -o $@ fee_daemon.c $(INC_PATH) $(LIB_PATH) -lfee -lkdk -ldl -lm

clean:
	rm -rf $(BIN_FILE) $(SO_FILE) $(TEST_SO_FILE)
