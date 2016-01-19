SELF_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

all: fwxenv wxconfig yconfig wxjson wxthings wxplotctrl wxflatnotebook libawx

fwxenv:	
	@[ ! -z `type -t buildwxconfig` ] || ( echo -e "\n\nfwx environment is not set!  Try this first:\nsource $(SELF_DIR)fwx-tdm\n\n" )

wxconfig:
	@echo "";  echo "############################## wx-config"
	bash -i -c buildwxconfig
yconfig: wxconfig
	@echo "";  echo "############################## y-config"
	bash -i -c buildyconfig
wxjson: wxconfig
	@echo "";  echo "############################## wxJSON"
	bash -i -c buildwxjson
wxthings: wxconfig
	@echo "";  echo "############################## wxThings"
	bash -i -c buildwxthings
wxplotctrl: wxconfig
	@echo "";  echo "############################## wxPlotCtrl"
	bash -i -c buildwxplotctrl
wxflatnotebook: wxconfig
	@echo "";  echo "############################## wxFlatNotebook"
	bash -i -c buildwxflatnotebook
libawx: wxconfig
	@echo "";  echo "############################## libawx"
	bash -i -c buildlibawx
wxwidgets:
	@echo "";  echo "############################## wxWidgets"
	bash -i -c buildwxrelease
