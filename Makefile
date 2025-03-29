SELF_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

all: fwxenv wxconfig yconfig wxjson wxplotctrl libawx

fwxenv:	
	@[ ! -z `type -t buildwxconfig` ] || ( echo -e "\n\nfwx environment is not set!  Try this first:\nsource $(SELF_DIR)fwx-tdm\n\n" )

yconfig: wxconfig
	@echo "";  echo "############################## y-config"
	bash -i -c buildyconfig
wxjson: wxconfig
	@echo "";  echo "############################## wxJSON"
	bash -i -c buildwxjson
wxplotctrl: wxconfig
	@echo "";  echo "############################## wxPlotCtrl"
	bash -i -c buildwxplotctrl
libawx: wxconfig
	@echo "";  echo "############################## libawx"
	bash -i -c buildlibawx
wxwidgets:
	@echo "";  echo "############################## wxWidgets"
	bash -i -c buildwxrelease
