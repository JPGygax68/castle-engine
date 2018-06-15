program Test1;

uses cthreads, CastleVectors, X3DNodes, CastleWindow, CastleLog,
  CastleUtils, SysUtils, CastleGLUtils, CastleScene, CastleCameras,
  CastleFilesUtils, CastleParameters, CastleStringUtils, CastleKeysMouse,
  CastleApplicationProperties, 
  CastleUIControls, CastleControls, CastleColors,
  X3DLoad;

const 
  librasperf3d = 'rasperf3d';

{$linklib 'rasperf3d'}

function rp_InitModel: Integer; cdecl; external librasperf3d name 'rp_InitModel';

var
  Window: TCastleWindow;
  Scene: TCastleScene;

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

procedure OpenWindow(Container: TUIContainer);
begin
  rp_InitModel;
end;

procedure Update(Container: TUIContainer);
begin
end;

var
  Root: TX3DRootNode;
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

    FPSDisplay := TFPSDisplay.Create(Application);
    Window.Controls.InsertFront(FPSDisplay);

    Window.OnOpen := @OpenWindow;
    Window.OnUpdate := @Update;
    Window.SetDemoOptions(K_F11, CharEscape, true);
    Window.OpenAndRun;
  finally Scene.Free end;
end.
