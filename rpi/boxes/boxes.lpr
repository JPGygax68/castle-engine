{
  Copyright 2008-2018 Michalis Kamburelis.

  This file is part of "Castle Game Engine".

  "Castle Game Engine" is free software; see the file COPYING.txt,
  included in this distribution, for details about the copyright.

  "Castle Game Engine" is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  ----------------------------------------------------------------------------
}

{ Adapted from examples/3d_rendering_process/animate_3d_model_by_code.lpr for the purpose
  of testing performance on the Raspberry Pi. gygax@practicomp.ch }

program Boxes;

uses cthreads, CastleVectors, X3DNodes, CastleWindow, CastleLog,
  CastleUtils, SysUtils, CastleGLUtils, CastleScene, CastleCameras,
  CastleFilesUtils, CastleParameters, CastleStringUtils, CastleKeysMouse,
  CastleApplicationProperties, 
  CastleUIControls, CastleControls, CastleColors, CastleGLShaders,
  X3DLoad;

var
  Window: TCastleWindow;
  Scene: TCastleScene;

const 
  COLUMNS = 8;
  ROWS = 8;
var
  Transforms: array[0 .. ROWS*COLUMNS-1] of TTransformNode;

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

procedure Update(Container: TUIContainer);
var
  i: Integer;
  Tr: TTransformNode;
begin
  for i := 0 to High(Transforms) do
  begin
    Tr := Transforms[i];
    Tr.Rotation := Vector4(Tr.Rotation.XYZ, i * Scene.Time / (High(Transforms) + 1));
  end;
end;

var
  Root: TX3DRootNode;
  Objects: TX3DNode; // Shape: TShapeNode;
  Transform: TTransformNode;
  FPSDisplay: TFPSDisplay;
  row, col, i: Integer;

begin
  //LogShaders := true;
  //
  Parameters.CheckHigh(0);
  ApplicationProperties.OnWarning.Add(@ApplicationProperties.WriteWarningOnConsole);

  Window := TCastleWindow.Create(Application);

  Scene := TCastleScene.Create(nil);
  try
    Root := TX3DRootNode.Create;

    Objects := Load3D(ApplicationData('boxes.x3dv'));
    i := 0;
    for row := 0 to ROWS -1 do
      for col := 0 to COLUMNS - 1 do
      begin
        Transform := TTransformNode.Create;
        Transform.Translation := Vector3(2 * col, 2 * row, 0);
        Transform.AddChildren(Objects as TAbstractChildNode);
        Root.AddChildren(Transform);
        Transforms[i] := Transform;
        Inc(i);
      end;

    Scene.RootNode := Root;

    { init SceneManager with our Scene }
    Window.SceneManager.MainScene := Scene;
    Window.SceneManager.Items.Add(Scene);

    { init SceneManager.Camera }
    Window.SceneManager.ExamineCamera.Init(Scene.BoundingBox, 0.1);

    FPSDisplay := TFPSDisplay.Create(Application);
    Window.Controls.InsertFront(FPSDisplay);

    Window.OnUpdate := @Update;
    Window.SetDemoOptions(K_F11, CharEscape, true);
    Window.OpenAndRun;
  finally Scene.Free end;
end.
