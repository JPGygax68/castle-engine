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

{ Example how to animate (change, modify) the 3D model (it's VRML/X3D graph)
  using ObjectPascal code.
  Run this program without any parameters in this directory
  (it opens file "data/boxes.x3dv") and watch the trivial animation.
  You can rotate/move the scene by dragging with mouse,
  see view3dscene docs (we use the same "Examine" camera).

  For programmers:

  Generally, you just change VRML/X3D graph (rooted in Scene.RootNode)
  however you like, whenever you like. TX3DNode class has a lot of methods
  to find and change nodes within the graph, you can insert/delete/change
  any of their children nodes, fields, and generally do everything.
  Remember to call "Changed" on every changed field (or change it only
  by Send methods, see more info on
  [https://castle-engine.io/vrml_engine_doc/output/xsl/html/section.scene.html#section.scene_caching].)

  Of course, in case of trivial animation in this program, we could
  also express it directly in VRML/X3D and just load the scene,
  setting Scene.ProcessEvents := true. This would make the scene "animate itself",
  without the need for any ObjectPascal code to do this. But, for example sake,
  we animate it here by our own code.
}

program animate_3d_model_by_code;

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
  //TransformBox2, TransformBox3, TransformBox4, TransformBox5, TransformBox6, TransformBox7, TransformBox8, TransformBox9: TTransformNode;
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
  { We want to keep track of current time here (for calculating rotations
    below). It's most natural to just use Scene.Time property for this.
    (Scene.Time is already incremented for us by SceneManager.) }

  { change rotation angles (4th component of the vector),
    leaving the rotation axis (XYZ components) unchanged. }

  //TransformBox2.Rotation := Vector4(TransformBox2.Rotation.XYZ, Scene.Time);
  //TransformBox3.Rotation := Vector4(TransformBox2.Rotation.XYZ, Scene.Time * 2);
  //TransformBox4.Rotation := Vector4(TransformBox2.Rotation.XYZ, Scene.Time * 4);
  
  for i := 0 to High(Transforms) do
  begin
    Tr := Transforms[i];
    Tr.Rotation := Vector4(Tr.Rotation.XYZ, i * Scene.Time / (High(Transforms) + 1));
  end;
end;

var
  Root: TX3DRootNode;
  Boxes: TX3DNode; // Shape: TShapeNode;
  Transform: TTransformNode;
  FPSDisplay: TFPSDisplay;
  row, col, i: Integer;

begin
  LogShaders := true;

  Window := TCastleWindow.Create(Application);
  //Application.Initialize;

  Parameters.CheckHigh(0);
  ApplicationProperties.OnWarning.Add(@ApplicationProperties.WriteWarningOnConsole);

  Scene := TCastleScene.Create(nil);
  try
    Root := TX3DRootNode.Create;

    //Scene.Load(ApplicationData('boxes.x3dv'));
    //TransformBox2 := Scene.RootNode.FindNodeByName(TTransformNode, 'Box2Transform', true) as TTransformNode;
    //TransformBox3 := Scene.RootNode.FindNodeByName(TTransformNode, 'Box3Transform', true) as TTransformNode;
    //
    Boxes := Load3D(ApplicationData('boxes.x3dv'));
    //Shape := Scene.RootNode.FindNodeByName(TShapeNode, 'Box1', true) as TShapeNode;
    i := 0;
    for row := 0 to ROWS -1 do
      for col := 0 to COLUMNS - 1 do
      begin
        Transform := TTransformNode.Create;
        Transform.Translation := Vector3(col, row, 0);
        Transform.AddChildren(Boxes as TAbstractChildNode);
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
