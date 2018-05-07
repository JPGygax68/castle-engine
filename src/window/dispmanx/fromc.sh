#!/bin/bash

h2pas -d /opt/vc/include/bcm_host.h -o bcm_host.pas

cp /opt/vc/include/interface/vmcs_host/vc_dispmanx.h pp_temp/vc_dispmanx.h_1
sed -r 's/VCHPRE_//'  pp_temp/vc_dispmanx.h_1 >pp_temp/vc_dispmanx.h_2
sed -r 's/VCHPOST_//' pp_temp/vc_dispmanx.h_2 > pp_temp/vc_dispmanx.h_3
cp pp_temp/vc_dispmanx.h_3 pp_temp/vc_dispmanx.h
h2pas -i pp_temp/vc_dispmanx.h -o interface/vmcs_host/vc_dispmanx.inc

# h2pas -i pp_temp/vc_dispmanx.h   -o interface/vmcs_host/vc_dispmanx.inc
# h2paspp -dVCHPRE_ -dVCHPOST_ /opt/vc/include/interface/vmcs_host/vc_dispmanx.h -opp_temp/vc_dispmanx.h
# h2pas -i pp_temp/vc_dispmanx.h   -o interface/vmcs_host/vc_dispmanx.inc

exit

h2pas -i /opt/vc/include/interface/vmcs_host/vc_dispmanx.h   -o interface/vmcs_host/vc_dispmanx.inc
h2pas -i /opt/vc/include/interface/vmcs_host/vc_tvservice.h  -o interface/vmcs_host/vc_tvservice.h.inc
h2pas -i /opt/vc/include/interface/vmcs_host/vc_cec.h        -o interface/vmcs_host/vc_cec.h
h2pas -i /opt/vc/include/interface/vmcs_host/vc_cecservice.h -o interface/vmcs_host/vc_cecservice.h
h2pas -i /opt/vc/include/interface/vmcs_host/vcgencmd.h      -o interface/vmcs_host/vcgencmd.h
