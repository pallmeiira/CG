#ifndef __P1_h
#define __P1_h

#include "Primitive.h"
#include "Scene.h"
#include "graphics/Application.h"

class P1: public cg::GLWindow
{
public:
  using Base = cg::GLWindow;
  using Program = cg::GLSL::Program;
  using Color = cg::Color;

  P1(int width, int height):
    Base{"cg2018 - P1", width, height},
    _program{"P1"}
  {
    // do nothing
  }

  /// Initialize the app.
  void initialize() override;

  /// Update the GUI.
  void gui() override;

  /// Renders the scene.
  void render() override;

private:
  Program _program;
  Reference<cg::Scene> _scene;
  Reference<cg::SceneObject> _boxA;
  Reference<cg::SceneObject> _boxB;
  Reference<cg::SceneObject> _boxC;
  Reference<cg::SceneObject> _boxD;

  Reference<cg::Primitive> _primitive;
  cg::SceneNode* _current{};
  Color selectedWireframeColor{255, 102, 0};
  mat4f _transform{mat4f::identity()};


  void buildScene();
  void drawHierarchyWindow(cg::SceneObject*, cg::SceneNode*, ImGuiTreeNodeFlags);
  void drawHierarchyWindow(cg::Scene*, cg::SceneNode*, ImGuiTreeNodeFlags);
  void drawPrimitives(cg::SceneObject*);
  void hierarchyWindow();
  void inspectorWindow();
  void sceneGui();
  void showComponents(cg::SceneObject* node);
  void sceneObjectGui();
  void objectGui();

}; // P1

#endif // __P1_h
