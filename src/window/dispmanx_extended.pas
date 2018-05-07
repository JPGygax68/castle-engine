{DispmanX Headers:
 
  Ported to FreePascal by Garry Wood <garry@softoz.com.au>
 
 Original Copyright:
 
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

unit DispmanX;

{$mode objfpc} {Default to ObjFPC compatible syntax}
{$H+}          {Default to AnsiString}
{$inline on}   {Allow use of Inline procedures}
 
interface
 
uses {$ifdef ultibo}GlobalTypes,{$endif}SysUtils{$ifdef ultibo},Syscalls{$endif}; //,VC4;
 
{$linklib 'bcm_host'}

{$PACKRECORDS C}

const
 libvchostif = 'vchostif';

{$linklib libvchostif}

{$ifndef ultibo}
type
 int32_t = Longint;
 
 uint8_t = Byte;
 uint32_t = LongWord;
 Puint32_t = ^uint32_t;
{$endif}

{From interface\vmcs_host\vc_dispmanx_types.h}
const
 VC_DISPMANX_VERSION = 1;    

 { Opaque handles }
type
 PDISPMANX_DISPLAY_HANDLE_T = ^DISPMANX_DISPLAY_HANDLE_T;
 DISPMANX_DISPLAY_HANDLE_T = uint32_t;

 PDISPMANX_UPDATE_HANDLE_T = ^DISPMANX_UPDATE_HANDLE_T;
 DISPMANX_UPDATE_HANDLE_T = uint32_t;

 PDISPMANX_ELEMENT_HANDLE_T = ^DISPMANX_ELEMENT_HANDLE_T;
 DISPMANX_ELEMENT_HANDLE_T = uint32_t;

 PDISPMANX_RESOURCE_HANDLE_T = ^DISPMANX_RESOURCE_HANDLE_T;
 DISPMANX_RESOURCE_HANDLE_T = uint32_t;

 PDISPMANX_PROTECTION_T = ^DISPMANX_PROTECTION_T;
 DISPMANX_PROTECTION_T = uint32_t;

const
 DISPMANX_NO_HANDLE = 0;   
 
 DISPMANX_PROTECTION_MAX = $0f;    
 DISPMANX_PROTECTION_NONE = 0;    
 DISPMANX_PROTECTION_HDCP = 11;    { Derived from the WM DRM levels, 101-300 }
 
 
 { Default display IDs.
   Note: if you overwrite with your own dispmanx_platform_init function, you
   should use IDs you provided during dispmanx_display_attach. }
 DISPMANX_ID_MAIN_LCD = 0;    
 DISPMANX_ID_AUX_LCD = 1;    
 DISPMANX_ID_HDMI = 2;    
 DISPMANX_ID_SDTV = 3;    
 DISPMANX_ID_FORCE_LCD = 4;    
 DISPMANX_ID_FORCE_TV = 5;    
 DISPMANX_ID_FORCE_OTHER = 6;  { non-default display  }
 
type
 PDISPMANX_STATUS_T = ^DISPMANX_STATUS_T;
 DISPMANX_STATUS_T = int32_t; 
const
 { Return codes. Nonzero ones indicate failure. }
 DISPMANX_SUCCESS = 0;
 DISPMANX_INVALID = -1;
 { XXX others TBA  }
 
type
 PDISPMANX_TRANSFORM_T = ^DISPMANX_TRANSFORM_T;
 DISPMANX_TRANSFORM_T = uint32_t;
const
 { Bottom 2 bits sets the orientation  }
 DISPMANX_NO_ROTATE = 0;
 DISPMANX_ROTATE_90 = 1;
 DISPMANX_ROTATE_180 = 2;
 DISPMANX_ROTATE_270 = 3;
 
 DISPMANX_FLIP_HRIZ = 1 shl 16;
 DISPMANX_FLIP_VERT = 1 shl 17;
 
 { invert left/right images  }
 DISPMANX_STEREOSCOPIC_INVERT = 1 shl 19;
 
 { extra flags for controlling 3d duplication behaviour  }
 DISPMANX_STEREOSCOPIC_NONE = 0 shl 20;
 DISPMANX_STEREOSCOPIC_MONO = 1 shl 20;
 DISPMANX_STEREOSCOPIC_SBS = 2 shl 20;
 DISPMANX_STEREOSCOPIC_TB = 3 shl 20;
 DISPMANX_STEREOSCOPIC_MASK = 15 shl 20;
 
 { extra flags for controlling snapshot behaviour  }
 DISPMANX_SNAPSHOT_NO_YUV = 1 shl 24;
 DISPMANX_SNAPSHOT_NO_RGB = 1 shl 25;
 DISPMANX_SNAPSHOT_FILL = 1 shl 26;
 DISPMANX_SNAPSHOT_SWAP_RED_BLUE = 1 shl 27;
 DISPMANX_SNAPSHOT_PACK = 1 shl 28;

type
 PDISPMANX_FLAGS_ALPHA_T = ^DISPMANX_FLAGS_ALPHA_T;
 DISPMANX_FLAGS_ALPHA_T = uint32_t;
const 
 { Bottom 2 bits sets the alpha mode  }
 DISPMANX_FLAGS_ALPHA_FROM_SOURCE = 0;
 DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS = 1;
 DISPMANX_FLAGS_ALPHA_FIXED_NON_ZERO = 2;
 DISPMANX_FLAGS_ALPHA_FIXED_EXCEED_0X07 = 3;
  
 DISPMANX_FLAGS_ALPHA_PREMULT = 1 shl 16;
 DISPMANX_FLAGS_ALPHA_MIX = 1 shl 17;

{ BEGIN 2018-05-06 gygax@practicomp.ch: copied image-related types from Ultibo file Core/vc4.pas }

{The image structure.}
type
 Pvc_image_extra_uv_s = ^vc_image_extra_uv_s;
 vc_image_extra_uv_s = record
  u : pointer;
  v : pointer;
  vpitch : longint;
 end;
 VC_IMAGE_EXTRA_UV_T = vc_image_extra_uv_s;
 PVC_IMAGE_EXTRA_UV_T = ^VC_IMAGE_EXTRA_UV_T;
      
 Pvc_image_extra_rgba_s = ^vc_image_extra_rgba_s;
 vc_image_extra_rgba_s = record
  value: LongWord; {component_order   : 24 (diagnostic use only)}
                   {normalised_alpha  : 1}
                   {transparent_colour: 1}
                   {unused_26_31      : 6}
  arg: LongWord;
 end;
 VC_IMAGE_EXTRA_RGBA_T = vc_image_extra_rgba_s;
 PVC_IMAGE_EXTRA_RGBA_T = ^VC_IMAGE_EXTRA_RGBA_T;
 
 Pvc_image_extra_pal_s = ^vc_image_extra_pal_s;
 vc_image_extra_pal_s = record
  palette: Psmallint;
  palette32: Int32_t;  {palette32 : 1} { gygax@practicomp.ch: changed this from int to Int32_t}
 end;
 VC_IMAGE_EXTRA_PAL_T = vc_image_extra_pal_s;
 PVC_IMAGE_EXTRA_PAL_T = ^VC_IMAGE_EXTRA_PAL_T;
 
 {These fields are subject to change / being moved around}
 Pvc_image_extra_tf_s = ^vc_image_extra_tf_s;
 vc_image_extra_tf_s = record
  value: LongWord; {mipmap_levels  : 8}
                   {xxx            : 23}
                   {cube_map       : 1}
  palette: Pointer;
 end;
 VC_IMAGE_EXTRA_TF_T = vc_image_extra_tf_s;
 PVC_IMAGE_EXTRA_TF_T = ^VC_IMAGE_EXTRA_TF_T;

 Pvc_image_extra_bayer_s = ^vc_image_extra_bayer_s;
 vc_image_extra_bayer_s = record
  order : word;
  format : word;
  block_length : longint;
 end;
 VC_IMAGE_EXTRA_BAYER_T = vc_image_extra_bayer_s;
 PVC_IMAGE_EXTRA_BAYER_T = ^VC_IMAGE_EXTRA_BAYER_T;
      
 {The next block can be used with Visual C++ which treats enums as long ints}
 Pvc_image_extra_msbayer_s = ^vc_image_extra_msbayer_s;
 vc_image_extra_msbayer_s = record
  order : byte;
  format : byte;
  dummy1 : byte;
  dummy2 : byte;
  block_length : longint;
 end;
 VC_IMAGE_EXTRA_MSBAYER_T = vc_image_extra_msbayer_s;
 PVC_IMAGE_EXTRA_MSBAYER_T = ^VC_IMAGE_EXTRA_MSBAYER_T;
      
 {NB this will be copied to image.size in parmalloc()}
 Pvc_image_extra_codec_s = ^vc_image_extra_codec_s;
 vc_image_extra_codec_s = record
  fourcc : longint;
  maxsize : longint;
 end;
 VC_IMAGE_EXTRA_CODEC_T = vc_image_extra_codec_s;
 PVC_IMAGE_EXTRA_CODEC_T = ^VC_IMAGE_EXTRA_CODEC_T;
      
const
 VC_IMAGE_OPENGL_RGBA32 = $14011908;    {GL_UNSIGNED_BYTE GL_RGBA}
 VC_IMAGE_OPENGL_RGB24 = $14011907;    {GL_UNSIGNED_BYTE GL_RGB}
 VC_IMAGE_OPENGL_RGBA16 = $80331908;    {GL_UNSIGNED_SHORT_4_4_4_4 GL_RGBA}
 VC_IMAGE_OPENGL_RGBA5551 = $80341908;    {GL_UNSIGNED_SHORT_5_5_5_1 GL_RGBA}
 VC_IMAGE_OPENGL_RGB565 = $83631907;    {GL_UNSIGNED_SHORT_5_6_5 GL_RGB}
 VC_IMAGE_OPENGL_YA88 = $1401190A;    {GL_UNSIGNED_BYTE GL_LUMINANCE_ALPHA}
 VC_IMAGE_OPENGL_Y8 = $14011909;    {GL_UNSIGNED_BYTE GL_LUMINANCE}
 VC_IMAGE_OPENGL_A8 = $14011906;    {GL_UNSIGNED_BYTE GL_ALPHA}
 VC_IMAGE_OPENGL_ETC1 = $8D64;    {GL_ETC1_RGB8_OES}
 VC_IMAGE_OPENGL_PALETTE4_RGB24 = $8B90;    {GL_PALETTE4_RGB8_OES}
 VC_IMAGE_OPENGL_PALETTE4_RGBA32 = $8B91;    {GL_PALETTE4_RGBA8_OES}
 VC_IMAGE_OPENGL_PALETTE4_RGB565 = $8B92;    {GL_PALETTE4_R5_G6_B5_OES}
 VC_IMAGE_OPENGL_PALETTE4_RGBA16 = $8B93;    {GL_PALETTE4_RGBA4_OES}
 VC_IMAGE_OPENGL_PALETTE4_RGB5551 = $8B94;    {GL_PALETTE4_RGB5_A1_OES}
 VC_IMAGE_OPENGL_PALETTE8_RGB24 = $8B95;    {GL_PALETTE8_RGB8_OES}
 VC_IMAGE_OPENGL_PALETTE8_RGBA32 = $8B96;    {GL_PALETTE8_RGBA8_OES}
 VC_IMAGE_OPENGL_PALETTE8_RGB565 = $8B97;    {GL_PALETTE8_R5_G6_B5_OES}
 VC_IMAGE_OPENGL_PALETTE8_RGBA16 = $8B98;    {GL_PALETTE8_RGBA4_OES}
 VC_IMAGE_OPENGL_PALETTE8_RGB5551 = $8B99;    {GL_PALETTE8_RGB5_A1_OES}

type
 Pvc_image_extra_opengl_s = ^vc_image_extra_opengl_s;
 vc_image_extra_opengl_s = record
  format_and_type : dword;
  palette : pointer;
 end;
 VC_IMAGE_EXTRA_OPENGL_T = vc_image_extra_opengl_s;
 PVC_IMAGE_EXTRA_OPENGL_T = ^VC_IMAGE_EXTRA_OPENGL_T;

 PVC_IMAGE_EXTRA_T = ^VC_IMAGE_EXTRA_T;
 VC_IMAGE_EXTRA_T = record
  case longint of
   0 : ( uv : VC_IMAGE_EXTRA_UV_T );
   1 : ( rgba : VC_IMAGE_EXTRA_RGBA_T );
   2 : ( pal : VC_IMAGE_EXTRA_PAL_T );
   3 : ( tf : VC_IMAGE_EXTRA_TF_T );
   4 : ( bayer : VC_IMAGE_EXTRA_BAYER_T );
   5 : ( msbayer : VC_IMAGE_EXTRA_MSBAYER_T );
   6 : ( codec : VC_IMAGE_EXTRA_CODEC_T );
   7 : ( opengl : VC_IMAGE_EXTRA_OPENGL_T );
 end;

{Structure containing various colour meta-data for each format}
type
 PVC_IMAGE_INFO_T = ^VC_IMAGE_INFO_T;
 VC_IMAGE_INFO_T = record
  case longint of
   1 : ( yuv : word );               {Information pertinent to all YUV implementations}
   2 : ( info : word );              {Dummy, force size to min 16 bits}
 end;
 
 {Image handle object, which must be locked before image data becomes accessible.
  A handle to an image where the image data does not have a guaranteed storage location.  A call to vc_image_lock() must be made to convert
  this into a VC_IMAGE_BUF_T, which guarantees that image data can be accessed safely.
  This type will also be used in cases where it's unclear whether or not the buffer is already locked, and in legacy code}
type
 PVC_IMAGE_T = ^VC_IMAGE_T;
 VC_IMAGE_T = record
  _type : word;                            {Should restrict to 16 bits}
  info : VC_IMAGE_INFO_T;                  {Format-specific info; zero for VC02 behaviour}
  width : word;                            {Width in pixels}
  height : word;                           {Height in pixels}
  pitch : longint;                         {Pitch of image_data array in bytes}
  size : longint;                          {Number of bytes available in image_data array}
  image_data : Pointer;                    {Pixel data}
  extra : VC_IMAGE_EXTRA_T;                {Extra data like palette pointer}
  metadata : Pointer;                      {Metadata header for the image (vc_metadata_header_s)}
  pool_object : Pointer;                   {Non NULL if image was allocated from a vc_pool (opaque_vc_pool_object_s)}
  mem_handle : uint32_t;                   {The mem handle for relocatable memory storage}
  metadata_size : longint;                 {Size of metadata of each channel in bytes}
  channel_offset : longint;                {Offset of consecutive channels in bytes}
  video_timestamp : uint32_t;              {90000 Hz RTP times domain - derived from audio timestamp}
  num_channels : uint8_t;                  {Number of channels (2 for stereo)}
  current_channel : uint8_t;               {The channel this header is currently pointing to}
  linked_multichann_flag : uint8_t;        {Indicate the header has the linked-multichannel structure}
  is_channel_linked : uint8_t;             {Track if the above structure is been used to link the header into a linked-mulitchannel image}
  channel_index : uint8_t;                 {Index of the channel this header represents while it is being linked}
  _dummy : array[0..2] of uint8_t;         {Pad struct to 64 bytes}
end;

{ // END image-related types }

{==============================================================================}
{VC4 VC Display Types (From interface\vctypes\vc_display_types.h)}
{$PACKRECORDS C}
{Enums of display input format }
type
 VCOS_DISPLAY_INPUT_FORMAT_T = (
  VCOS_DISPLAY_INPUT_FORMAT_INVALID = 0,
  VCOS_DISPLAY_INPUT_FORMAT_RGB888,
  VCOS_DISPLAY_INPUT_FORMAT_RGB565
 );

{For backward compatibility}
const
 DISPLAY_INPUT_FORMAT_INVALID = VCOS_DISPLAY_INPUT_FORMAT_INVALID;    
 DISPLAY_INPUT_FORMAT_RGB888 = VCOS_DISPLAY_INPUT_FORMAT_RGB888;    
 DISPLAY_INPUT_FORMAT_RGB565 = VCOS_DISPLAY_INPUT_FORMAT_RGB565;    
  
type
 DISPLAY_INPUT_FORMAT_T = VCOS_DISPLAY_INPUT_FORMAT_T;
 
 {Enum determining how image data for 3D displays has to be supplied}
 DISPLAY_3D_FORMAT_T = (
  DISPLAY_3D_UNSUPPORTED = 0, {Default}
  DISPLAY_3D_INTERLEAVED,      {For autosteroscopic displays}
  DISPLAY_3D_SBS_FULL_AUTO,    {Side-By-Side, Full Width (also used by some autostereoscopic displays)}
  DISPLAY_3D_SBS_HALF_HORIZ,   {Side-By-Side, Half Width, Horizontal Subsampling (see HDMI spec)}
  DISPLAY_3D_TB_HALF,          {Top-bottom 3D}
  DISPLAY_3D_FRAME_PACKING,    {Frame Packed 3D}
  DISPLAY_3D_FRAME_SEQUENTIAL, {Output left on even frames and right on odd frames (typically 120Hz)}
  DISPLAY_3D_FORMAT_MAX
 );
  
 {Enums of display types}
 DISPLAY_INTERFACE_T = (
  DISPLAY_INTERFACE_MIN,
  DISPLAY_INTERFACE_SMI,
  DISPLAY_INTERFACE_DPI,
  DISPLAY_INTERFACE_DSI,
  DISPLAY_INTERFACE_LVDS,
  DISPLAY_INTERFACE_MAX
 );

 {Display dither setting, used on B0}
 DISPLAY_DITHER_T = (
  DISPLAY_DITHER_NONE = 0,   {Default if not set}
  DISPLAY_DITHER_RGB666 = 1,
  DISPLAY_DITHER_RGB565 = 2,
  DISPLAY_DITHER_RGB555 = 3,
  DISPLAY_DITHER_MAX);
  
 {Info struct}
 PDISPLAY_INFO_T = ^DISPLAY_INFO_T;
 DISPLAY_INFO_T = record
  _type : DISPLAY_INTERFACE_T;           {Type}
  width : uint32_t;                      {Width / Height}
  height : uint32_t;
  input_format : DISPLAY_INPUT_FORMAT_T; {Output format}
  interlaced : uint32_t;                 {Interlaced?}
  output_dither : DISPLAY_DITHER_T;      {Output dither setting (if required)}
  pixel_freq : uint32_t;                 {Pixel frequency}
  line_rate : uint32_t;                  {Line rate in lines per second}
  format_3d : DISPLAY_3D_FORMAT_T;       {Format required for image data for 3D displays}
  use_pixelvalve_1 : uint32_t;           {If display requires PV1 (e.g. DSI1), special config is required in HVS}
  dsi_video_mode : uint32_t;             {Set for DSI displays which use video mode}
  hvs_channel : uint32_t;                {Select HVS channel (usually 0)}
 end;
 
{$PACKRECORDS DEFAULT}

{==============================================================================}
{VC4 VC Image Types (From interface\vctypes\vc_image_types.h)}
{$PACKRECORDS C}
{We have so many rectangle types; let's try to introduce a common one.  }
type
 Ptag_VC_RECT_T = ^tag_VC_RECT_T;
 tag_VC_RECT_T = record
  x : int32_t;
  y : int32_t;
  width : int32_t;
  height : int32_t;
 end;
 VC_RECT_T = tag_VC_RECT_T;
 PVC_RECT_T = ^VC_RECT_T;

 {Types of image supported}
 VC_IMAGE_TYPE_T = (
  VC_IMAGE_MIN = 0,      {Bounds for error checking}
  
  VC_IMAGE_RGB565 = 1,
  VC_IMAGE_1BPP,
  VC_IMAGE_YUV420,
  VC_IMAGE_48BPP,
  VC_IMAGE_RGB888,
  VC_IMAGE_8BPP,
  VC_IMAGE_4BPP,          {4bpp palettised image}
  VC_IMAGE_3D32,          {A separated format of 16 colour/light shorts followed by 16 z values}
  VC_IMAGE_3D32B,         {16 colours followed by 16 z values}
  VC_IMAGE_3D32MAT,       {A separated format of 16 material/colour/light shorts followed by 16 z values}
  VC_IMAGE_RGB2X9,        {32 bit format containing 18 bits of 6.6.6 RGB, 9 bits per short}
  VC_IMAGE_RGB666,        {32-bit format holding 18 bits of 6.6.6 RGB}
  VC_IMAGE_PAL4_OBSOLETE, {4bpp palettised image with embedded palette}
  VC_IMAGE_PAL8_OBSOLETE, {8bpp palettised image with embedded palette}
  VC_IMAGE_RGBA32,        {RGB888 with an alpha byte after each pixel (Isn't it BEFORE each pixel?)}
  VC_IMAGE_YUV422,        {A line of Y (32-byte padded), a line of U (16-byte padded), and a line of V (16-byte padded)}
  VC_IMAGE_RGBA565,       {RGB565 with a transparent patch}
  VC_IMAGE_RGBA16,        {Compressed (4444) version of RGBA32}
  VC_IMAGE_YUV_UV,        {VCIII codec format}
  VC_IMAGE_TF_RGBA32,     {VCIII T-format RGBA8888}
  VC_IMAGE_TF_RGBX32,     {VCIII T-format RGBx8888}
  VC_IMAGE_TF_FLOAT,      {VCIII T-format float}
  VC_IMAGE_TF_RGBA16,     {VCIII T-format RGBA4444}
  VC_IMAGE_TF_RGBA5551,   {VCIII T-format RGB5551}
  VC_IMAGE_TF_RGB565,     {VCIII T-format RGB565}
  VC_IMAGE_TF_YA88,       {VCIII T-format 8-bit luma and 8-bit alpha}
  VC_IMAGE_TF_BYTE,       {VCIII T-format 8 bit generic sample}
  VC_IMAGE_TF_PAL8,       {VCIII T-format 8-bit palette}
  VC_IMAGE_TF_PAL4,       {VCIII T-format 4-bit palette}
  VC_IMAGE_TF_ETC1,       {VCIII T-format Ericsson Texture Compressed}
  VC_IMAGE_BGR888,        {RGB888 with R & B swapped}
  VC_IMAGE_BGR888_NP,     {RGB888 with R & B swapped, but with no pitch, i.e. no padding after each row of pixels}
  VC_IMAGE_BAYER,         {Bayer image, extra defines which variant is being used}
  VC_IMAGE_CODEC,         {General wrapper for codec images e.g. JPEG from camera}
  VC_IMAGE_YUV_UV32,      {VCIII codec format}
  VC_IMAGE_TF_Y8,         {VCIII T-format 8-bit luma}
  VC_IMAGE_TF_A8,         {VCIII T-format 8-bit alpha}
  VC_IMAGE_TF_SHORT,      {VCIII T-format 16-bit generic sample}
  VC_IMAGE_TF_1BPP,       {VCIII T-format 1bpp black/white}
  VC_IMAGE_OPENGL,        
  VC_IMAGE_YUV444I,       {VCIII-B0 HVS YUV 4:4:4 interleaved samples}
  VC_IMAGE_YUV422PLANAR,  {Y, U, & V planes separately (VC_IMAGE_YUV422 has them interleaved on a per line basis)}
  VC_IMAGE_ARGB8888,      {32bpp with 8bit alpha at MS byte, with R, G, B (LS byte)}
  VC_IMAGE_XRGB8888,      {32bpp with 8bit unused at MS byte, with R, G, B (LS byte)}
  
  VC_IMAGE_YUV422YUYV,    {Interleaved 8 bit samples of Y, U, Y, V}
  VC_IMAGE_YUV422YVYU,    {Interleaved 8 bit samples of Y, V, Y, U}
  VC_IMAGE_YUV422UYVY,    {Interleaved 8 bit samples of U, Y, V, Y}
  VC_IMAGE_YUV422VYUY,    {Interleaved 8 bit samples of V, Y, U, Y}
  
  VC_IMAGE_RGBX32,        {32bpp like RGBA32 but with unused alpha}
  VC_IMAGE_RGBX8888,      {32bpp, corresponding to RGBA with unused alpha}
  VC_IMAGE_BGRX8888,      {32bpp, corresponding to BGRA with unused alpha}
  
  VC_IMAGE_YUV420SP,      {Y as a plane, then UV byte interleaved in plane with with same pitch, half height}
  
  VC_IMAGE_YUV444PLANAR,  {Y, U, & V planes separately 4:4:4}
  
  VC_IMAGE_TF_U8,         {T-format 8-bit U - same as TF_Y8 buf from U plane}
  VC_IMAGE_TF_V8,         {T-format 8-bit U - same as TF_Y8 buf from V plane}
  
  VC_IMAGE_YUV420_16,     {YUV4:2:0 planar, 16bit values}
  VC_IMAGE_YUV_UV_16,     {YUV4:2:0 codec format, 16bit values}
  
  VC_IMAGE_MAX,           {Bounds for error checking}
  VC_IMAGE_FORCE_ENUM_16BIT = $ffff
 );
 
  {Image transformations (flips and 90 degree rotations). These are made out of 3 primitives (transpose is done first). These must match the DISPMAN and Media Player definitions}
const
 TRANSFORM_HFLIP = 1 shl 0;    
 TRANSFORM_VFLIP = 1 shl 1;    
 TRANSFORM_TRANSPOSE = 1 shl 2;    

type
 VC_IMAGE_TRANSFORM_T = (
  VC_IMAGE_ROT0 = 0,
  VC_IMAGE_MIRROR_ROT0 = TRANSFORM_HFLIP,
  VC_IMAGE_MIRROR_ROT180 = TRANSFORM_VFLIP,
  VC_IMAGE_ROT180 = TRANSFORM_HFLIP or TRANSFORM_VFLIP,
  VC_IMAGE_MIRROR_ROT90 = TRANSFORM_TRANSPOSE,
  VC_IMAGE_ROT270 = TRANSFORM_TRANSPOSE or TRANSFORM_HFLIP,
  VC_IMAGE_ROT90 = TRANSFORM_TRANSPOSE or TRANSFORM_VFLIP,
  VC_IMAGE_MIRROR_ROT270 = (TRANSFORM_TRANSPOSE or TRANSFORM_HFLIP) or TRANSFORM_VFLIP
 );
 
 {Defined to be identical to register bits }
 VC_IMAGE_BAYER_ORDER_T = (
  VC_IMAGE_BAYER_RGGB = 0,
  VC_IMAGE_BAYER_GBRG = 1,
  VC_IMAGE_BAYER_BGGR = 2,
  VC_IMAGE_BAYER_GRBG = 3
 );
 
 {Defined to be identical to register bits }
 VC_IMAGE_BAYER_FORMAT_T = (
  VC_IMAGE_BAYER_RAW6 = 0,
  VC_IMAGE_BAYER_RAW7 = 1,
  VC_IMAGE_BAYER_RAW8 = 2,
  VC_IMAGE_BAYER_RAW10 = 3,
  VC_IMAGE_BAYER_RAW12 = 4,
  VC_IMAGE_BAYER_RAW14 = 5,
  VC_IMAGE_BAYER_RAW16 = 6,
  VC_IMAGE_BAYER_RAW10_8 = 7,
  VC_IMAGE_BAYER_RAW12_8 = 8,
  VC_IMAGE_BAYER_RAW14_8 = 9,
  VC_IMAGE_BAYER_RAW10L = 11,
  VC_IMAGE_BAYER_RAW12L = 12,
  VC_IMAGE_BAYER_RAW14L = 13,
  VC_IMAGE_BAYER_RAW16_BIG_ENDIAN = 14,
  VC_IMAGE_BAYER_RAW4 = 15
 );

{$PACKRECORDS DEFAULT}

{==============================================================================}
{VC4 VCHI Mem Handle (From interface\vchi\vchi_mh.h)}
{$PACKRECORDS C}
type
 VCHI_MEM_HANDLE_T = int32_t;
 
const 
 VCHI_MEM_HANDLE_INVALID = 0;
 
{$PACKRECORDS DEFAULT}

{==============================================================================}
{VC4 VCHI Connection Types (From interface\vchi\connections\connection.h)}
{$PACKRECORDS C}
type
 PPVCHI_CONNECTION_T = ^PVCHI_CONNECTION_T;
 PVCHI_CONNECTION_T = ^VCHI_CONNECTION_T;
 VCHI_CONNECTION_T = record
  {Opaque structure}
 end;

 //To Do //Continuing
 
{$PACKRECORDS DEFAULT}
{==============================================================================}
{VC4 VCHI Types (From interface\vchi\vchi.h)}
{$PACKRECORDS C}
type
 PPVCHI_INSTANCE_T = ^PVCHI_INSTANCE_T;
 PVCHI_INSTANCE_T = ^VCHI_INSTANCE_T;
 VCHI_INSTANCE_T = record
  {Opaque structure}
 end;

//To Do //Continuing

type
 PDISPMANX_ALPHA_T = ^DISPMANX_ALPHA_T;
 DISPMANX_ALPHA_T = record
  flags : DISPMANX_FLAGS_ALPHA_T;
  opacity : uint32_t;
  mask : PVC_IMAGE_T;
 end;

 PVC_DISPMANX_ALPHA_T = ^VC_DISPMANX_ALPHA_T; { for use with vmcs_host  }
 VC_DISPMANX_ALPHA_T = record
  flags : DISPMANX_FLAGS_ALPHA_T;
  opacity : uint32_t;
  mask : DISPMANX_RESOURCE_HANDLE_T; 
 end;
  
type
 PDISPMANX_FLAGS_CLAMP_T = ^DISPMANX_FLAGS_CLAMP_T;
 DISPMANX_FLAGS_CLAMP_T = uint32_t;
const 
 DISPMANX_FLAGS_CLAMP_NONE = 0;
 DISPMANX_FLAGS_CLAMP_LUMA_TRANSPARENT = 1;
 {DISPMANX_FLAGS_CLAMP_TRANSPARENT = 2;}
 {DISPMANX_FLAGS_CLAMP_REPLACE = 3;}
 DISPMANX_FLAGS_CLAMP_CHROMA_TRANSPARENT = 2;
 DISPMANX_FLAGS_CLAMP_TRANSPARENT = 3;

type
 PDISPMANX_FLAGS_KEYMASK_T = ^DISPMANX_FLAGS_KEYMASK_T;
 DISPMANX_FLAGS_KEYMASK_T = uint32_t;
const 
 DISPMANX_FLAGS_KEYMASK_OVERRIDE = 1;
 DISPMANX_FLAGS_KEYMASK_SMOOTH = 1 shl 1;
 DISPMANX_FLAGS_KEYMASK_CR_INV = 1 shl 2;
 DISPMANX_FLAGS_KEYMASK_CB_INV = 1 shl 3;
 DISPMANX_FLAGS_KEYMASK_YY_INV = 1 shl 4;

type
 PDISPMANX_CLAMP_KEYS_T = ^DISPMANX_CLAMP_KEYS_T;
 DISPMANX_CLAMP_KEYS_T = record
  case longint of
   0 : ( yuv : record
    yy_upper : uint8_t;
    yy_lower : uint8_t;
    cr_upper : uint8_t;
    cr_lower : uint8_t;
    cb_upper : uint8_t;
    cb_lower : uint8_t;
   end );
   1 : ( rgb : record
    red_upper : uint8_t;
    red_lower : uint8_t;
    blue_upper : uint8_t;
    blue_lower : uint8_t;
    green_upper : uint8_t;
    green_lower : uint8_t;
   end );
 end;

 PDISPMANX_CLAMP_T = ^DISPMANX_CLAMP_T;
 DISPMANX_CLAMP_T = record
  mode : DISPMANX_FLAGS_CLAMP_T;
  key_mask : DISPMANX_FLAGS_KEYMASK_T;
  key_value : DISPMANX_CLAMP_KEYS_T;
  replace_value : uint32_t;
 end;

 PDISPMANX_MODEINFO_T = ^DISPMANX_MODEINFO_T;
 DISPMANX_MODEINFO_T = record
  width : int32_t;
  height : int32_t;
  transform : DISPMANX_TRANSFORM_T;
  input_format : DISPLAY_INPUT_FORMAT_T;
  display_num : uint32_t;
 end;

 { Update callback.  }
 DISPMANX_CALLBACK_FUNC_T = procedure (u:DISPMANX_UPDATE_HANDLE_T; arg:pointer); cdecl;
 
 { Progress callback  }
 DISPMANX_PROGRESS_CALLBACK_FUNC_T = procedure (u:DISPMANX_UPDATE_HANDLE_T; line:uint32_t; arg:pointer); cdecl;
 
 { Pluggable display interface  }
 Pint32_t_array3 = ^int32_t_array3;
 int32_t_array3 = array[0..2] of int32_t;

 Puint32_t_array3 = ^uint32_t_array3;
 uint32_t_array3 = array[0..2] of uint32_t;
 
 Ptag_DISPMANX_DISPLAY_FUNCS_T = ^tag_DISPMANX_DISPLAY_FUNCS_T;
 tag_DISPMANX_DISPLAY_FUNCS_T = record
  {Get essential HVS configuration to be passed to the HVS driver. Options
   is any combination of the following flags: HVS_ONESHOT, HVS_FIFOREG,
   HVS_FIFO32, HVS_AUTOHSTART, HVS_INTLACE; and if HVS_FIFOREG, one of;
   ( HVS_FMT_RGB888, HVS_FMT_RGB565, HVS_FMT_RGB666, HVS_FMT_YUV) }
  get_hvs_config : function (instance:pointer; pchan:Puint32_t; poptions:Puint32_t; info:PDISPLAY_INFO_T; bg_colour:Puint32_t; 
                    test_mode:Puint32_t):int32_t; cdecl;
  
  {Get optional HVS configuration for gamma tables, OLED matrix and dither controls.
   Set these function pointers to NULL if the relevant features are not required}
  get_gamma_params : function (instance:pointer; gain:Pint32_t_array3; offset:Pint32_t_array3; gamma:Pint32_t_array3):int32_t; cdecl;
  get_oled_params : function (instance:pointer; poffsets:Puint32_t; coeffs:Puint32_t_array3):int32_t; cdecl;
  get_dither : function (instance:pointer; dither_depth:Puint32_t; dither_type:Puint32_t):int32_t; cdecl;
  
  {Get mode information, which may be returned to the applications as a courtesy.
   Transform should be set to 0, and (width,height) should be final dimensions}
  get_info : function (instance:pointer; info:PDISPMANX_MODEINFO_T):int32_t; cdecl;
  
  {Inform driver that the application refcount has become nonzero / zero
   These callbacks might perhaps be used for backlight and power management}
  open : function (instance:pointer):int32_t; cdecl;
  close : function (instance:pointer):int32_t; cdecl;
  
  {Display list updated callback. Primarily of use to a "one-shot" display.
   For convenience of the driver, we pass the register address of the HVS FIFO}
  dlist_updated : procedure (instance:pointer; fifo_reg:Puint32_t);
  
  {End-of-field callback. This may occur in an interrupt context}
  eof_callback : procedure (instance:pointer); cdecl;
 
  {Return screen resolution format}
  get_input_format : function (instance:pointer):DISPLAY_INPUT_FORMAT_T; cdecl;
  
  suspend_resume : function (instance:pointer; up:longint):int32_t; cdecl;
  
  get_3d_format : function (instance:pointer):DISPLAY_3D_FORMAT_T; cdecl;
 end;
 DISPMANX_DISPLAY_FUNCS_T = tag_DISPMANX_DISPLAY_FUNCS_T;
 PDISPMANX_DISPLAY_FUNCS_T = ^DISPMANX_DISPLAY_FUNCS_T;
 
{From interface\vmcs_host\vc_dispmanx.h}
 { Display manager service API }
 { Same function as above, to aid migration of code. }
 function vc_dispman_init:longint; cdecl; external libvchostif name 'vc_dispman_init';

 { Stop the service from being used }
 procedure vc_dispmanx_stop; cdecl; external libvchostif name 'vc_dispmanx_stop';

 { Set the entries in the rect structure }
 function vc_dispmanx_rect_set(rect:PVC_RECT_T; x_offset:uint32_t; y_offset:uint32_t; width:uint32_t; height:uint32_t):longint; cdecl; external libvchostif name 'vc_dispmanx_rect_set';

 { Resources }
 { Create a new resource }
 function vc_dispmanx_resource_create(_type:VC_IMAGE_TYPE_T; width:uint32_t; height:uint32_t; native_image_handle:Puint32_t):DISPMANX_RESOURCE_HANDLE_T; cdecl; external libvchostif name 'vc_dispmanx_resource_create';

 { Write the bitmap data to VideoCore memory }
 function vc_dispmanx_resource_write_data(res:DISPMANX_RESOURCE_HANDLE_T; src_type:VC_IMAGE_TYPE_T; src_pitch:longint; src_address:pointer; rect:PVC_RECT_T):longint; cdecl; external libvchostif name 'vc_dispmanx_resource_write_data';

 function vc_dispmanx_resource_write_data_handle(res:DISPMANX_RESOURCE_HANDLE_T; src_type:VC_IMAGE_TYPE_T; src_pitch:longint; handle:VCHI_MEM_HANDLE_T; offset:uint32_t; 
            rect:PVC_RECT_T):longint; cdecl; external libvchostif name 'vc_dispmanx_resource_write_data_handle';

 function vc_dispmanx_resource_read_data(handle:DISPMANX_RESOURCE_HANDLE_T; p_rect:PVC_RECT_T; dst_address:pointer; dst_pitch:uint32_t):longint; cdecl; external libvchostif name 'vc_dispmanx_resource_read_data';

 { Delete a resource }
 function vc_dispmanx_resource_delete(res:DISPMANX_RESOURCE_HANDLE_T):longint; cdecl; external libvchostif name 'vc_dispmanx_resource_delete';

 { Displays }
 { Opens a display on the given device }
 function vc_dispmanx_display_open(device:uint32_t):DISPMANX_DISPLAY_HANDLE_T; cdecl; external libvchostif name 'vc_dispmanx_display_open';

 { Opens a display on the given device in the request mode }
 function vc_dispmanx_display_open_mode(device:uint32_t; mode:uint32_t):DISPMANX_DISPLAY_HANDLE_T; cdecl; external libvchostif name 'vc_dispmanx_display_open_mode';

 { Open an offscreen display }
 function vc_dispmanx_display_open_offscreen(dest:DISPMANX_RESOURCE_HANDLE_T; orientation:DISPMANX_TRANSFORM_T):DISPMANX_DISPLAY_HANDLE_T; cdecl; external libvchostif name 'vc_dispmanx_display_open_offscreen';

 { Change the mode of a display }
 function vc_dispmanx_display_reconfigure(display:DISPMANX_DISPLAY_HANDLE_T; mode:uint32_t):longint; cdecl; external libvchostif name 'vc_dispmanx_display_reconfigure';

 { Sets the desstination of the display to be the given resource }
 function vc_dispmanx_display_set_destination(display:DISPMANX_DISPLAY_HANDLE_T; dest:DISPMANX_RESOURCE_HANDLE_T):longint; cdecl; external libvchostif name 'vc_dispmanx_display_set_destination';

 { Set the background colour of the display }
 function vc_dispmanx_display_set_background(update:DISPMANX_UPDATE_HANDLE_T; display:DISPMANX_DISPLAY_HANDLE_T; red:uint8_t; green:uint8_t; blue:uint8_t):longint; cdecl; external libvchostif name 'vc_dispmanx_display_set_background';

 { get the width, height, frame rate and aspect ratio of the display }
 function vc_dispmanx_display_get_info(display:DISPMANX_DISPLAY_HANDLE_T; pinfo:PDISPMANX_MODEINFO_T):longint; cdecl; external libvchostif name 'vc_dispmanx_display_get_info';

 { Closes a display }
 function vc_dispmanx_display_close(display:DISPMANX_DISPLAY_HANDLE_T):longint; cdecl; external libvchostif name 'vc_dispmanx_display_close';

 { Updates }
 { Start a new update, DISPMANX_NO_HANDLE on error }
 function vc_dispmanx_update_start(priority:int32_t):DISPMANX_UPDATE_HANDLE_T; cdecl; external libvchostif name 'vc_dispmanx_update_start';

 { Add an elment to a display as part of an update }
 function vc_dispmanx_element_add(update:DISPMANX_UPDATE_HANDLE_T; display:DISPMANX_DISPLAY_HANDLE_T; layer:int32_t; dest_rect:PVC_RECT_T; src:DISPMANX_RESOURCE_HANDLE_T; 
            src_rect:PVC_RECT_T; protection:DISPMANX_PROTECTION_T; alpha:PVC_DISPMANX_ALPHA_T; clamp:PDISPMANX_CLAMP_T; transform:DISPMANX_TRANSFORM_T):DISPMANX_ELEMENT_HANDLE_T; cdecl; external libvchostif name 'vc_dispmanx_element_add';

 { Change the source image of a display element }
 function vc_dispmanx_element_change_source(update:DISPMANX_UPDATE_HANDLE_T; element:DISPMANX_ELEMENT_HANDLE_T; src:DISPMANX_RESOURCE_HANDLE_T):longint; cdecl; external libvchostif name 'vc_dispmanx_element_change_source';

 { Change the layer number of a display element }
 function vc_dispmanx_element_change_layer(update:DISPMANX_UPDATE_HANDLE_T; element:DISPMANX_ELEMENT_HANDLE_T; layer:int32_t):longint; cdecl; external libvchostif name 'vc_dispmanx_element_change_layer';

 { Signal that a region of the bitmap has been modified }
 function vc_dispmanx_element_modified(update:DISPMANX_UPDATE_HANDLE_T; element:DISPMANX_ELEMENT_HANDLE_T; rect:PVC_RECT_T):longint; cdecl; external libvchostif name 'vc_dispmanx_element_modified';

 { Remove a display element from its display }
 function vc_dispmanx_element_remove(update:DISPMANX_UPDATE_HANDLE_T; element:DISPMANX_ELEMENT_HANDLE_T):longint; cdecl; external libvchostif name 'vc_dispmanx_element_remove';

 { Ends an update }
 function vc_dispmanx_update_submit(update:DISPMANX_UPDATE_HANDLE_T; cb_func:DISPMANX_CALLBACK_FUNC_T; cb_arg:pointer):longint; cdecl; external libvchostif name 'vc_dispmanx_update_submit';

 { End an update and wait for it to complete }
 function vc_dispmanx_update_submit_sync(update:DISPMANX_UPDATE_HANDLE_T):longint; cdecl; external libvchostif name 'vc_dispmanx_update_submit_sync';

 { Query the image formats supported in the VMCS build }
 function vc_dispmanx_query_image_formats(supported_formats:Puint32_t):longint; cdecl; external libvchostif name 'vc_dispmanx_query_image_formats';

 {New function added to VCHI to change attributes, set_opacity does not work there. }
 function vc_dispmanx_element_change_attributes(update:DISPMANX_UPDATE_HANDLE_T; element:DISPMANX_ELEMENT_HANDLE_T; change_flags:uint32_t; layer:int32_t; opacity:uint8_t; 
            dest_rect:PVC_RECT_T; src_rect:PVC_RECT_T; mask:DISPMANX_RESOURCE_HANDLE_T; transform:DISPMANX_TRANSFORM_T):longint; cdecl; external libvchostif name 'vc_dispmanx_element_change_attributes';

 {xxx hack to get the image pointer from a resource handle, will be obsolete real soon }
 function vc_dispmanx_resource_get_image_handle(res:DISPMANX_RESOURCE_HANDLE_T):uint32_t; cdecl; external libvchostif name 'vc_dispmanx_resource_get_image_handle';

 {Call this instead of vc_dispman_init }
 procedure vc_vchi_dispmanx_init(initialise_instance:PVCHI_INSTANCE_T; connections:PPVCHI_CONNECTION_T; num_connections:uint32_t); cdecl; external libvchostif name 'vc_vchi_dispmanx_init';

 { Take a snapshot of a display in its current state. }
 { This call may block for a time; when it completes, the snapshot is ready. }
 { only transform=0 is supported }
 function vc_dispmanx_snapshot(display:DISPMANX_DISPLAY_HANDLE_T; snapshot_resource:DISPMANX_RESOURCE_HANDLE_T; transform:DISPMANX_TRANSFORM_T):longint; cdecl; external libvchostif name 'vc_dispmanx_snapshot';

 { Set the resource palette (for VC_IMAGE_4BPP and VC_IMAGE_8BPP) }
 function vc_dispmanx_resource_set_palette(handle:DISPMANX_RESOURCE_HANDLE_T; src_address:pointer; offset:longint; size:longint):longint; cdecl; external libvchostif name 'vc_dispmanx_resource_set_palette';

 { Start triggering callbacks synced to vsync }
 function vc_dispmanx_vsync_callback(display:DISPMANX_DISPLAY_HANDLE_T; cb_func:DISPMANX_CALLBACK_FUNC_T; cb_arg:pointer):longint; cdecl; external libvchostif name 'vc_dispmanx_vsync_callback';
 
implementation

end.
