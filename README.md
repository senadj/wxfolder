# wxfolder
wxWidgets contrib modules, new classes, apps and other files.
Intended primary use with tdm-gcc compiler and C++11 standard. 
Most wxWidgets contrib module files have been modified so they compile with included build scripts.

Contents:

 - ycc
 - [wxJSON](http://wxcode.sourceforge.net/docs/wxjson/index.html)
 - [wxPlotCtrl](http://wxcode.sourceforge.net/showcomp.php?name=wxPlotCtrl)
 - [awxLed](https://iftools.com/opensource/awx.en.php)



Follow links for original files (with documentation, makefiles, other build scripts...)


# wx-config RPATH mod (prints linker option rpath with --libs switch)
``` sh
sed -i 's/is_installed\ ||\ //' $YWX/bin/wx-config
```
