## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,em4f linker.cmd package/cfg/matilda_pem4f.oem4f

linker.cmd: package/cfg/matilda_pem4f.xdl
	$(SED) 's"^\"\(package/cfg/matilda_pem4fcfg.cmd\)\"$""\"C:/Users/Ultima1/Github/Tron Academy/Matilda/.config/xconfig_matilda/\1\""' package/cfg/matilda_pem4f.xdl > $@
