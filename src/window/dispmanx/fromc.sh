#!/bin/bash

h2pas -d /opt/vc/include/bcm_host.h -o bcm_host.pas

# We need a temporary directory for "pre-processing"
mkdir -p pp_temp
rm -f pp_temp/*

convert_header () {

    #cp /opt/vc/include/interface/vmcs_host/vc_dispmanx.h pp_temp/vc_dispmanx.h_1
    #sed -r 's/VCHPRE_//; s/VCHPOST_//' pp_temp/vc_dispmanx.h_1 >pp_temp/vc_dispmanx.h_2
    # cp pp_temp/vc_dispmanx.h_2 pp_temp/vc_dispmanx.h

    mkdir -p pp_temp/$(dirname "$1")

    sed -r 's/VCHPRE_//; s/VCHPOST_//' /opt/vc/include/$1 >pp_temp/$1

    h2pas -i pp_temp/$1 -o $(echo "$1" | cut -f 1 -d '.').$2 #interface/vmcs_host/vc_dispmanx.inc
}

convert_header interface/vmcs_host/vc_dispmanx.h inc

# h2pas -i pp_temp/vc_dispmanx.h   -o interface/vmcs_host/vc_dispmanx.inc
# h2paspp -dVCHPRE_ -dVCHPOST_ /opt/vc/include/interface/vmcs_host/vc_dispmanx.h -opp_temp/vc_dispmanx.h
# h2pas -i pp_temp/vc_dispmanx.h   -o interface/vmcs_host/vc_dispmanx.inc

exit

h2pas -i /opt/vc/include/interface/vmcs_host/vc_dispmanx.h   -o interface/vmcs_host/vc_dispmanx.inc
h2pas -i /opt/vc/include/interface/vmcs_host/vc_tvservice.h  -o interface/vmcs_host/vc_tvservice.h.inc
h2pas -i /opt/vc/include/interface/vmcs_host/vc_cec.h        -o interface/vmcs_host/vc_cec.h
h2pas -i /opt/vc/include/interface/vmcs_host/vc_cecservice.h -o interface/vmcs_host/vc_cecservice.h
h2pas -i /opt/vc/include/interface/vmcs_host/vcgencmd.h      -o interface/vmcs_host/vcgencmd.h
