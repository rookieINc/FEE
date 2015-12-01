FEE_PATH=$(subst $(prefix),,$(shell pwd))
.PHONY:ALL
ALL:
	cd $(FEE_PATH)/src && make
