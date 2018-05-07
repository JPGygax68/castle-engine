
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
(* error 
VCHPRE_ int VCHPOST_ vc_dispman_init( void );
in declaration at line 43 *)
    { Stop the service from being used }
(* error 
VCHPRE_ void VCHPOST_ vc_dispmanx_stop( void );
in declaration at line 45 *)
    { Set the entries in the rect structure }
(* error 
VCHPRE_ int VCHPOST_ vc_dispmanx_rect_set( VC_RECT_T *rect, uint32_t x_offset, uint32_t y_offset, uint32_t width, uint32_t height );
in declaration at line 47 *)
    { Resources }
    { Create a new resource }
(* error 
VCHPRE_ DISPMANX_RESOURCE_HANDLE_T VCHPOST_ vc_dispmanx_resource_create( VC_IMAGE_TYPE_T type, uint32_t width, uint32_t height, uint32_t *native_image_handle );
(* error 
VCHPRE_ DISPMANX_RESOURCE_HANDLE_T VCHPOST_ vc_dispmanx_resource_create( VC_IMAGE_TYPE_T type, uint32_t width, uint32_t height, uint32_t *native_image_handle );
(* error 
VCHPRE_ DISPMANX_RESOURCE_HANDLE_T VCHPOST_ vc_dispmanx_resource_create( VC_IMAGE_TYPE_T type, uint32_t width, uint32_t height, uint32_t *native_image_handle );
(* error 
VCHPRE_ DISPMANX_RESOURCE_HANDLE_T VCHPOST_ vc_dispmanx_resource_create( VC_IMAGE_TYPE_T type, uint32_t width, uint32_t height, uint32_t *native_image_handle );
 in declarator_list *)
 in declarator_list *)
 in declarator_list *)
 in declarator_list *)
    { Write the bitmap data to VideoCore memory }
(* error 
VCHPRE_ int VCHPOST_ vc_dispmanx_resource_write_data( DISPMANX_RESOURCE_HANDLE_T res, VC_IMAGE_TYPE_T src_type, int src_pitch, void * src_address, const VC_RECT_T * rect );
in declaration at line 52 *)
(* error 
VCHPRE_ int VCHPOST_ vc_dispmanx_resource_write_data_handle( DISPMANX_RESOURCE_HANDLE_T res, VC_IMAGE_TYPE_T src_type, int src_pitch, VCHI_MEM_HANDLE_T handle, uint32_t offset, const VC_RECT_T * rect );
in declaration at line 53 *)
(* error 
VCHPRE_ int VCHPOST_ vc_dispmanx_resource_read_data(
in declaration at line 58 *)
    { Delete a resource }
(* error 
VCHPRE_ int VCHPOST_ vc_dispmanx_resource_delete( DISPMANX_RESOURCE_HANDLE_T res );
in declaration at line 60 *)
    { Displays }
    { Opens a display on the given device }
(* error 
VCHPRE_ DISPMANX_DISPLAY_HANDLE_T VCHPOST_ vc_dispmanx_display_open( uint32_t device );
 in declarator_list *)
    { Opens a display on the given device in the request mode }
(* error 
VCHPRE_ DISPMANX_DISPLAY_HANDLE_T VCHPOST_ vc_dispmanx_display_open_mode( uint32_t device, uint32_t mode );
(* error 
VCHPRE_ DISPMANX_DISPLAY_HANDLE_T VCHPOST_ vc_dispmanx_display_open_mode( uint32_t device, uint32_t mode );
 in declarator_list *)
 in declarator_list *)
    { Open an offscreen display }
(* error 
VCHPRE_ DISPMANX_DISPLAY_HANDLE_T VCHPOST_ vc_dispmanx_display_open_offscreen( DISPMANX_RESOURCE_HANDLE_T dest, DISPMANX_TRANSFORM_T orientation );
(* error 
VCHPRE_ DISPMANX_DISPLAY_HANDLE_T VCHPOST_ vc_dispmanx_display_open_offscreen( DISPMANX_RESOURCE_HANDLE_T dest, DISPMANX_TRANSFORM_T orientation );
 in declarator_list *)
 in declarator_list *)
    { Change the mode of a display }
(* error 
VCHPRE_ int VCHPOST_ vc_dispmanx_display_reconfigure( DISPMANX_DISPLAY_HANDLE_T display, uint32_t mode );
in declaration at line 70 *)
    { Sets the desstination of the display to be the given resource }
(* error 
VCHPRE_ int VCHPOST_ vc_dispmanx_display_set_destination( DISPMANX_DISPLAY_HANDLE_T display, DISPMANX_RESOURCE_HANDLE_T dest );
in declaration at line 72 *)
    { Set the background colour of the display }
(* error 
VCHPRE_ int VCHPOST_ vc_dispmanx_display_set_background( DISPMANX_UPDATE_HANDLE_T update, DISPMANX_DISPLAY_HANDLE_T display,
in declaration at line 75 *)
    { get the width, height, frame rate and aspect ratio of the display }
(* error 
VCHPRE_ int VCHPOST_ vc_dispmanx_display_get_info( DISPMANX_DISPLAY_HANDLE_T display, DISPMANX_MODEINFO_T * pinfo );
in declaration at line 77 *)
    { Closes a display }
(* error 
VCHPRE_ int VCHPOST_ vc_dispmanx_display_close( DISPMANX_DISPLAY_HANDLE_T display );
in declaration at line 79 *)
    { Updates }
    { Start a new update, DISPMANX_NO_HANDLE on error }
(* error 
VCHPRE_ DISPMANX_UPDATE_HANDLE_T VCHPOST_ vc_dispmanx_update_start( int32_t priority );
 in declarator_list *)
    { Add an elment to a display as part of an update }
(* error 
VCHPRE_ DISPMANX_ELEMENT_HANDLE_T VCHPOST_ vc_dispmanx_element_add ( DISPMANX_UPDATE_HANDLE_T update, DISPMANX_DISPLAY_HANDLE_T display,
(* error 
VCHPRE_ DISPMANX_ELEMENT_HANDLE_T VCHPOST_ vc_dispmanx_element_add ( DISPMANX_UPDATE_HANDLE_T update, DISPMANX_DISPLAY_HANDLE_T display,
(* error 
                                                                     int32_t layer, const VC_RECT_T *dest_rect, DISPMANX_RESOURCE_HANDLE_T src,
(* error 
                                                                     int32_t layer, const VC_RECT_T *dest_rect, DISPMANX_RESOURCE_HANDLE_T src,
(* error 
                                                                     int32_t layer, const VC_RECT_T *dest_rect, DISPMANX_RESOURCE_HANDLE_T src,
(* error 
                                                                     const VC_RECT_T *src_rect, DISPMANX_PROTECTION_T protection, 
(* error 
                                                                     const VC_RECT_T *src_rect, DISPMANX_PROTECTION_T protection, 
(* error 
                                                                     VC_DISPMANX_ALPHA_T *alpha,
(* error 
                                                                     DISPMANX_CLAMP_T *clamp, DISPMANX_TRANSFORM_T transform );
(* error 
                                                                     DISPMANX_CLAMP_T *clamp, DISPMANX_TRANSFORM_T transform );
 in declarator_list *)
 in declarator_list *)
 in declarator_list *)
 in declarator_list *)
 in declarator_list *)
 in declarator_list *)
 in declarator_list *)
 in declarator_list *)
 in declarator_list *)
 in declarator_list *)
    { Change the source image of a display element }
(* error 
VCHPRE_ int VCHPOST_ vc_dispmanx_element_change_source( DISPMANX_UPDATE_HANDLE_T update, DISPMANX_ELEMENT_HANDLE_T element,
in declaration at line 92 *)
    { Change the layer number of a display element }
(* error 
VCHPRE_ int VCHPOST_ vc_dispmanx_element_change_layer ( DISPMANX_UPDATE_HANDLE_T update, DISPMANX_ELEMENT_HANDLE_T element,
in declaration at line 95 *)
    { Signal that a region of the bitmap has been modified }
(* error 
VCHPRE_ int VCHPOST_ vc_dispmanx_element_modified( DISPMANX_UPDATE_HANDLE_T update, DISPMANX_ELEMENT_HANDLE_T element, const VC_RECT_T * rect );
in declaration at line 97 *)
    { Remove a display element from its display }
(* error 
VCHPRE_ int VCHPOST_ vc_dispmanx_element_remove( DISPMANX_UPDATE_HANDLE_T update, DISPMANX_ELEMENT_HANDLE_T element );
in declaration at line 99 *)
    { Ends an update }
(* error 
VCHPRE_ int VCHPOST_ vc_dispmanx_update_submit( DISPMANX_UPDATE_HANDLE_T update, DISPMANX_CALLBACK_FUNC_T cb_func, void *cb_arg );
in declaration at line 101 *)
    { End an update and wait for it to complete }
(* error 
VCHPRE_ int VCHPOST_ vc_dispmanx_update_submit_sync( DISPMANX_UPDATE_HANDLE_T update );
in declaration at line 103 *)
    { Query the image formats supported in the VMCS build }
(* error 
VCHPRE_ int VCHPOST_ vc_dispmanx_query_image_formats( uint32_t *supported_formats );
in declaration at line 105 *)
    {New function added to VCHI to change attributes, set_opacity does not work there. }
(* error 
VCHPRE_ int VCHPOST_ vc_dispmanx_element_change_attributes( DISPMANX_UPDATE_HANDLE_T update, 
in declaration at line 116 *)
    {xxx hack to get the image pointer from a resource handle, will be obsolete real soon }
(* error 
VCHPRE_ uint32_t VCHPOST_ vc_dispmanx_resource_get_image_handle( DISPMANX_RESOURCE_HANDLE_T res);
 in declarator_list *)
    {Call this instead of vc_dispman_init }
(* error 
VCHPRE_ void VCHPOST_ vc_vchi_dispmanx_init (VCHI_INSTANCE_T initialise_instance, VCHI_CONNECTION_T **connections, uint32_t num_connections );
in declaration at line 122 *)
    { Take a snapshot of a display in its current state. }
    { This call may block for a time; when it completes, the snapshot is ready. }
    { only transform=0 is supported }
(* error 
VCHPRE_ int VCHPOST_ vc_dispmanx_snapshot( DISPMANX_DISPLAY_HANDLE_T display, 
in declaration at line 129 *)
    { Set the resource palette (for VC_IMAGE_4BPP and VC_IMAGE_8BPP) }
(* error 
VCHPRE_ int VCHPOST_ vc_dispmanx_resource_set_palette( DISPMANX_RESOURCE_HANDLE_T handle, 
in declaration at line 133 *)
    { Start triggering callbacks synced to vsync }
(* error 
VCHPRE_ int VCHPOST_ vc_dispmanx_vsync_callback( DISPMANX_DISPLAY_HANDLE_T display, DISPMANX_CALLBACK_FUNC_T cb_func, void *cb_arg );
in declaration at line 136 *)
{ C++ end of extern C conditionnal removed }
{$endif}
    { _VC_DISPMANX_H_ }

