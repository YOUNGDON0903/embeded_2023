cmd_/home/kim/working/device_driver03/Module.symvers := sed 's/\.ko$$/\.o/' /home/kim/working/device_driver03/modules.order | scripts/mod/modpost -m -a  -o /home/kim/working/device_driver03/Module.symvers -e -i Module.symvers   -T -
