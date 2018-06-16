program Test1;

uses cthreads, CastleVectors, X3DNodes, CastleWindow, CastleLog,
  CastleUtils, SysUtils, CastleGLUtils, CastleScene, CastleCameras,
  CastleFilesUtils, CastleParameters, CastleStringUtils, CastleKeysMouse,
  CastleApplicationProperties, 
  CastleUIControls, CastleControls, CastleColors,
  X3DLoad, CTypes;

const 
  librasperf3d = 'rasperf3d';

{$linklib 'rasperf3d'}

function rp_InitModel: Integer; cdecl; external librasperf3d name 'rp_InitModel';
function rp_Render: Integer; cdecl; external librasperf3d name 'rp_Render';
function rp_SetFramebufferSize(w, h: cuint32): Integer; cdecl; external librasperf3d name 'rp_SetFramebufferSize';
function rp_SetDefaultSettings: Integer; cdecl; external librasperf3d name 'rp_SetDefaultSettings';

var
  Window: TCastleWindow;
  Scene: TCastleScene;

type
  TRasperf3d = class(TUIControl)
  public
    procedure Render; override;
  end;

procedure TRasperf3d.Render;
begin
  inherited;
  if rp_Render < 0 then raise Exception.Create('Error trying to render Rasperf3d scene');
end;

{ Framerate display }
type
  TFPSDisplay = class(TUIControl)
  public
    procedure Render; override;
  end;

procedure TFPSDisplay.Render;
begin
  inherited;
  UIFont.PrintRect(Window.Rect.Grow(-8), Red,
    'FPS: ' + Window.Fps.ToString, hpRight, vpTop);
end;

{ Main rendering}
procedure OpenWindow(Container: TUIContainer);
begin
  if rp_InitModel < 0 then raise Exception.Create('Error trying to initialize models');
  rp_SetFramebufferSize(Container.Width, Container.Height);
  rp_SetDefaultSettings;
end;

procedure Update(Container: TUIContainer);
begin
end;

procedure Render(Container: TUIContainer);
begin
end;

var
  Root: TX3DRootNode;
  Perf3dControl: TRasperf3d;
  FPSDisplay: TFPSDisplay;

begin
  Window := TCastleWindow.Create(Application);
  //Application.Initialize;

  Parameters.CheckHigh(0);
  ApplicationProperties.OnWarning.Add(@ApplicationProperties.WriteWarningOnConsole);

  Scene := TCastleScene.Create(nil);
  try
    Root := TX3DRootNode.Create;

    Scene.RootNode := Root;

    { init SceneManager with our Scene }
    Window.SceneManager.MainScene := Scene;
    Window.SceneManager.Items.Add(Scene);

    { init SceneManager.Camera }
    Window.SceneManager.ExamineCamera.Init(Scene.BoundingBox, 0.1);

    Perf3dControl := TRasperf3d.Create(Application);
    Window.Controls.InsertFront(Perf3dControl);
    FPSDisplay := TFPSDisplay.Create(Application);
    Window.Controls.InsertFront(FPSDisplay);

    Window.OnOpen := @OpenWindow;
    //Window.OnUpdate := @Update;
    //Window.OnRender := @Render;
    Window.SetDemoOptions(K_F11, CharEscape, true);
    Window.OpenAndRun;
  finally Scene.Free end;
end.
