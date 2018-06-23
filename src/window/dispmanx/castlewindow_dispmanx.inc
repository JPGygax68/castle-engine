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

{
  This is Castle Game Engine backend for DispmanX, i.e. for the Raspberry Pi
  without X or Wayland.

  THIS BACK-END IS EXPERIMENTAL!

  Please report all questions and problems to:
  Jean-Pierre Gygax <gygax@practicomp.ch> ("JPNotADragon" on the 
  "castle-engine" Discord network).

  To use this backend, define the preprocessor symbols 
  CASTLE_ENGINE_RASPBERRY_PI and CASTLE_WINDOW_DISPMANX. You can do this in 
  your personal Freepascal configuration file `~/.fpc.cfg`. For example:

  ```
    #include /etc/fpc.cfg

    -dCASTLE_ENGINE_RASPBERRY_PI
    -dCASTLE_WINDOW_DISPMANX
    -Fl/opt/vc/lib/
  ```

  The first symbol is not actually related to the backend, but switches the
  engine from OpenGL to OpenGL ES (version 2), which is necessary since
  the Pi only supports the latter in hardware. The `-Fl' option tells
  Freepascal where to find the Broadcom libraries (it is possible that this
  search path is already defined globally in `/etc/fpc.cfg`).

  What this backend does
  ----------------------

  This backend makes it possible to use the Castle Game Engine on the 
  Raspberry Pi under Linux (tested with Raspbian) without X or Wayland.

  This is possible because Broadcom (the manufacturer of the System-on-Chip)
  provides a low-level graphics API called DispmanX, which, in cooperation
  with several other Broadcom-provided libraries, makes it possible to
  access the VideoCore 4 chip that is a the core of all the Pi's special
  abilities, configure the connected display and then draw on it via GLESv2
  (using EGL for housekeeping and presentation tasks).

  For input, the DispmanX backend relies on the `/dev/input/eventX` files
  provided by Linux. By listening to all of those device files, the backend 
  gets informed about touchpad, mouse, and keyboard input (read about 
  limitations under "Notes" below), which it translates and relays to the
  engine as all backends do.

  Notes and limitations:
  ---------------------

  - I have only ever used this backend on my FT5406 7" touch display (the
    "official" display for the Raspberry Pi). It should work on other
    displays - please test and report!

  - Touch input is supposed to work, but wasn't formally tested and green-
    lighted (the `draw_toy` example app does not seem to work correctly,
    and we haven't yet established why that is).

  - Mouse input has yet to be reviewed by the engine author. Currently,
    the mouse pointer starts at the center of the screen, but is totally
    unconstrained in all directions. This will most probably change soon.

  - Keyboard input is somewhat limited in that, as is the case for all 
    backends except GTK and Windows, it does not use keymaps, so that text 
    input is in effect limited to US ASCII. (We have a couple of ideas on how 
    to address this, so please inform us if that particular limitation is a 
    concern to you).

  - This backend does not currently take any of the configuration options
    (such as multisampling) into account. (Though it does enable the maximum
    level of multisampling available on the Pi, which is 4 - there's just no
    way to change that right now.)
}

{$ifdef read_interface_uses}
// The following two imports do not really belong in the interface part, but 
// must be here because cthreads must be initialized early.
{$ifdef unix}
  //cthreads,
  pthreads,
  //cmem, // the c memory manager is on some systems much faster for multi-threading
{$endif}
DispmanX, VC4, // CastleGLES20,
{$endif}

{$ifdef read_implementation_uses}
BaseUnix, CastleLinuxEvents, CastleLinuxUtils,
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
  procedure CleanupDispmanX;
  procedure InitEGL;
  procedure CleanupEGL;
  procedure EGLFromDispmanX;
{$endif read_window_interface}

{$ifdef read_application_interface}
type 
  TTouchState = (tsInactive = 0, tsDown, tsUp, tsMotion);
  TTouch = record
    State: TTouchState;
    LastPosition, CurrentPosition: TVector2;
  end;
  TMultiTouch = array[0..4] of TTouch;

private
  // TODO: support multiple screens!
  FScreenWidth, FScreenHeight: UInt32;
  FInputFds: array of Longint;
  FTouchSlot: Integer;
  FMultiTouch: TMultiTouch;
  FShiftState: TShiftState;

  procedure InitDispmanX;
  procedure ShutdownDispmanX;
  procedure OpenAllInputDevices;
  procedure CloseAllInputDevices;
  procedure HandleInputEvents;
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

  WritelnLog('CastleWindowCustom', 'Opening DispmanX display...');
  dispman_display := vc_dispmanx_display_open(0 {LCD});
  if dispman_display = 0 then raise Exception.Create('Failed to open dispmanx display');
  WritelnLog('CastleWindowCustom', 'done.');

  WritelnLog('CastleWindowCustom', 'Creating DispmanX window from display...');
  dst_rect.x := 0;
  dst_rect.y := 0;
  dst_rect.width  := Application.ScreenWidth;
  dst_rect.height := Application.ScreenHeight;
  WritelnLog('CastleWindowCustom', 'screen size: %dx%d', [Application.ScreenWidth, Application.ScreenHeight]);
  src_rect.x := 0;
  src_rect.y := 0;
  src_rect.width  := Application.ScreenWidth  shl 16;
  src_rect.height := Application.ScreenHeight shl 16;
  dispman_update := vc_dispmanx_update_start(0);
  if dispman_update = 0 then raise Exception.Create('Failed to obtain update handle');
  dispman_element := vc_dispmanx_element_add(dispman_update, dispman_display, 
    0 {layer}, @dst_rect, 0 {source}, @src_rect, DISPMANX_PROTECTION_NONE, 
    nil {alpha}, nil {clamp}, 0 {transform});
  if dispman_element = 0 then raise Exception.Create('Failed to add Dispman element');
  native_window.element := dispman_element;
  native_window.width  := Application.ScreenWidth;
  native_window.height := Application.ScreenHeight;
  vc_dispmanx_update_submit_sync(dispman_update);
  vc_dispmanx_element_remove(dispman_update, dispman_element);

  WritelnLog('CastleWindowCustom', 'done.');
end;

procedure TCastleWindowCustom.CleanupDispmanX;
begin
  WritelnLog('CastleWindowCustom', 'Closing DispmanX display...');
  if vc_dispmanx_display_close(dispman_display) <> 0 then WritelnWarning('TCastleWindowCustom', 'Error');
  WritelnLog('CastleWindowCustom', 'Done.');
end;

procedure TCastleWindowCustom.InitEGL;
const
  config_attribs: array[0..18] of EGLint = (
    EGL_RED_SIZE,        8,
    EGL_GREEN_SIZE,      8,
    EGL_BLUE_SIZE,       8,
    EGL_ALPHA_SIZE,      8,
    EGL_DEPTH_SIZE,      24, 
    EGL_SURFACE_TYPE,    EGL_WINDOW_BIT, 
    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT, 
    EGL_SAMPLE_BUFFERS,  1,
    EGL_SAMPLES,         4,
    //EGL_RENDER_BUFFER,   EGL_SINGLE_BUFFER,
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
  {$ifndef NOT_DEFINED}
  configs: array of EGLConfig;
  {$endif}
begin
  WritelnLog('TCastleWindowCustom', 'Obtaining EGL default display...');
  display := eglGetDisplay(EGL_DEFAULT_DISPLAY);
  if display = nil then raise Exception.Create('Failed to obtain EGL default display');
  WritelnLog('TCastleWindowCustom', 'done.');

  // eglQueryString() only return nil pointers
  {$ifdef NOT_DEFINED}
  WritelnLog('TCastleWindowCustom', 'Dumping display information...');
  WritelnLog('TCastleWindowCustom', 'APIs      : ' + StrPas(eglQueryString(display, EGL_CLIENT_APIS)));
  WritelnLog('TCastleWindowCustom', 'Vendor    : ' + StrPas(eglQueryString(display, EGL_VENDOR)     ));
  WritelnLog('TCastleWindowCustom', 'Version   : ' + StrPas(eglQueryString(display, EGL_VERSION)    ));
  WritelnLog('TCastleWindowCustom', 'Extensions: ' + StrPas(eglQueryString(display, EGL_EXTENSIONS) ));
  WritelnLog('TCastleWindowCustom', 'Display info dumped.');
  {$endif}

  WritelnLog('TCastleWindowCustom', 'Initializing EGL display connection...');
  Res := eglInitialize(display, @Major, @Minor);
  if Res = EGL_FALSE then raise Exception.Create('Failed to initialize the EGL display');
  WritelnLog('TCastleWindowCustom', 'done, EGL version is %d.%d', [Major, Minor]);

  {$ifdef NOT_DEFINED}
  WritelnLog('TCastleWindowCustom', 'Obtaining EGL display configs...');
  eglGetConfigs(display, nil, 0, @num_configs);
  // TODO: abort if num_configs = 0
  SetLength(configs, num_configs);
  eglGetConfigs(display, @configs, num_configs, @num_configs);
  WritelnLog('TCastleWindowCustom', 'obtained %d configs', [num_configs]);
  {$endif}

  WritelnLog('TCastleWindowCustom', 'Choosing a display config...');
  Res := eglChooseConfig(display, @config_attribs, @config, 1, @num_configs);
  if Res <> EGL_TRUE then raise Exception.Create('Failed to choose a display configuration');
  WritelnLog('TCastleWindowCustom', 'done.');

  WritelnLog('TCastleWindowCustom', 'Binding the OpenGL ES API...');
  Res := eglBindAPI(EGL_OPENGL_ES_API);
  if Res <> EGL_TRUE then raise Exception.Create('Failed to bind OpenGL ES API');
  WritelnLog('TCastleWindowCustom', 'done.');

  WritelnLog('TCastleWindowCustom', 'Creating EGL context...');
  context := eglCreateContext(display, config, EGL_NO_CONTEXT, @context_attribs);
  if context = nil then raise Exception.Create('Failed to create EGL context');
  WritelnLog('TCastleWindowCustom', 'done.');
end;

procedure TCastleWindowCustom.EGLFromDispmanX;
var
  Res: EGLboolean;
begin
  WritelnLog('TCastleWindowCustom', 'Creating a Surface for the native Window...');
  // Note: egl.pas defines EGLNativeWindowType = Pointer
  surface := eglCreateWindowSurface(display, config, @native_window, nil);
  if surface = EGL_NO_SURFACE then raise Exception.Create('Failed to create Window Surface');
  WritelnLog('TCastleWindowCustom', 'done.');

  WritelnLog('TCastleWindowCustom', 'Connecting the rendering context to the Surface...');
  Res := eglMakeCurrent(display, surface, surface, context);
  if Res = EGL_FALSE then raise Exception.Create('Failed to connect the rendering context to the Surface');
  WritelnLog('TCastleWindowCustom', 'done.');

  // TODO: the following may not be useful, I just copied it from rasperf3d
  eglSwapInterval(display, 0);  // no vsync
end;

procedure TCastleWindowCustom.CleanupEGL;
begin
  WritelnLog('TCastleWindowCustom', 'Cleaning up EGL');
  if (surface <> EGL_NO_SURFACE) and (eglDestroySurface(display, surface) <> 0) then
    WritelnLog('TCastleWindowCustom', 'EGL surface destroyed ok');
  if (context <> EGL_NO_CONTEXT) and (eglDestroyContext(display, context) <> 0) then
    WritelnLog('TCastleWindowCustom', 'EGL context destroyed ok');
  if (display <> EGL_NO_DISPLAY) and (eglTerminate(display) <> 0) then
    WritelnLog('TCastleWindowCustom', 'EGL display terminated ok');
end;


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
  //WritelnLog('TCastleWindowCustom', 'SwapBuffers');
  if eglSwapBuffers(Display, Surface) = EGL_FALSE then
    WritelnWarning('TCastleWindowCustom', 'Cannot swap buffers (this is normal if app is no longer active)');
end;

procedure TCastleWindowCustom.OpenBackend;
begin
  // TODO: honour: DoubleBuffer, StencilBits, DepthBits, AlphaBits, AccumBits, MultiSampling
  
  InitDispmanX;
  InitEGL;
  EGLFromDispmanX;

  FLeft := 0; FTop := 0;
  DoResize(Application.ScreenWidth, Application.ScreenHeight, false);

  Application.OpenWindowsAdd(Self);

  //FFullScreenBackend := true;
end;

procedure TCastleWindowCustom.CloseBackend;
begin
  { TODO (destroy OpenGL context, hide window; counterpart to OpenBackend) }
  WritelnLog('TCastleWindowCustom', 'CloseBackend');
  CleanupEGL;
end;

procedure TCastleWindowCustom.SetCaption(const Part: TCaptionPart; const Value: string);
begin
  FCaption[Part] := Value;
  if not Closed then { TODO: use GetWholeCaption };
end;

procedure TCastleWindowCustom.BackendMakeCurrent;
begin
  Assert(not Closed);
  WritelnLog('CastleWindowCustom', 'BackendMakeCurrent');
  if eglMakeCurrent(Display, Surface, Surface, Context) = EGL_FALSE then
    WritelnWarning('TCastleWindowCustom', 'Cannot make context current: ');
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
  WritelnLog('TCastleApplication', 'BCM host initialized.');
  Success := BCMHostGraphicsGetDisplaySize(0 {LCD}, FScreenWidth, FScreenHeight);
  if Success < 0 then raise Exception.Create('Failed to obtain LCD display size'); // TODO: custom exception class ?
  WritelnLog('TCastleApplication', 'LCD display size obtained: %dx%d', [FScreenWidth, FScreenHeight]);
end;

procedure TCastleApplication.ShutdownDispmanX;
begin
  BCMHostDeinit;
  WritelnWarning('TCastleApplication_dispmanx.ShutdownDispmanX', 'BCM host deinitialized');
end;

procedure TCastleApplication.OpenAllInputDevices; // TODO: better name
const
  ParentDir: String = '/dev/input/';
var
  SR: TSearchRec;
  Filename: String;
  i: Integer;
begin
  // Open all input file
  WritelnLog('TCastleApplication', 'Opening all input devices');
  i := 0;
  if FindFirst(ParentDir + 'event*', (faAnyFile And Not faDirectory) , SR) = 0 then
  begin
    repeat
      Filename := ParentDir + SR.Name;
      SetLength(FInputFds, i + 1);
      WritelnLog('TCastleApplication', 'Opening input file "%s"', [Filename]);
      FInputFds[i] := fpOpen(Filename, O_RdOnly or O_NonBlock);
      Inc(i);
    until FindNext(SR) <> 0;
  end;
  FindClose(SR);

  // Initialize touch states (5 max)
  for i := Low(FMultiTouch) to High(FMultiTouch) do FMultiTouch[i].State := tsInactive;
  // Special case: multi-touch "finger 0" is treated the same as the mouse (or mice)
  FMultiTouch[0].CurrentPosition.X := ScreenWidth  / 2;
  FMultiTouch[0].CurrentPosition.Y := ScreenHeight / 2;
  FTouchSlot := -1;

  // Keyboard
  FShiftState := []; // TODO: obtain capslock, numlock states ?
end;


procedure TCastleApplication.CloseAllInputDevices;
var
  i: Integer;
begin
  WritelnLog('TCastleApplication', 'Closing all %d input devices', [Length(FInputFds)]);
  for i := 0 to High(FInputFds) do fpClose(FInputFds[i]);
  SetLength(FInputFds, 0);
end;

procedure TCastleApplication.HandleInputEvents;
  
  procedure ReportMouseButton(BtnNum: Integer; Down: Boolean);
  var
    Position: TVector2;
    Button: CastleKeysMouse.TMouseButton;
  begin
    Position := FMultiTouch[0].CurrentPosition;
    Button := TMouseButton(Integer(CastleKeysMouse.mbLeft) + BtnNum);
    if Down then
      GuessedMainWindow.DoMouseDown(Position, Button, 0)
    else
      GuessedMainWindow.DoMouseUp  (Position, Button, 0);
  end;

  procedure ReportMultiTouch;
  begin
    if FTouchSlot >= 0 then
    begin
      WritelnLog('TCastleApplication', 'ReportMultiTouch');
      with FMultiTouch[FTouchSlot] do
      begin
        case State of
          tsInactive: // "finger down" is implicit when first position is received
            begin
              LastPosition := CurrentPosition; // TODO: correct ? reporting same position twice
              //WritelnLog('TCastleApplication', 'MouseDown: %f, %f, finger %d', [CurrentPosition.X, CurrentPosition.Y, FTouchSlot]);
              GuessedMainWindow.DoMouseDown(CurrentPosition, CastleKeysMouse.mbLeft, FTouchSlot);
              State := tsDown;
              LastPosition := CurrentPosition;
            end;
          tsDown:
            begin
              //WritelnLog('TCastleApplication', 'MouseMotion: %f, %f -> %f, %f, finger %d', [LastPosition.X, LastPosition.Y, 
              //  CurrentPosition.X, CurrentPosition.Y, FTouchSlot]);
              GuessedMainWindow.DoMotion(InputMotion(LastPosition, CurrentPosition, [], FTouchSlot));
              LastPosition := CurrentPosition;
            end;
          tsup: // "finger up" was registered via a tracking id of -1
            begin
              //WritelnLog('TCastleApplication', 'MouseUp: %f, %f, finger %d', [CurrentPosition.X, CurrentPosition.Y, FTouchSlot]);
              GuessedMainWindow.DoMouseUp(CurrentPosition, CastleKeysMouse.mbLeft, FTouchSlot);
              State := tsInactive;
            end;
        end;
      end;
      FTouchSlot := -1;
    end;
  end;

  procedure MultiTouchSelectFinger(Slot: Longint);
  begin
    if FTouchSlot >= 0 then ReportMultiTouch; // if another finger was selected, reporting on that finger is now complete
    FTouchSlot := Slot;
  end;

  procedure MultiTouchSetX(X: Int32);
  begin
    if FTouchSlot < 0 then FTouchSlot := 0;
    FMultiTouch[FTouchSlot].CurrentPosition.X := X;
  end;

  procedure MultiTouchSetY(Y: Int32);
  begin
    if FTouchSlot < 0 then FTouchSlot := 0;
    FMultiTouch[FTouchSlot].CurrentPosition.Y := FScreenHeight - Y;
  end;

  procedure MultiTouchFingerUp;
  begin
    if FTouchSlot < 0 then FTouchSlot := 0;
    FMultiTouch[FTouchSlot].State := tsUp;
  end;

  procedure ReportCompletedEvent;
  begin
    ReportMultiTouch;
    // TODO: others?
  end;

  procedure HandleABS(var Event: TLinuxInputEvent);
  begin
    case Event.Code of
      { ABS_X             : begin WritelnLog('TCastleApplication', 'ABS_X'); FMultiTouch[0].CurrentPosition.X := Event.value; end;
      ABS_Y             : FMultiTouch[0].CurrentPosition.Y := Event.value;
      ABS_RX            : begin WritelnLog('TCastleApplication', 'ABS_RX'); FMultiTouch[0].CurrentPosition.X := Event.value; end;
      ABS_RY            : FMultiTouch[0].CurrentPosition.Y := Event.value; }
      ABS_MT_SLOT       : MultiTouchSelectFinger(Event.value);
      ABS_MT_POSITION_X : MultiTouchSetX(Event.value);
      ABS_MT_POSITION_Y : MultiTouchSetY(Event.value);
      ABS_MT_TRACKING_ID: if Event.value = -1 then MultiTouchFingerUp; // we otherwise ignore the tracking ID for now
      else
        WritelnLog('TCastleApplication', 'Unhandled ABS code: %d', [Event.Code]);
    end;
  end;

  procedure HandleKEY(var Event: TLinuxInputEvent);

    procedure UpdateShiftState(State: TShiftState; Add: Boolean);
    begin
      if Add then
        FShiftState := FShiftState + State
      else
        FShiftState := FShiftState - State;
    end;

  var
    MyKey: TKey;
    MyCharKey: char;
  begin
    //WritelnWarning('TCastleApplication', 'KEY event: code = %d, value = %d', [Event.code, Event.value]);
    case Event.Code of
      BTN_MOUSE..BTN_TASK: ReportMouseButton(Event.Code - BTN_MOUSE, Event.value <> 0);
      else begin
        KeyLinuxToCastle(Event.Code, FShiftState, MyKey, MyCharKey); // TODO: ShiftState!
        if MyKey <> K_None then
        begin
          case MyKey of 
            K_Shift: UpdateShiftState([ssShift], Event.value <> 0);
            K_Alt  : UpdateShiftState([ssAlt  ], Event.value <> 0);
            K_Ctrl : UpdateShiftState([ssCtrl ], Event.value <> 0);
          end;
          if Event.value <> 0 then
            GuessedMainWindow.DoKeyDown(MyKey, MyCharKey)
          else
            GuessedMainWindow.DoKeyUp(MyKey);
        end;
      end;
    end;
  end;

  procedure HandleREL(var Event: TLinuxInputEvent);
  begin
    WritelnLog('TCastleApplication', 'REL: code = %d, value = %d', [Event.code, Event.value]);
    case Event.Code of
      REL_X             : FMultiTouch[0].CurrentPosition.X := FMultiTouch[0].CurrentPosition.X + Event.value;
      REL_Y             : FMultiTouch[0].CurrentPosition.Y := FMultiTouch[0].CurrentPosition.Y + Event.value;
      else
        WritelnLog('TCastleApplication', 'Unhandled REL code: %d', [Event.Code]);
    end;
  end;

  procedure HandleMSC(var Event: TLinuxInputEvent);
  begin
    //WritelnLog('TCastleApplication', 'MSC: code = %d, value = %d', [Event.code, Event.value]);
    case Event.Code of
      9999: ;
      else
        WritelnLog('TCastleApplication', 'Unhandled MSC code: %d', [Event.Code]);
    end;
  end;

  procedure HandleSYN(var Event: TLinuxInputEvent);
  begin
    case Event.Code of
      SYN_REPORT: ReportCompletedEvent;
      SYN_MT_REPORT: WritelnLog('TCastleApplication', 'SYN_MT_REPORT !!');
      // TODO: SYN_DROPPED
    end;
  end;

//const
//  ZeroTime: timeval = (tv_sec: 0; tv_usec: 0);
var
  //FDS: TFDSet;
  Buffer: array[0..1023] of Byte;
  PEvent: ^TLinuxInputEvent;
  Count, Offset: Longint;
  i: Integer;
begin
  //WritelnLog('HandleInputEvents', 'touch file handle: %d', [touch_file.Handle]);
  //fpfd_zero(FDS);
  //fpfd_set(0, FDS);
  //if fpSelect(1, @FDS, nil, nil, @ZeroTime) > 0 then
  //begin
    //WritelnLog('HandleInputEvents', 'input!');
    //Count := touch_file.Read(Buffer, SizeOf(Buffer));
  
  // Read pending input from ALL input devices
  for i := 0 to High(FInputFds) do
  begin
    Count := fpRead(FInputFds[i], Buffer, SizeOf(Buffer));
    Offset := 0;
    while Count > 0 do
    begin
      //WritelnLog('TCastleApplication', 'Got input from input device #%d', [i]);
      PEvent := @Buffer[Offset];
      Offset += SizeOf(TLinuxInputEvent);
      Count -= SizeOf(TLinuxInputEvent);
      //WritelnLog('TCastleApplication_dispmanx.HandleInputEvents', 'Time: %u.%u: %u', [
      //  PEvent^.time.tv_sec, PEvent^.time.tv_usec, PEvent^._type]);
      case PEvent^._type of 
        EV_SYN: HandleSYN(PEvent^);
        EV_ABS: HandleABS(PEvent^);
        EV_KEY: HandleKEY(PEvent^);
        EV_REL: HandleREL(PEvent^);
        // TODO?: EV_SW: ... switches (are there any on Raspberry ?)
        EV_MSC: handleMSC(PEvent^);
        // TODO?: EV_LED: ... ?
        // TODO?: EV_REP: ... auto-repeats
        else
          WritelnLog('TCastleApplication', 'Unhandled event type: %u', [PEvent^._type]);
      end;
    end;
  end;
end;

{ Standard methods }

procedure TCastleApplication.CreateBackend;
begin
  { TODO (initialize backend-specific stuff for the whole application;
    may do nothing if you don't need anything) }
  InitDispmanX;
  OpenAllInputDevices;
end;

procedure TCastleApplication.DestroyBackend;
begin
  CloseAllInputDevices;
  ShutdownDispmanX;
  // DoneKeyboard; NOTE: it may be better not to call this at all, in case the Keyboard unit
  // is used somewhere else.
end;

function TCastleApplication.ProcessMessage(WaitForMessage, WaitToLimitFPS: boolean): boolean;
begin
  Result := not Terminated;

  if Result then
  begin
    HandleInputEvents;
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
  Result := FScreenWidth;
end;

function TCastleApplication.ScreenHeight: integer;
begin
  Result := FScreenHeight;
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
