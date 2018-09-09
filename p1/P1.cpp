#include "P1.h"
#include <iostream>
namespace cg
{ // begin namespace cg

inline Primitive*
makeBoxMesh()
{
  const vec4f p1{-0.5, -0.5, -0.5, 1};
  const vec4f p2{+0.5, -0.5, -0.5, 1};
  const vec4f p3{+0.5, +0.5, -0.5, 1};
  const vec4f p4{-0.5, +0.5, -0.5, 1};
  const vec4f p5{-0.5, -0.5, +0.5, 1};
  const vec4f p6{+0.5, -0.5, +0.5, 1};
  const vec4f p7{+0.5, +0.5, +0.5, 1};
  const vec4f p8{-0.5, +0.5, +0.5, 1};
  const Color c1{Color::black};
  const Color c2{Color::red};
  const Color c3{Color::yellow};
  const Color c4{Color::green};
  const Color c5{Color::blue};
  const Color c6{Color::magenta};
  const Color c7{Color::cyan};
  const Color c8{Color::white};

  // Box vertices
  static const vec4f v[]
  {
    p1, p5, p8, p4, // x = -0.5
    p2, p3, p7, p6, // x = +0.5
    p1, p2, p6, p5, // y = -0.5
    p4, p8, p7, p3, // y = +0.5
    p1, p4, p3, p2, // z = -0.5
    p5, p6, p7, p8  // z = +0.5
  };

  // Box vertex colors
  static const Color c[]
  {
    c1, c5, c8, c4, // x = -0.5
    c2, c3, c7, c6, // x = +0.5
    c1, c2, c6, c5, // y = -0.5
    c4, c8, c7, c3, // y = +0.5
    c1, c4, c3, c2, // z = -0.5
    c5, c6, c7, c8  // z = +0.5
  };

  // Box triangles
  static const GLMeshArray::Triangle t[]
  {
    { 0,  1,  2}, { 2,  3,  0},
    { 4,  5,  7}, { 5,  6,  7},
    { 8,  9, 11}, { 9, 10, 11},
    {12, 13, 14}, {14, 15, 12},
    {16, 17, 19}, {17, 18, 19},
    {20, 21, 22}, {22, 23, 20}
  };

  return new Primitive{new GLMeshArray{24, {v, 0}, {c, 1}, 12, t}};
}

} // end namespace cg

inline void
P1::buildScene()
{
  _primitive = cg::makeBoxMesh();
  _current = _scene = new cg::Scene{"Scene 1"};
  
  _boxA = new cg::SceneObject{ "Box A", _scene, {new cg::SceneObject{ "Box E", _scene, {}, {cg::makeBoxMesh()} }}, {cg::makeBoxMesh()} };
  _boxB = new cg::SceneObject{ "Box B", _scene, {}, {cg::makeBoxMesh()} };
  _boxC = new cg::SceneObject{ "Box C", _scene, {}, {cg::makeBoxMesh()} };
//  _boxD = new cg::SceneObject{ "Box D", _scene, {}, {cg::makeBoxMesh()} };

  _scene->root->filhos.push_back(_boxA); 
  _boxA->filhos.push_back(_boxB);
  _boxB->filhos.push_back(_boxC);
 // _boxC->filhos.push_back(_boxD);
  delete(_boxD);
  /*   Hierarquia criada :
			Scene (Root)
			  |
			 BoxA -----+
			  |		   |
			 BoxB    BoxE
			  |
			 BoxC
			  | 
			 BoxD
  
  */
 
}

void
P1::initialize()
{
  cg::Application::loadShaders(_program, "p1.vs", "p1.fs");
  buildScene();
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1.0f, 1.0f);
  glLineWidth(2.0f);
  glEnable(GL_LINE_SMOOTH);
  _program.use();
}

namespace ImGui
{
  void ShowDemoWindow(bool*);
}

inline void
P1::hierarchyWindow()
{
  ImGui::Begin("Hierarchy");
  
  /* Protótipo da função para capturar o clique do mouse
  if (ImGui::GetIO().MouseClicked[1])
	  std::cout << "Oi" << std::endl;
  */
  if (ImGui::Button("Create###object"))
	  ImGui::OpenPopup("CreateObjectPopup");
	  if (ImGui::BeginPopup("CreateObjectPopup"))
	  {
		  if (ImGui::MenuItem("Empty Object"))
		  {
			  if (dynamic_cast<cg::SceneObject*>(_current)) {
				  dynamic_cast<cg::SceneObject*>(_current)->novoFilho("Object ");
			  }
			  else {
				  dynamic_cast<cg::Scene*>(_current)->root->novoFilho("Object ");
			  }
		  }

		  if (ImGui::BeginMenu("3D Object"))
		  {
			  if (ImGui::MenuItem("Box"))
			  {
				  if (dynamic_cast<cg::SceneObject*>(_current)) {
					  dynamic_cast<cg::SceneObject*>(_current)->novoFilho("Box ", cg::makeBoxMesh());
				  }
				  else {
					  dynamic_cast<cg::Scene*>(_current)->root->novoFilho("Box ", cg::makeBoxMesh());
				  }

			  }
			  ImGui::EndMenu();
		  }
		  ImGui::EndPopup();
	  }
	  ImGui::SameLine();
  if(ImGui::Button("Delete -- NOTWORKING###Object")) {
	  std::cout << "Cliquei no Botão Delete()" << std::endl;
  }
  ImGui::Separator();

  ImGuiTreeNodeFlags flag{ ImGuiTreeNodeFlags_OpenOnArrow };
  drawHierarchyWindow(_scene, _current, flag);	 
  ImGui::End();


}

void P1::drawHierarchyWindow(cg::SceneObject* node, cg::SceneNode* current, ImGuiTreeNodeFlags flag) {
	if (node->filhos.size() > 0) {
		auto open = ImGui::TreeNodeEx(node, _current == node ? flag | ImGuiTreeNodeFlags_Selected : flag, node->name());
		if (ImGui::IsItemClicked())
			_current = node;
		if (ImGui::IsItemClicked(1))
			ImGui::OpenPopup("RightClickPopup");
		if (ImGui::BeginPopup("RightClickPopup"))
		{
			if (ImGui::MenuItem(node->name()))
			{
				
			}			
			ImGui::EndPopup();
		}
		if (open) {
			for (auto &c : node->filhos) {
				drawHierarchyWindow(c, _current, flag);
			}
			ImGui::TreePop();
		}
	}
	else {
		flag |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		ImGui::TreeNodeEx(node, _current == node ? flag | ImGuiTreeNodeFlags_Selected : flag, node->name());
		if (ImGui::IsItemClicked())
			_current = node;		
	}
}

void P1::drawHierarchyWindow(cg::Scene* node, cg::SceneNode* current, ImGuiTreeNodeFlags flag) {
	if (node->root->filhos.size() > 0) {
		auto open = ImGui::TreeNodeEx(node, _current == node ? flag | ImGuiTreeNodeFlags_Selected : flag, node->name());
		if (ImGui::IsItemClicked())
			_current = node;
		if (ImGui::IsItemClicked(1))
			ImGui::OpenPopup("RightClickPopup");
		if (ImGui::BeginPopup("RightClickPopup"))
		{
			if (ImGui::MenuItem(node->root->name()))
			{

			}
			ImGui::EndPopup();

		}
		if (open) {
			for (auto &c : node->root->filhos) {
				drawHierarchyWindow(c, _current, flag);
			}
			ImGui::TreePop();
		}
	}
	else {
		flag |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
		ImGui::TreeNodeEx(node, _current == node ? flag | ImGuiTreeNodeFlags_Selected : flag, node->name());
		if (ImGui::IsItemClicked())
			_current = node;
	}
}

namespace ImGui
{ // begin namespace ImGui

void
ObjectNameInput(cg::SceneNode* object)
{
  const int bufferSize{128};
  static cg::SceneNode* current;
  static char buffer[bufferSize];

  if (object != current)
  {
    strcpy_s(buffer, bufferSize, object->name());
    current = object;
  }
  if (ImGui::InputText("Name", buffer, bufferSize))
    object->setName(buffer);
}

inline bool
ColorEdit3(const char* label, cg::Color& color)
{
  return ImGui::ColorEdit3(label, (float*)&color);
}

inline bool
DragVec3(const char* label, vec3f& v)
{
	return DragFloat3(label, (float*)&v, 0.1f, 0.0f, 0.0f, "%.2g");
}

void
TransformEdit(cg::Transform* transform)
{
  vec3f temp;

  temp = transform->localPosition();
  if (ImGui::DragVec3("Position", temp))
    transform->setLocalPosition(temp);
  temp = transform->localEulerAngles();
  if (ImGui::DragVec3("Rotation", temp))
    transform->setLocalEulerAngles(temp);
  temp = transform->localScale();
  if (ImGui::DragVec3("Scale", temp))
    transform->setLocalScale(temp);
}

} // end namespace ImGui

inline void
P1::sceneGui()
{
  auto scene = (cg::Scene*)_current;

  ImGui::ObjectNameInput(_current);
  ImGui::Separator();
  if (ImGui::CollapsingHeader("Colors"))
  {
    ImGui::ColorEdit3("Background", backgroundColor);
    ImGui::ColorEdit3("Selected Wireframe", selectedWireframeColor);
  }
}

inline void
P1::sceneObjectGui()
{
  auto object = (cg::SceneObject*)_current;

  ImGui::ObjectNameInput(object);
  ImGui::SameLine();
  ImGui::Checkbox("###visible", &object->visible);  // ALTERAR AQUI PARA TOGGLEAR O CHECKBOX


  ImGui::Separator();
  for (auto &c : object->componentes) {
	  if (ImGui::CollapsingHeader(c->typeName()))
	  {
		  if (std::strcmp(c->typeName(), "Transform") == 0)
		  {
			  ImGui::TransformEdit(object->transform());
		  }		  
		  else if (std::strcmp(c->typeName(), "Primitive") == 0)
		  {
			auto primitiva = dynamic_cast<cg::Primitive*>(object->getPrimitive())->mesh();			
			ImGui::TreeNodeEx(primitiva, ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen, ("Numero de Triangulos: " + std::to_string(primitiva->vertexCount() / 3)).c_str());
		   }
	  
	  }
	  
  }
}

inline void
P1::objectGui()
{
  if (_current == nullptr)
    return;
  if (dynamic_cast<cg::SceneObject*>(_current))
  {
    sceneObjectGui();
    return;
  }
  if (dynamic_cast<cg::Scene*>(_current))
    sceneGui();
}

inline void
P1::inspectorWindow()
{
  ImGui::Begin("Inspector");
  objectGui();
  ImGui::End();
}

void
P1::gui()
{
  hierarchyWindow();
  inspectorWindow();
 
  static bool demo = true;
  ImGui::ShowDemoWindow(&demo);
 
}

void
P1::render()
{ 
  Base::render();
  
  P1::drawPrimitives(_scene->root);

  }

void
P1::drawPrimitives(cg::SceneObject* obj) {
	if (obj->visible) {
		_program.setUniformMat4("transform", obj->transform()->localToWorldMatrix()); //aplica as tranformações.
		auto ptr = dynamic_cast<cg::Primitive*>(obj->getPrimitive());
		if (ptr != nullptr) {
			auto m = ptr->mesh();
			m->bind();
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			glDrawElements(GL_TRIANGLES, m->vertexCount(), GL_UNSIGNED_INT, 0);
			m->setVertexColor(selectedWireframeColor);
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glDrawElements(GL_TRIANGLES, m->vertexCount(), GL_UNSIGNED_INT, 0);
			m->useVertexColors();
		}
	}
	for (auto &c : obj->filhos) {
			P1::drawPrimitives(c);
	}
}
