{
  Copyright 2004-2018 Michalis Kamburelis.

  This file is part of "Castle Game Engine".

  "Castle Game Engine" is free software; see the file COPYING.txt,
  included in this distribution, for details about the copyright.

  "Castle Game Engine" is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  ----------------------------------------------------------------------------
}

// TODO: this should probably not be in a separate unit, better use conditional blocks
//  within castlewindow_dispmanx.inc 

{$ifdef read_interface_uses}
// The following two imports do nor really belong in the interface part, but 
// must be here because cthreads must be initialized early.
{$ifdef unix}
  cthreads,
  cmem, // the c memory manager is on some systems much faster for multi-threading
{$endif}
DispmanX, VC4, // EGL, // NOTE: trying to use CGE's built-in EGL declarations right now
{$endif}

{$ifdef read_implementation_uses}
{$endif}

{$ifdef read_window_interface}
private
  display: EGLDisplay;
  surface: EGLSurface;
  context: EGLContext;
  config : EGLConfig;
  native_window: EGL_DISPMANX_WINDOW_T;
  dispman_display: DISPMANX_DISPLAY_HANDLE_T;

  procedure InitDispmanX;
  procedure InitEGL;
  procedure EGLFromDispmanX;
{$endif read_window_interface}

{$ifdef read_application_interface}
private
  screen_width, screen_height: UInt32;
  procedure InitDispmanX;
{$endif read_application_interface}

{$ifdef read_implementation}

{ TCastleWindowCustom ------------------------------------------------------------------ }

{ DispmanX/Raspberry Pi specific methods }

procedure TCastleWindowCustom.InitDispmanX;
var
  dispman_element: DISPMANX_ELEMENT_HANDLE_T;
  dispman_update: DISPMANX_UPDATE_HANDLE_T;
  src_rect, dst_rect: VC_RECT_T;
begin

  //Application.InitDispmanX;

  WritelnLog('CastleWindow', 'Opening Dispman display...');
  dispman_display := vc_dispmanx_display_open(0 {LCD});
  if dispman_display = 0 then raise Exception.Create('Failed to open dispman display');
  WritelnLog('CastleWindow', 'done.');

  WritelnLog('CastleWindow', 'Creating Dispman window from display...');
  dst_rect.x := 0;
  dst_rect.y := 0;
  dst_rect.width  := Application.ScreenWidth;
  dst_rect.height := Application.ScreenHeight;
  WritelnLog('CastleWindow', 'screen size: %dx%d', [Application.ScreenWidth, Application.ScreenHeight]);
  src_rect.x := 0;
  src_rect.y := 0;
  src_rect.width  := Application.ScreenWidth  shl 16;
  src_rect.height := Application.ScreenHeight shl 16;
  dispman_update := vc_dispmanx_update_start(0);
  if dispman_update = 0 then raise Exception.Create('Failed to obtain update handle');
  dispman_element := vc_dispmanx_element_add(dispman_update, dispman_display, 
    0 {layer}, @src_rect, 0 {source}, @dst_rect, DISPMANX_PROTECTION_NONE, 
    nil {alpha}, nil {clamp}, 0 {transform});
  if dispman_element = 0 then raise Exception.Create('Failed to add Dispman element');
  native_window.element := dispman_element;
  native_window.width  := Application.ScreenWidth;
  native_window.height := Application.ScreenHeight;
  vc_dispmanx_update_submit_sync(dispman_update);
  vc_dispmanx_element_remove(dispman_update, dispman_element);

  WritelnLog('CastleWindow', 'done.');
end;

procedure TCastleWindowCustom.InitEGL;
const
  config_attribs: array[0..12] of EGLint = (
    EGL_RED_SIZE, 8,
    EGL_GREEN_SIZE, 8,
    EGL_BLUE_SIZE, 8,
    EGL_ALPHA_SIZE, 8,
    EGL_DEPTH_SIZE, 24,
    EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
    EGL_NONE
  );
  context_attribs: array[0..2] of EGLint = (
    EGL_CONTEXT_CLIENT_VERSION, 2,
    EGL_NONE
  );
var
  Res: EGLboolean;
  Major, Minor: EGLint;
  num_configs: EGLint;
  //configs: array of EGLConfig;
begin
  WritelnLog('CastleWindow', 'Obtaining EGL default "display".');
  display := eglGetDisplay(EGL_DEFAULT_DISPLAY);
  if display = nil then raise Exception.Create('Failed to obtain EGL default display');

  WritelnLog('CastleWindow', 'Initializing EGL display connection...');
  Res := eglInitialize(display, @Major, @Minor);
  if Res = EGL_FALSE then raise Exception.Create('Failed to initialize the EGL display');
  WritelnLog('CastleWindow', 'done, EGL version is %d.%d', [Major, Minor]);

  {$ifdef NOT_DEFINED}
  WritelnLog('CastleWindow', 'Obtaining EGL display configs...');
  eglGetConfigs(display, nil, 0, @num_configs);
  // TODO: abort if num_configs = 0
  SetLength(configs, num_configs);
  eglGetConfigs(display, @configs, num_configs, @num_configs);
  WritelnLog('CastleWindow', 'obtained %d configs', [num_configs]);
  {$endif}

  WritelnLog('CastleWindow', 'Choosing a display config...');
  Res := eglChooseConfig(display, @config_attribs, @config, 1, @num_configs);
  if Res <> EGL_TRUE then raise Exception.Create('Failed to choose a display configuration');
  WritelnLog('CastleWindow', 'done.');

  WritelnLog('CastleWindow', 'Binding the OpenGL ES API...');
  Res := eglBindAPI(EGL_OPENGL_ES_API);
  if Res <> EGL_TRUE then raise Exception.Create('Failed to bind OpenGL ES API');
  WritelnLog('CastleWindow', 'done.');

  WritelnLog('CastleWindow', 'Creating EGL context...');
  context := eglCreateContext(display, config, EGL_NO_CONTEXT, @context_attribs);
  if context = nil then raise Exception.Create('Failed to create EGL context');
  WritelnLog('CastleWindow', 'done.');
end;

procedure TCastleWindowCustom.EGLFromDispmanX;
var
  Res: EGLboolean;
begin
  WritelnLog('CastleWindow', 'Creating a Surface for the native Window...');
  // Note: egl.pas defines EGLNativeWindowType = Pointer
  surface := eglCreateWindowSurface(display, config, @native_window, nil);
  if surface = EGL_NO_SURFACE then raise Exception.Create('Failed to create Window Surface');
  WritelnLog('CastleWindow', 'done.');

  WritelnLog('CastleWindow', 'Connecting the rendering context to the Surface...');
  Res := eglMakeCurrent(display, surface, surface, context);
  if Res = EGL_FALSE then raise Exception.Create('Failed to connect the rendering context to the Surface');
  WritelnLog('CastleWindow', 'done.');
end;

// TODO: all the cleanup!


{ Standard methods }

procedure TCastleWindowCustom.CreateBackend;
begin
  { Initialize backend-specific per-window stuff }
  { 2018-05-23 MOVED to OpenBackend as per Discord with michalis }
end;

procedure TCastleWindowCustom.BackendMenuInitialize;
begin
  { TODO (may do nothing if you do not care about displaying menu) }
end;

procedure TCastleWindowCustom.BackendMenuFinalize;
begin
  { TODO (may do nothing if you do not care about displaying menu) }
end;

procedure TCastleWindowCustom.MenuUpdateCaption(Entry: TMenuEntryWithCaption);
begin
  { TODO (may do nothing if you do not care about displaying menu;
    may do MenuFinalize + MenuInitialize if you display menu,
    but don't care about updating it fast) }
  MenuFinalize;
  MenuInitialize;
end;

procedure TCastleWindowCustom.MenuUpdateEnabled(Entry: TMenuEntryWithCaption);
begin
  { TODO (may do nothing if you do not care about displaying menu;
    may do MenuFinalize + MenuInitialize if you display menu,
    but don't care about updating it fast) }
  MenuFinalize;
  MenuInitialize;
end;

procedure TCastleWindowCustom.MenuUpdateChecked(Entry: TMenuItemChecked);
begin
  { TODO (may do nothing if you do not care about displaying menu;
    may do MenuFinalize + MenuInitialize if you display menu,
    but don't care about updating it fast) }
  MenuFinalize;
  MenuInitialize;
end;

function TCastleWindowCustom.MenuUpdateCheckedFast: boolean;
begin
  Result := false;
end;

procedure TCastleWindowCustom.MenuInsert(const Parent: TMenu;
  const ParentPosition: Integer; const Entry: TMenuEntry);
begin
  { TODO (may do nothing if you do not care about displaying menu;
    may do MenuFinalize + MenuInitialize if you display menu,
    but don't care about updating it fast) }
  MenuFinalize;
  MenuInitialize;
end;

procedure TCastleWindowCustom.MenuDelete(const Parent: TMenu;
  const ParentPosition: Integer; const Entry: TMenuEntry);
begin
  { TODO (may do nothing if you do not care about displaying menu;
    may do MenuFinalize + MenuInitialize if you display menu,
    but don't care about updating it fast) }
  MenuFinalize;
  MenuInitialize;
end;

procedure TCastleWindowCustom.SwapBuffers;
begin
  //WritelnLog('CastleWindow', 'SwapBuffers');
  if eglSwapBuffers(Display, Surface) = EGL_FALSE then
    WritelnWarning('CastleWindow', 'Cannot swap buffers (this is normal if app is no longer active)');
end;

procedure TCastleWindowCustom.OpenBackend;
begin
  InitDispmanX;
  InitEGL;
  EGLFromDispmanX;

  FLeft := 0; FTop := 0;
  DoResize(Application.ScreenWidth, Application.ScreenHeight, false);

  Application.OpenWindowsAdd(Self);
end;

procedure TCastleWindowCustom.CloseBackend;
begin
  WritelnLog('CastleWindow', 'CloseBackend');
  { TODO (destroy OpenGL context, hide window; counterpart to OpenBackend) }
end;

procedure TCastleWindowCustom.SetCaption(const Part: TCaptionPart; const Value: string);
begin
  FCaption[Part] := Value;
  if not Closed then { TODO: use GetWholeCaption };
end;

procedure TCastleWindowCustom.BackendMakeCurrent;
begin
  Assert(not Closed);
  if eglMakeCurrent(Display, Surface, Surface, Context) = EGL_FALSE then
    WritelnWarning('CastleWindow', 'Cannot make context current: ');
end;

procedure TCastleWindowCustom.SetCursor(const Value: TMouseCursor);
begin
  if FCursor <> Value then
  begin
    FCursor := Value;
    if not Closed then
      { TODO UpdateCursor };
  end;
end;

procedure TCastleWindowCustom.SetCustomCursor(const Value: TRGBAlphaImage);
begin
  { TODO }
  FCustomCursor := Value;
end;

function TCastleWindowCustom.RedirectKeyDownToMenuClick: boolean;
begin
  Result := { TODO } true;
end;

procedure TCastleWindowCustom.SetMousePosition(const Value: TVector2);
begin
  if not Closed then
    { TODO };
end;

procedure TCastleWindowCustom.UpdateFullScreenBackend;
begin
  { This is the simplest implementation of this method: }
  SimpleUpdateFullScreenBackend;
  { Some backends may provide a better way of doing this.
    Remove the above line then, and put better implementation here. }
end;

{ TCastleWindowCustom dialogs -------------------------------------------------- }

{ Methods below should make native-looking dialog boxes.
  Alternatively, you can include castlewindow_dialogs_by_messages.inc
  (outside of $ifdef read_implementation, usually at the beginning of
  castlewindow_xxx.inc file), to have simple dialog boxes drawn on
  OpenGL context using our CastleMessages unit. }

function TCastleWindowCustom.BackendFileDialog(const Title: string; var FileName: string;
  OpenDialog: boolean; FileFilters: TFileFilterList): boolean;
begin
  { TODO } Result := false;
end;

function TCastleWindowCustom.ColorDialog(var Color: TVector3): boolean;
begin
  { TODO } Result := false;
end;

procedure TCastleWindowCustom.MessageOK(const S: string; const MessageType: TWindowMessageType);
begin
  { TODO }
end;

function TCastleWindowCustom.MessageYesNo(const S: string;
  const MessageType: TWindowMessageType): boolean;
begin
  { TODO } Result := true;
end;

{ TCastleApplication ---------------------------------------------------------- }

{ DispmanX/Raspberry Pi specific methods }

procedure TCastleApplication.InitDispmanX;
var
  Success: Int32;
begin
  InitializeLog;
  BCMHostInit;
  WritelnLog('DispmanX Application', 'BCM host initialized.');

  Success := BCMHostGraphicsGetDisplaySize(0 {LCD}, screen_width, screen_height);
  if Success < 0 then raise Exception.Create('Failed to obtain LCD display size');
  WritelnLog('DispmanX Application', 'LCD display size obtained: %dx%d', [screen_width, screen_height]);
end;

{ Standard methods }

procedure TCastleApplication.CreateBackend;
begin
  { TODO (initialize backend-specific stuff for the whole application;
    may do nothing if you don't need anything) }
  InitDispmanX;
end;

procedure TCastleApplication.DestroyBackend;
begin
  { TODO }
end;

function TCastleApplication.ProcessMessage(WaitForMessage, WaitToLimitFPS: boolean): boolean;
begin
  Result := not Terminated;

  if Result then
  begin
    UpdateAndRenderEverything;
    Result := not Terminated;
  end;
end;

function TCastleApplication.ProcessAllMessages: boolean;
begin
  Result := ProcessMessage(false, false);
end;

procedure TCastleApplication.Run;
begin
  if OpenWindowsCount = 0 then Exit;

  { Implementing Run by calling ProcessMessage in a loop (like below)
    is a valid and good implementation. Make sure your ProcessMessage
    honours WaitForMessage and WaitToLimitFPS = true,
    to avoid wasting CPU on "busy waiting". }
  while ProcessMessage(true, true) do ;
end;

procedure TCastleApplication.QuitWhenNoOpenWindows;
begin
  { TODO }
end;

function TCastleApplication.ScreenWidth: integer;
begin
  Result := screen_width;
end;

function TCastleApplication.ScreenHeight: integer;
begin
  Result := screen_height;
end;

function TCastleApplication.ScreenStatusBarScaledHeight: Cardinal;
begin
  Result := 0;
end;

function TCastleApplication.BackendName: string;
begin
  { TODO }
  Result := 'CastleWindow Raspberry Pi (X-less) backend';
end;

{ TCastleWindow -------------------------------------------------------------- }

procedure TCastleWindow.NavigationInfoChanged;
begin
end;

{$endif read_implementation}
