
  Type
  PDISPMANX_CLAMP_T  = ^DISPMANX_CLAMP_T;
  PDISPMANX_MODEINFO_T  = ^DISPMANX_MODEINFO_T;
  Puint32_t  = ^uint32_t;
  PVC_DISPMANX_ALPHA_T  = ^VC_DISPMANX_ALPHA_T;
  PVC_RECT_T  = ^VC_RECT_T;
  PVCHI_CONNECTION_T  = ^VCHI_CONNECTION_T;
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
  { Display manager service API }
{$ifndef _VC_DISPMANX_H_}
{$define _VC_DISPMANX_H_}  
{$include "interface/vcos/vcos.h"}
{$include "interface/vctypes/vc_image_types.h"}
{$include "vc_dispservice_x_defs.h"}
{$include "interface/vmcs_host/vc_dispmanx_types.h"}
{$include "interface/vchi/vchi.h"}
{ C++ extern C conditionnal removed }
  { Same function as above, to aid migration of code. }

  function vc_dispman_init:longint;

  { Stop the service from being used }
  procedure vc_dispmanx_stop;

  { Set the entries in the rect structure }
  function vc_dispmanx_rect_set(rect:PVC_RECT_T; x_offset:uint32_t; y_offset:uint32_t; width:uint32_t; height:uint32_t):longint;

  { Resources }
  { Create a new resource }
  function vc_dispmanx_resource_create(_type:VC_IMAGE_TYPE_T; width:uint32_t; height:uint32_t; native_image_handle:Puint32_t):DISPMANX_RESOURCE_HANDLE_T;

  { Write the bitmap data to VideoCore memory }
(* Const before type ignored *)
  function vc_dispmanx_resource_write_data(res:DISPMANX_RESOURCE_HANDLE_T; src_type:VC_IMAGE_TYPE_T; src_pitch:longint; src_address:pointer; rect:PVC_RECT_T):longint;

(* Const before type ignored *)
  function vc_dispmanx_resource_write_data_handle(res:DISPMANX_RESOURCE_HANDLE_T; src_type:VC_IMAGE_TYPE_T; src_pitch:longint; handle:VCHI_MEM_HANDLE_T; offset:uint32_t; 
             rect:PVC_RECT_T):longint;

(* Const before type ignored *)
  function vc_dispmanx_resource_read_data(handle:DISPMANX_RESOURCE_HANDLE_T; p_rect:PVC_RECT_T; dst_address:pointer; dst_pitch:uint32_t):longint;

  { Delete a resource }
  function vc_dispmanx_resource_delete(res:DISPMANX_RESOURCE_HANDLE_T):longint;

  { Displays }
  { Opens a display on the given device }
  function vc_dispmanx_display_open(device:uint32_t):DISPMANX_DISPLAY_HANDLE_T;

  { Opens a display on the given device in the request mode }
  function vc_dispmanx_display_open_mode(device:uint32_t; mode:uint32_t):DISPMANX_DISPLAY_HANDLE_T;

  { Open an offscreen display }
  function vc_dispmanx_display_open_offscreen(dest:DISPMANX_RESOURCE_HANDLE_T; orientation:DISPMANX_TRANSFORM_T):DISPMANX_DISPLAY_HANDLE_T;

  { Change the mode of a display }
  function vc_dispmanx_display_reconfigure(display:DISPMANX_DISPLAY_HANDLE_T; mode:uint32_t):longint;

  { Sets the desstination of the display to be the given resource }
  function vc_dispmanx_display_set_destination(display:DISPMANX_DISPLAY_HANDLE_T; dest:DISPMANX_RESOURCE_HANDLE_T):longint;

  { Set the background colour of the display }
  function vc_dispmanx_display_set_background(update:DISPMANX_UPDATE_HANDLE_T; display:DISPMANX_DISPLAY_HANDLE_T; red:uint8_t; green:uint8_t; blue:uint8_t):longint;

  { get the width, height, frame rate and aspect ratio of the display }
  function vc_dispmanx_display_get_info(display:DISPMANX_DISPLAY_HANDLE_T; pinfo:PDISPMANX_MODEINFO_T):longint;

  { Closes a display }
  function vc_dispmanx_display_close(display:DISPMANX_DISPLAY_HANDLE_T):longint;

  { Updates }
  { Start a new update, DISPMANX_NO_HANDLE on error }
  function vc_dispmanx_update_start(priority:int32_t):DISPMANX_UPDATE_HANDLE_T;

  { Add an elment to a display as part of an update }
(* Const before type ignored *)
(* Const before type ignored *)
  function vc_dispmanx_element_add(update:DISPMANX_UPDATE_HANDLE_T; display:DISPMANX_DISPLAY_HANDLE_T; layer:int32_t; dest_rect:PVC_RECT_T; src:DISPMANX_RESOURCE_HANDLE_T; 
             src_rect:PVC_RECT_T; protection:DISPMANX_PROTECTION_T; alpha:PVC_DISPMANX_ALPHA_T; clamp:PDISPMANX_CLAMP_T; transform:DISPMANX_TRANSFORM_T):DISPMANX_ELEMENT_HANDLE_T;

  { Change the source image of a display element }
  function vc_dispmanx_element_change_source(update:DISPMANX_UPDATE_HANDLE_T; element:DISPMANX_ELEMENT_HANDLE_T; src:DISPMANX_RESOURCE_HANDLE_T):longint;

  { Change the layer number of a display element }
  function vc_dispmanx_element_change_layer(update:DISPMANX_UPDATE_HANDLE_T; element:DISPMANX_ELEMENT_HANDLE_T; layer:int32_t):longint;

  { Signal that a region of the bitmap has been modified }
(* Const before type ignored *)
  function vc_dispmanx_element_modified(update:DISPMANX_UPDATE_HANDLE_T; element:DISPMANX_ELEMENT_HANDLE_T; rect:PVC_RECT_T):longint;

  { Remove a display element from its display }
  function vc_dispmanx_element_remove(update:DISPMANX_UPDATE_HANDLE_T; element:DISPMANX_ELEMENT_HANDLE_T):longint;

  { Ends an update }
  function vc_dispmanx_update_submit(update:DISPMANX_UPDATE_HANDLE_T; cb_func:DISPMANX_CALLBACK_FUNC_T; cb_arg:pointer):longint;

  { End an update and wait for it to complete }
  function vc_dispmanx_update_submit_sync(update:DISPMANX_UPDATE_HANDLE_T):longint;

  { Query the image formats supported in the VMCS build }
  function vc_dispmanx_query_image_formats(supported_formats:Puint32_t):longint;

  {New function added to VCHI to change attributes, set_opacity does not work there. }
(* Const before type ignored *)
(* Const before type ignored *)
  function vc_dispmanx_element_change_attributes(update:DISPMANX_UPDATE_HANDLE_T; element:DISPMANX_ELEMENT_HANDLE_T; change_flags:uint32_t; layer:int32_t; opacity:uint8_t; 
             dest_rect:PVC_RECT_T; src_rect:PVC_RECT_T; mask:DISPMANX_RESOURCE_HANDLE_T; transform:DISPMANX_TRANSFORM_T):longint;

  {xxx hack to get the image pointer from a resource handle, will be obsolete real soon }
  function vc_dispmanx_resource_get_image_handle(res:DISPMANX_RESOURCE_HANDLE_T):uint32_t;

  {Call this instead of vc_dispman_init }
  procedure vc_vchi_dispmanx_init(initialise_instance:VCHI_INSTANCE_T; connections:PPVCHI_CONNECTION_T; num_connections:uint32_t);

  { Take a snapshot of a display in its current state. }
  { This call may block for a time; when it completes, the snapshot is ready. }
  { only transform=0 is supported }
  function vc_dispmanx_snapshot(display:DISPMANX_DISPLAY_HANDLE_T; snapshot_resource:DISPMANX_RESOURCE_HANDLE_T; transform:DISPMANX_TRANSFORM_T):longint;

  { Set the resource palette (for VC_IMAGE_4BPP and VC_IMAGE_8BPP) }
  function vc_dispmanx_resource_set_palette(handle:DISPMANX_RESOURCE_HANDLE_T; src_address:pointer; offset:longint; size:longint):longint;

  { Start triggering callbacks synced to vsync }
  function vc_dispmanx_vsync_callback(display:DISPMANX_DISPLAY_HANDLE_T; cb_func:DISPMANX_CALLBACK_FUNC_T; cb_arg:pointer):longint;

{ C++ end of extern C conditionnal removed }
{$endif}
  { _VC_DISPMANX_H_ }
  function vc_dispman_init:longint;
  begin
    { You must implement this function }
  end;
  procedure vc_dispmanx_stop;
  begin
    { You must implement this function }
  end;
  function vc_dispmanx_rect_set(rect:PVC_RECT_T; x_offset:uint32_t; y_offset:uint32_t; width:uint32_t; height:uint32_t):longint;
  begin
    { You must implement this function }
  end;
  function vc_dispmanx_resource_create(_type:VC_IMAGE_TYPE_T; width:uint32_t; height:uint32_t; native_image_handle:Puint32_t):DISPMANX_RESOURCE_HANDLE_T;
  begin
    { You must implement this function }
  end;
  function vc_dispmanx_resource_write_data(res:DISPMANX_RESOURCE_HANDLE_T; src_type:VC_IMAGE_TYPE_T; src_pitch:longint; src_address:pointer; rect:PVC_RECT_T):longint;
  begin
    { You must implement this function }
  end;
  function vc_dispmanx_resource_write_data_handle(res:DISPMANX_RESOURCE_HANDLE_T; src_type:VC_IMAGE_TYPE_T; src_pitch:longint; handle:VCHI_MEM_HANDLE_T; offset:uint32_t; 
             rect:PVC_RECT_T):longint;
  begin
    { You must implement this function }
  end;
  function vc_dispmanx_resource_read_data(handle:DISPMANX_RESOURCE_HANDLE_T; p_rect:PVC_RECT_T; dst_address:pointer; dst_pitch:uint32_t):longint;
  begin
    { You must implement this function }
  end;
  function vc_dispmanx_resource_delete(res:DISPMANX_RESOURCE_HANDLE_T):longint;
  begin
    { You must implement this function }
  end;
  function vc_dispmanx_display_open(device:uint32_t):DISPMANX_DISPLAY_HANDLE_T;
  begin
    { You must implement this function }
  end;
  function vc_dispmanx_display_open_mode(device:uint32_t; mode:uint32_t):DISPMANX_DISPLAY_HANDLE_T;
  begin
    { You must implement this function }
  end;
  function vc_dispmanx_display_open_offscreen(dest:DISPMANX_RESOURCE_HANDLE_T; orientation:DISPMANX_TRANSFORM_T):DISPMANX_DISPLAY_HANDLE_T;
  begin
    { You must implement this function }
  end;
  function vc_dispmanx_display_reconfigure(display:DISPMANX_DISPLAY_HANDLE_T; mode:uint32_t):longint;
  begin
    { You must implement this function }
  end;
  function vc_dispmanx_display_set_destination(display:DISPMANX_DISPLAY_HANDLE_T; dest:DISPMANX_RESOURCE_HANDLE_T):longint;
  begin
    { You must implement this function }
  end;
  function vc_dispmanx_display_set_background(update:DISPMANX_UPDATE_HANDLE_T; display:DISPMANX_DISPLAY_HANDLE_T; red:uint8_t; green:uint8_t; blue:uint8_t):longint;
  begin
    { You must implement this function }
  end;
  function vc_dispmanx_display_get_info(display:DISPMANX_DISPLAY_HANDLE_T; pinfo:PDISPMANX_MODEINFO_T):longint;
  begin
    { You must implement this function }
  end;
  function vc_dispmanx_display_close(display:DISPMANX_DISPLAY_HANDLE_T):longint;
  begin
    { You must implement this function }
  end;
  function vc_dispmanx_update_start(priority:int32_t):DISPMANX_UPDATE_HANDLE_T;
  begin
    { You must implement this function }
  end;
  function vc_dispmanx_element_add(update:DISPMANX_UPDATE_HANDLE_T; display:DISPMANX_DISPLAY_HANDLE_T; layer:int32_t; dest_rect:PVC_RECT_T; src:DISPMANX_RESOURCE_HANDLE_T; 
             src_rect:PVC_RECT_T; protection:DISPMANX_PROTECTION_T; alpha:PVC_DISPMANX_ALPHA_T; clamp:PDISPMANX_CLAMP_T; transform:DISPMANX_TRANSFORM_T):DISPMANX_ELEMENT_HANDLE_T;
  begin
    { You must implement this function }
  end;
  function vc_dispmanx_element_change_source(update:DISPMANX_UPDATE_HANDLE_T; element:DISPMANX_ELEMENT_HANDLE_T; src:DISPMANX_RESOURCE_HANDLE_T):longint;
  begin
    { You must implement this function }
  end;
  function vc_dispmanx_element_change_layer(update:DISPMANX_UPDATE_HANDLE_T; element:DISPMANX_ELEMENT_HANDLE_T; layer:int32_t):longint;
  begin
    { You must implement this function }
  end;
  function vc_dispmanx_element_modified(update:DISPMANX_UPDATE_HANDLE_T; element:DISPMANX_ELEMENT_HANDLE_T; rect:PVC_RECT_T):longint;
  begin
    { You must implement this function }
  end;
  function vc_dispmanx_element_remove(update:DISPMANX_UPDATE_HANDLE_T; element:DISPMANX_ELEMENT_HANDLE_T):longint;
  begin
    { You must implement this function }
  end;
  function vc_dispmanx_update_submit(update:DISPMANX_UPDATE_HANDLE_T; cb_func:DISPMANX_CALLBACK_FUNC_T; cb_arg:pointer):longint;
  begin
    { You must implement this function }
  end;
  function vc_dispmanx_update_submit_sync(update:DISPMANX_UPDATE_HANDLE_T):longint;
  begin
    { You must implement this function }
  end;
  function vc_dispmanx_query_image_formats(supported_formats:Puint32_t):longint;
  begin
    { You must implement this function }
  end;
  function vc_dispmanx_element_change_attributes(update:DISPMANX_UPDATE_HANDLE_T; element:DISPMANX_ELEMENT_HANDLE_T; change_flags:uint32_t; layer:int32_t; opacity:uint8_t; 
             dest_rect:PVC_RECT_T; src_rect:PVC_RECT_T; mask:DISPMANX_RESOURCE_HANDLE_T; transform:DISPMANX_TRANSFORM_T):longint;
  begin
    { You must implement this function }
  end;
  function vc_dispmanx_resource_get_image_handle(res:DISPMANX_RESOURCE_HANDLE_T):uint32_t;
  begin
    { You must implement this function }
  end;
  procedure vc_vchi_dispmanx_init(initialise_instance:VCHI_INSTANCE_T; connections:PPVCHI_CONNECTION_T; num_connections:uint32_t);
  begin
    { You must implement this function }
  end;
  function vc_dispmanx_snapshot(display:DISPMANX_DISPLAY_HANDLE_T; snapshot_resource:DISPMANX_RESOURCE_HANDLE_T; transform:DISPMANX_TRANSFORM_T):longint;
  begin
    { You must implement this function }
  end;
  function vc_dispmanx_resource_set_palette(handle:DISPMANX_RESOURCE_HANDLE_T; src_address:pointer; offset:longint; size:longint):longint;
  begin
    { You must implement this function }
  end;
  function vc_dispmanx_vsync_callback(display:DISPMANX_DISPLAY_HANDLE_T; cb_func:DISPMANX_CALLBACK_FUNC_T; cb_arg:pointer):longint;
  begin
    { You must implement this function }
  end;

