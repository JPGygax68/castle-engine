{
  Copyright 2018-2018 Michalis Kamburelis.

  This file is part of "Castle Game Engine".

  "Castle Game Engine" is free software; see the file COPYING.txt,
  included in this distribution, for details about the copyright.

  "Castle Game Engine" is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

  ----------------------------------------------------------------------------
}

{ Demo showing how to visualize a rectangle in 2D.
  In paricualar, a bounding rectangle of a scene in 2D. }

uses CastleWindow, Castle2DSceneManager, X3DNodes, CastleRectangles,
  CastleColors, CastleVectors, CastleControls, CastleSceneCore;

procedure UpdateRectangleCoords(const Rect: TFloatRectangle;
  const RectCoords: TCoordinateNode); forward;

{ Build X3D node showing a wireframe rectangle.

  There are a lot of things you can customize here,
  adjusting the look of the resulting rectangle.

  Note: instead of TLineSetNode, one could also use TRectangle2DNode
  to show a rectangle in 2D. You would need to set TRectangle2DNode.Size,
  and place it within TTransformNode to position is correctly.
  Below we show a little different, but also more flexible, approach
  that uses TLineSetNode and TCoordinateNode. }
function CreateRectangleNode(const Rect: TFloatRectangle;
  const Color: TCastleColorRGB; const LineWidth: Single;
  out RectCoords: TCoordinateNode): TAbstractChildNode;
var
  Shape: TShapeNode;
  LineSet: TLineSetNode;
  LineProperties: TLinePropertiesNode;
begin
  RectCoords := TCoordinateNode.Create;
  UpdateRectangleCoords(Rect, RectCoords);

  LineSet := TLineSetNode.CreateWithShape(Shape);
  LineSet.Coord := RectCoords;
  LineSet.SetVertexCount([RectCoords.FdPoint.Count]);

  Shape.Material := TMaterialNode.Create;
  Shape.Material.EmissiveColor := Color;
  //Shape.Material.Transparency := 0.8;

  if LineWidth <> 1 then
  begin
    LineProperties := TLinePropertiesNode.Create;
    LineProperties.LineWidthScaleFactor := LineWidth;
    Shape.Appearance.LineProperties := LineProperties;
  end;

  Result := Shape;
end;

{ Update RectCoords (created by CreateRectangleNode). }
procedure UpdateRectangleCoords(const Rect: TFloatRectangle;
  const RectCoords: TCoordinateNode);
begin
  RectCoords.SetPoint([
    Vector3(Rect.Left , Rect.Bottom, 0),
    Vector3(Rect.Right, Rect.Bottom, 0),
    Vector3(Rect.Right, Rect.Top   , 0),
    Vector3(Rect.Left , Rect.Top   , 0),
    Vector3(Rect.Left , Rect.Bottom, 0)
  ]);
end;

var
  Window: TCastleWindowCustom;
  SceneManager: TCastle2DSceneManager;
  Scene: TCastle2DScene;
  SceneDebugVisualization: TCastle2DScene;
  RectCoords: TCoordinateNode;
  SceneDebugVisualizationRoot: TX3DRootNode;

{ Optionally update the displayed rectangle,
  if the bounding box of your Scene may change every frame. }
procedure WindowUpdate(Container: TUIContainer);
begin
  UpdateRectangleCoords(Scene.BoundingBox.RectangleXY, RectCoords);
end;

begin
  Window := TCastleWindowCustom.Create(Application);
  Window.Open;

  SceneManager := TCastle2DSceneManager.Create(Application);
  SceneManager.FullSize := true;
  SceneManager.ProjectionAutoSize := false;
  SceneManager.ProjectionHeight := 6000;
  SceneManager.ProjectionOriginCenter := true;
  Window.Controls.InsertFront(SceneManager);

  Scene := TCastle2DScene.Create(Application);
  Scene.Load('../2d_dragon_spine_game/data/dragon/dragon.json');
  Scene.ProcessEvents := true;
  Scene.PlayAnimation('flying', true);
  SceneManager.Items.Add(Scene);

  SceneDebugVisualizationRoot := TX3DRootNode.Create;
  SceneDebugVisualizationRoot.AddChildren(CreateRectangleNode(
    Scene.BoundingBox.RectangleXY, YellowRGB, 2, RectCoords));

  SceneDebugVisualization := TCastle2DScene.Create(Application);
  SceneDebugVisualization.Load(SceneDebugVisualizationRoot, true);
  SceneManager.Items.Add(SceneDebugVisualization);

  { Note 1: instead of creating a new SceneDebugVisualization,
    you could add the rectangle to the main Scene, by doing

      Scene.RootNode.AddChildren(CreateRectangleNode(...));

    Then you don't need separate SceneDebugVisualization at all.
    It's *usually* more comfortable to keep debug things in a separate scene,
    and this way you know that changing the SceneDebugVisualization
    does not cause any additional processing for Scene (although it is optimized
    anyway)... but it's really up to you and your use-case.

    Note 2: if you plan to move the scene, you could place
    the SceneDebugVisualization as child of Scene, by doing

      Scene.Add(SceneDebugVisualization);

    instead of

      SceneManager.Items.Add(SceneDebugVisualization);

    Transforming the Scene (changing Scene.Translation, Scene.Rotation,
    Scene.Scale...) will then transform the SceneDebugVisualization as well.
    Of course, make sure in this case that Rect is in local coordinates,
    relative to the Scene transformation.

    Note 3: you can easily hide the debug rectangle, by

      SceneDebugVisualization.Exists := false;

    This can be done at any moment during the game, at no cost.
    Alternatively, you could save the TShapeNode reference (created by
    CreateRectangleNode function) and change TShapeNode.Render boolean.
  }

  { This is optional, do this if you want to continously update the displayed
    rectangle. }
  Window.OnUpdate := @WindowUpdate;

  Application.Run;
end.
