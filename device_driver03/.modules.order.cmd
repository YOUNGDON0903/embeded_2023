cmd_/home/kim/working/device_driver03/modules.order := {   echo /home/kim/working/device_driver03/seg_driver.ko; :; } | awk '!x[$$0]++' - > /home/kim/working/device_driver03/modules.order
