
unit bcm_host;
interface

{
  Automatically converted by H2Pas 1.0.0 from /opt/vc/include/bcm_host.h
  The following command line parameters were used:
    -d
    /opt/vc/include/bcm_host.h
    -o
    bcm_host.pas
}

  Type
  Puint32_t  = ^uint32_t;
{$IFDEF FPC}
{$PACKRECORDS C}
{$ENDIF}


  {
  Copyright (c) 2012, Broadcom Europe Ltd
  All rights reserved.
  
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
      * Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
      * Neither the name of the copyright holder nor the
        names of its contributors may be used to endorse or promote products
        derived from this software without specific prior written permission.
  
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   }
  { Header file with useful bits from other headers }
{$ifndef BCM_HOST_H}
{$define BCM_HOST_H}  
{$include <stdint.h>}
{ C++ extern C conditionnal removed }

  procedure bcm_host_init;cdecl;external;

  procedure bcm_host_deinit;cdecl;external;

(* Const before type ignored *)
  function graphics_get_display_size(display_number:uint16_t; width:Puint32_t; height:Puint32_t):int32_t;cdecl;external;

  function bcm_host_get_peripheral_address:dword;cdecl;external;

  function bcm_host_get_peripheral_size:dword;cdecl;external;

  function bcm_host_get_sdram_address:dword;cdecl;external;

{$include "interface/vmcs_host/vc_dispmanx.h"}
{$include "interface/vmcs_host/vc_tvservice.h"}
{$include "interface/vmcs_host/vc_cec.h"}
{$include "interface/vmcs_host/vc_cecservice.h"}
{$include "interface/vmcs_host/vcgencmd.h"}
{ C++ end of extern C conditionnal removed }
{$endif}

implementation


end.
