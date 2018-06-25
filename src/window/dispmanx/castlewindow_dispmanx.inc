{
  Copyright 2004-2018 Michalis Kamburelis, Jean-Pierre Gygax

  This file is part of "Castle Game Engine".

  "Castle Game Engine" is free software; see the file COPYING.txt,
  included in this distribution, for details about the copyright.

  "Castle Game Engine" is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  ----------------------------------------------------------------------------
}

{ Wiki page: https://github.com/castle-engine/castle-engine/wiki/DispmanX-backend }

{$ifdef read_interface_uses}
// The following two imports do not really belong in the interface part, but 
// must be here because cthreads must be initialized early.
{$ifdef unix}
  //cthreads,
  pthreads,
  //cmem, // the c memory manager is on some systems much faster for multi-threading
{$endif}
CastleInternalDispmanX, CastleInternalVC4,
{$endif}

{$ifdef read_implementation_uses}
BaseUnix, CastleInternalLinuxEvents, CastleInternalLinuxUtils,
{$endif}

{$ifdef read_window_interface}
private
  Display: EGLDisplay;
  Surface: EGLSurface;
  Context: EGLContext;
  Config : EGLConfig;
  NativeWindow: EGL_DISPMANX_WINDOW_T;
  DispmanDisplay: DISPMANX_DISPLAY_HANDLE_T;

  procedure InitDispmanX;
  procedure CleanupDispmanX;
  procedure InitEGL;
  procedure CleanupEGL;
  procedure EGLFromDispmanX;
{$endif read_window_interface}

{$ifdef read_application_interface}
type 
  TTouchState = (tsInactive = 0, tsDown = 1, tsUp = 2);
  TTouch = record
    State: TTouchState;
    LastPosition, CurrentPosition: TVector2;
  end;
  TMultiTouch = array[0..4] of TTouch;

private
  // TODO: support multiple screens!
  FScreenWidth, FScreenHeight: UInt32;
  FInputFds: array of Longint;
  FTouchSlotNum: Integer;
  FMultiTouch: TMultiTouch;
  FShiftState: TShiftState;

  procedure InitDispmanX;
  procedure ShutdownDispmanX;
  function  RequiredMainWindow: TCastleWindowCustom;
  procedure OpenAllInputDevices;
  procedure CloseAllInputDevices;
  procedure HandleInputEvents;
{$endif read_application_interface}

{$ifdef read_implementation}

{ TCastleWindowCustom ------------------------------------------------------------------ }

{ DispmanX/Raspberry Pi specific methods }

procedure TCastleWindowCustom.InitDispmanX;
var
  Element: DISPMANX_ELEMENT_HANDLE_T;
  Update: DISPMANX_UPDATE_HANDLE_T;
  SrcRect, DstRect: VC_RECT_T;
begin

  WritelnLog('CastleWindowCustom', 'Opening DispmanX Display...');
  DispmanDisplay := vc_dispmanx_display_open(0 {LCD});
  if DispmanDisplay = 0 then raise Exception.Create('Failed to open dispmanx Display');
  WritelnLog('CastleWindowCustom', 'done.');

  WritelnLog('CastleWindowCustom', 'Creating DispmanX window from Display...');
  DstRect.x := 0;
  DstRect.y := 0;
  DstRect.width  := Application.ScreenWidth;
  DstRect.height := Application.ScreenHeight;
  WritelnLog('CastleWindowCustom', 'screen size: %dx%d', [Application.ScreenWidth, Application.ScreenHeight]);
  SrcRect.x := 0;
  SrcRect.y := 0;
  SrcRect.width  := Application.ScreenWidth  shl 16;
  SrcRect.height := Application.ScreenHeight shl 16;
  Update := vc_dispmanx_update_start(0);
  if Update = 0 then raise Exception.Create('Failed to obtain update handle');
  Element := vc_dispmanx_element_add(Update, DispmanDisplay, 
    0 {layer}, @DstRect, 0 {source}, @SrcRect, DISPMANX_PROTECTION_NONE, 
    nil {alpha}, nil {clamp}, 0 {transform});
  if Element = 0 then raise Exception.Create('Failed to add Dispman element');
  NativeWindow.element := Element;
  NativeWindow.width  := Application.ScreenWidth;
  NativeWindow.height := Application.ScreenHeight;
  vc_dispmanx_update_submit_sync(Update);
  vc_dispmanx_element_remove(Update, Element);

  WritelnLog('CastleWindowCustom', 'done.');
end;

procedure TCastleWindowCustom.CleanupDispmanX;
begin
  WritelnLog('CastleWindowCustom', 'Closing DispmanX Display...');
  if vc_dispmanx_display_close(DispmanDisplay) <> 0 then WritelnWarning('TCastleWindowCustom', 'Error');
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
  NumConfigs: EGLint;
  {$ifdef NOT_DEFINED}
  Configs: array of EGLConfig;
  {$endif}
begin
  WritelnLog('TCastleWindowCustom', 'Obtaining EGL default Display...');
  Display := eglGetDisplay(EGL_DEFAULT_DISPLAY);
  if Display = nil then raise Exception.Create('Failed to obtain EGL default Display');
  WritelnLog('TCastleWindowCustom', 'done.');

  // eglQueryString() only return nil pointers
  {$ifdef NOT_DEFINED}
  WritelnLog('TCastleWindowCustom', 'Dumping Display information...');
  WritelnLog('TCastleWindowCustom', 'APIs      : ' + StrPas(eglQueryString(Display, EGL_CLIENT_APIS)));
  WritelnLog('TCastleWindowCustom', 'Vendor    : ' + StrPas(eglQueryString(Display, EGL_VENDOR)     ));
  WritelnLog('TCastleWindowCustom', 'Version   : ' + StrPas(eglQueryString(Display, EGL_VERSION)    ));
  WritelnLog('TCastleWindowCustom', 'Extensions: ' + StrPas(eglQueryString(Display, EGL_EXTENSIONS) ));
  WritelnLog('TCastleWindowCustom', 'Display info dumped.');
  {$endif}

  WritelnLog('TCastleWindowCustom', 'Initializing EGL Display connection...');
  Res := eglInitialize(Display, @Major, @Minor);
  if Res = EGL_FALSE then raise Exception.Create('Failed to initialize the EGL Display');
  WritelnLog('TCastleWindowCustom', 'done, EGL version is %d.%d', [Major, Minor]);

  {$ifdef NOT_DEFINED}
  WritelnLog('TCastleWindowCustom', 'Obtaining EGL Display Configs...');
  eglGetConfigs(Display, nil, 0, @NumConfigs);
  // TODO: abort if NumConfigs = 0
  SetLength(Configs, NumConfigs);
  eglGetConfigs(Display, @Configs, NumConfigs, @NumConfigs);
  WritelnLog('TCastleWindowCustom', 'obtained %d Configs', [NumConfigs]);
  {$endif}

  WritelnLog('TCastleWindowCustom', 'Choosing a Display Config...');
  Res := eglChooseConfig(Display, @config_attribs, @Config, 1, @NumConfigs);
  if Res <> EGL_TRUE then raise Exception.Create('Failed to choose a Display configuration');
  WritelnLog('TCastleWindowCustom', 'done.');

  WritelnLog('TCastleWindowCustom', 'Binding the OpenGL ES API...');
  Res := eglBindAPI(EGL_OPENGL_ES_API);
  if Res <> EGL_TRUE then raise Exception.Create('Failed to bind OpenGL ES API');
  WritelnLog('TCastleWindowCustom', 'done.');

  WritelnLog('TCastleWindowCustom', 'Creating EGL Context...');
  Context := eglCreateContext(Display, Config, EGL_NO_CONTEXT, @context_attribs);
  if Context = nil then raise Exception.Create('Failed to create EGL Context');
  WritelnLog('TCastleWindowCustom', 'done.');
end;

procedure TCastleWindowCustom.EGLFromDispmanX;
var
  Res: EGLboolean;
begin
  WritelnLog('TCastleWindowCustom', 'Creating a Surface for the native Window...');
  // Note: egl.pas defines EGLNativeWindowType = Pointer
  Surface := eglCreateWindowSurface(Display, Config, @NativeWindow, nil);
  if Surface = EGL_NO_SURFACE then raise Exception.Create('Failed to create Window Surface');
  WritelnLog('TCastleWindowCustom', 'done.');

  WritelnLog('TCastleWindowCustom', 'Connecting the rendering Context to the Surface...');
  Res := eglMakeCurrent(Display, Surface, Surface, Context);
  if Res = EGL_FALSE then raise Exception.Create('Failed to connect the rendering Context to the Surface');
  WritelnLog('TCastleWindowCustom', 'done.');

  // TODO: make this dependent on TCastleWindow.VerticalSync if/when michalis decides to expose that
  eglSwapInterval(Display, 1);  // 0 = no vsync, 1 = wait for next vsync, 2 = wait for next + 1, etc.
end;

procedure TCastleWindowCustom.CleanupEGL;
begin
  WritelnLog('TCastleWindowCustom', 'Cleaning up EGL');
  if (Surface <> EGL_NO_SURFACE) and (eglDestroySurface(Display, Surface) <> 0) then
  begin
    WritelnLog('TCastleWindowCustom', 'EGL Surface destroyed ok');
    Surface := EGL_NO_SURFACE;
  end;
  if (Context <> EGL_NO_CONTEXT) and (eglDestroyContext(Display, Context) <> 0) then
  begin
    WritelnLog('TCastleWindowCustom', 'EGL Context destroyed ok');
    Context := EGL_NO_CONTEXT;
  end;
  if (Display <> EGL_NO_DISPLAY) and (eglTerminate(Display) <> 0) then
  begin
    WritelnLog('TCastleWindowCustom', 'EGL Display terminated ok');
    Display := EGL_NO_DISPLAY;
  end;
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
    may do MenuFinalize + MenuInitialize if you Display menu,
    but don't care about updating it fast) }
  MenuFinalize;
  MenuInitialize;
end;

procedure TCastleWindowCustom.MenuUpdateEnabled(Entry: TMenuEntryWithCaption);
begin
  { TODO (may do nothing if you do not care about displaying menu;
    may do MenuFinalize + MenuInitialize if you Display menu,
    but don't care about updating it fast) }
  MenuFinalize;
  MenuInitialize;
end;

procedure TCastleWindowCustom.MenuUpdateChecked(Entry: TMenuItemChecked);
begin
  { TODO (may do nothing if you do not care about displaying menu;
    may do MenuFinalize + MenuInitialize if you Display menu,
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
    may do MenuFinalize + MenuInitialize if you Display menu,
    but don't care about updating it fast) }
  MenuFinalize;
  MenuInitialize;
end;

procedure TCastleWindowCustom.MenuDelete(const Parent: TMenu;
  const ParentPosition: Integer; const Entry: TMenuEntry);
begin
  { TODO (may do nothing if you do not care about displaying menu;
    may do MenuFinalize + MenuInitialize if you Display menu,
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

  Assert(Application.MainWindow = nil);
  Application.MainWindow := Self;
end;

procedure TCastleWindowCustom.CloseBackend;
begin
  { TODO (destroy OpenGL Context, hide window; counterpart to OpenBackend) }
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
    WritelnWarning('TCastleWindowCustom', 'Cannot make Context current: ');
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
  OpenGL Context using our CastleMessages unit. }

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

function TCastleApplication.RequiredMainWindow: TCastleWindowCustom;
begin
  if MainWindow <> nil then
    Result := MainWindow
  else
    raise Exception.Create('For the DispmanX backend on Raspberry Pi, you have to assign Application.MainWindow');
end;

procedure TCastleApplication.InitDispmanX;
var
  Success: Int32;
begin
  BCMHostInit;
  WritelnLog('TCastleApplication', 'BCM host initialized.');
  Success := BCMHostGraphicsGetDisplaySize(0 {LCD}, FScreenWidth, FScreenHeight);
  if Success < 0 then raise Exception.Create('Failed to obtain LCD Display size'); // TODO: custom exception class ?
  WritelnLog('TCastleApplication', 'LCD Display size obtained: %dx%d', [FScreenWidth, FScreenHeight]);
end;

procedure TCastleApplication.ShutdownDispmanX;
begin
  BCMHostDeinit;
  WritelnLog('TCastleApplication_dispmanx.ShutdownDispmanX', 'BCM host deinitialized');
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
  FTouchSlotNum := -1;

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

var  
  W: TCastleWindowCustom;

  procedure ReportMouseButton(Button: TMouseButton; Down: Boolean);
  var
    Position: TVector2;
  begin
    //WritelnLog('TCastleApplication', 'ReportMouseButton(%d, %d)', [BtnNum, Down]);
    Position := FMultiTouch[0].CurrentPosition;
    if Down then
      W.DoMouseDown(Position, Button, 0)
    else
      W.DoMouseUp  (Position, Button, 0);
  end;

  procedure ReportMultiTouch;
  var
    S: ^TTouch;
  begin
    if FTouchSlotNum >= 0 then
    begin
      WritelnLog('TCastleApplication', 'ReportMultiTouch, slot = %d', [FTouchSlotNum]);
      S := @FMultiTouch[FTouchSlotNum];
      case S^.State of
        tsInactive: // "finger down" is implicit when first position is received
          begin
            S^.LastPosition := S^.CurrentPosition; // TODO: correct ? reporting same position twice
            //WritelnLog('TCastleApplication', 'MouseDown: %f, %f, finger %d', [CurrentPosition.X, CurrentPosition.Y, FTouchSlotNum]);
            W.DoMouseDown(S^.CurrentPosition, CastleKeysMouse.mbLeft, FTouchSlotNum);
            S^.State := tsDown;
            S^.LastPosition := S^.CurrentPosition;
          end;
        tsDown:
          begin
            //WritelnLog('TCastleApplication', 'MouseMotion: %f, %f -> %f, %f, finger %d', [LastPosition.X, LastPosition.Y, 
            //  CurrentPosition.X, CurrentPosition.Y, FTouchSlotNum]);
            if W.MousePressed <> [mbLeft] then
              WriteLnWarning ('Motion with touch input, but MousePressed is not [mbLeft]. This either indicates you have both mouse and touch input simultaneously, or we have a bug.');
            W.DoMotion(InputMotion(S^.LastPosition, S^.CurrentPosition, [CastleKeysMouse.mbLeft], FTouchSlotNum));
            S^.LastPosition := S^.CurrentPosition;
          end;
        tsUp: // "finger up" was registered via a tracking id of -1
          begin
            //WritelnLog('TCastleApplication', 'MouseUp: %f, %f, finger %d', [CurrentPosition.X, CurrentPosition.Y, FTouchSlotNum]);
            W.DoMouseUp(S^.CurrentPosition, CastleKeysMouse.mbLeft, FTouchSlotNum);
            S^.State := tsInactive;
          end;
      end;
      FTouchSlotNum := -1;
    end;
  end;

  procedure MultiTouchSelectFinger(Slot: Longint);
  begin
    if FTouchSlotNum >= 0 then ReportMultiTouch; // if another finger was selected, reporting on that finger is now complete
    FTouchSlotNum := Slot;
  end;

  procedure MultiTouchSetX(X: Int32);
  begin
    if FTouchSlotNum < 0 then FTouchSlotNum := 0;
    FMultiTouch[FTouchSlotNum].CurrentPosition.X := X;
  end;

  procedure MultiTouchSetY(Y: Int32);
  begin
    if FTouchSlotNum < 0 then FTouchSlotNum := 0;
    FMultiTouch[FTouchSlotNum].CurrentPosition.Y := FScreenHeight - 1 - Y;
  end;

  procedure MultiTouchFingerUp;
  begin
    WritelnLog('TCastleApplication', 'MultiTouchFingerUp, slot = %d', [FTouchSlotNum]);
    if FTouchSlotNum < 0 then FTouchSlotNum := 0;
    FMultiTouch[FTouchSlotNum].State := tsUp;
  end;

  procedure ReportCompletedEvent;
  begin
    ReportMultiTouch;
    // TODO: others?
  end;

  procedure HandleABS(var Event: TLinuxInputEvent);
  begin
    WritelnLog('TCastleApplication', 'ABS code: %d', [Event.Code]);
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
      BTN_MOUSE  : ReportMouseButton(mbLeft  , Event.value <> 0);
      BTN_RIGHT  : ReportMouseButton(mbRight , Event.value <> 0);
      BTN_MIDDLE : ReportMouseButton(mbMiddle, Event.value <> 0);
      BTN_SIDE   : ReportMouseButton(mbExtra1, Event.value <> 0);
      BTN_EXTRA  : ReportMouseButton(mbExtra2, Event.value <> 0);
      BTN_FORWARD: WritelnWarning('TCastleApplication', 'Linux mouse button BTN_FORWARD cannot be mapped to CGE button');
      BTN_BACK   : WritelnWarning('TCastleApplication', 'Linux mouse button BTN_BACK cannot be mapped to CGE button');
      BTN_TASK   : WritelnWarning('TCastleApplication', 'Linux mouse button BTN_TASK cannot be mapped to CGE button');
      else
      begin
        KeyLinuxToCastle(Event.Code, FShiftState, MyKey, MyCharKey);
        if MyKey <> K_None then
        begin
          case MyKey of 
            K_Shift: UpdateShiftState([ssShift], Event.value <> 0);
            K_Alt  : UpdateShiftState([ssAlt  ], Event.value <> 0);
            K_Ctrl : UpdateShiftState([ssCtrl ], Event.value <> 0);
          end;
          if Event.value <> 0 then
            W.DoKeyDown(MyKey, MyCharKey)
          else
            W.DoKeyUp(MyKey);
        end;
      end;
    end;
  end;

  procedure HandleREL(var Event: TLinuxInputEvent);
  begin
    WritelnLog('TCastleApplication', 'REL: code = %d, value = %d', [Event.code, Event.value]);
    case Event.Code of
      REL_X     : FMultiTouch[0].CurrentPosition.X := FMultiTouch[0].CurrentPosition.X + Event.value;
      REL_Y     : FMultiTouch[0].CurrentPosition.Y := FMultiTouch[0].CurrentPosition.Y + Event.value;
      REL_WHEEL : RequiredMainWindow.DoMouseWheel(Event.value, true);
      REL_HWHEEL: RequiredMainWindow.DoMouseWheel(Event.value, false);
      else
        WritelnLog('TCastleApplication', 'Unhandled REL code: %d', [Event.Code]);
    end;
  end;

  procedure HandleMSC(var Event: TLinuxInputEvent);
  begin
    //WritelnLog('TCastleApplication', 'MSC: code = %d, value = %d', [Event.code, Event.value]);
    //case Event.Code of
    //  else
        WritelnLog('TCastleApplication', 'Unhandled MSC code: %d', [Event.Code]);
    //end;
  end;

  procedure HandleSYN(var Event: TLinuxInputEvent);
  begin
    case Event.Code of
      SYN_REPORT: ReportCompletedEvent;
      SYN_MT_REPORT: WritelnLog('TCastleApplication', 'SYN_MT_REPORT received - this is deprecated and should not happen');
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
  
  W := RequiredMainWindow;

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
  InitDispmanX;
  OpenAllInputDevices;
end;

procedure TCastleApplication.DestroyBackend;
begin
  CloseAllInputDevices;
  ShutdownDispmanX;
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
  Result := 'CastleWindow DispmanX (Raspberry Pi without X) backend';
end;

{ TCastleWindow -------------------------------------------------------------- }

procedure TCastleWindow.NavigationInfoChanged;
begin
end;

{$endif read_implementation}
