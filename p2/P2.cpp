#include "MeshSweeper.h"
#include <iostream>

#include "P2.h"

MeshMap P2::_defaultMeshes;

inline void
P2::buildDefaultMeshes()
{
  _defaultMeshes["None"] = nullptr;
  _defaultMeshes["Box"] = MeshSweeper::makeBox();
}

inline Primitive*
makePrimitive(MeshMapIterator mit)
{
  return new Primitive(mit->second, mit->first);
}

inline void
P2::buildScene()
{
  _current = _scene = new Scene{"Scene 1"};
  _box = new SceneObject{ "Box 1", *_scene, {} , {}  };
  _box->setParent(_scene->root);
  _scene->root->filhos.push_back(_box);
  _primitive = makePrimitive(_defaultMeshes.find("Box"));
  _box->addComponent(_primitive);
}

void
P2::initialize()
{
  Application::loadShaders(_program, "shaders/p2.vs", "shaders/p2.fs");
  Assets::initialize();
  buildDefaultMeshes();
  buildScene();
  _camera = new Camera{};
  _camera->setAspectRatio((float)width() / (float)height());
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_POLYGON_OFFSET_FILL);
  glPolygonOffset(1.0f, 1.0f);
  glEnable(GL_LINE_SMOOTH);
  _program.use();
}

namespace ImGui
{
  void ShowDemoWindow(bool*);
}

inline void
P2::hierarchyWindow()
{
	ImGui::Begin("Hierarchy");

	/* Protótipo da função para capturar o clique do mouse
	
	if (ImGui::GetIO().MouseClicked[1])
		std::cout << _current << std::endl;
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
					dynamic_cast<cg::SceneObject*>(_current)->novoFilho("Box ", makePrimitive(_defaultMeshes.find("Box"))); // PRECISO ALTERAR AQUI
				}
				else {
					dynamic_cast<cg::Scene*>(_current)->root->novoFilho("Box ", makePrimitive(_defaultMeshes.find("Box"))); // OU ALTERAR AQUI
				}

			}
			ImGui::EndMenu();
		}
		ImGui::EndPopup();
	}
	ImGui::SameLine();
	if (ImGui::Button("Delete -- NOTWORKING###Object")) {
		std::cout << "Cliquei no Botão Delete()" << std::endl;
	}
	ImGui::Separator();

	ImGuiTreeNodeFlags flag{ ImGuiTreeNodeFlags_OpenOnArrow };
	drawHierarchyWindow(_scene, _current, flag);
	ImGui::End();


}

void
P2::drawHierarchyWindow(cg::SceneObject* node, cg::SceneNode* current, ImGuiTreeNodeFlags flag) {	
	if (node->filhos.size() > 0) {
		auto open = ImGui::TreeNodeEx(node, _current == node ? flag | ImGuiTreeNodeFlags_Selected : flag, node->name());
		
		// Atualiza current se nó foi clicado
		if (ImGui::IsItemClicked()) {
			_current = node;
		
		}
		
		// Aciona evento de clique em botão direito
		/*
		if (ImGui::IsItemClicked(1))
			ImGui::OpenPopup("RightClickPopup");
		if (ImGui::BeginPopup("RightClickPopup"))
		{
			if (ImGui::MenuItem(node->name()))
			{

			}
			ImGui::EndPopup();
		}
		*/
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
	// Drag and drop SOURCE -> Aqui ele pegou os nodes certos
	if (ImGui::BeginDragDropSource())
	{
		ImGui::SetDragDropPayload("setParent", node, sizeof(node));
		std::cout << "Nó que foi enviado :" << node->name() << std::endl;
		ImGui::EndDragDropSource();
	}
	//Drag and drop TARGET
	if (ImGui::BeginDragDropTarget())
	{
		if (auto* payload = ImGui::AcceptDragDropPayload("setParent"))
		{
			std::cout << "Nó que recebeu :" << node->name() << std::endl;
			auto nFilho = *(cg::SceneNode*)payload->Data; 
			// PRECISO RECUPERAR O PAYLOAD
		}
		ImGui::EndDragDropTarget();
	}
	
}

void
P2::drawHierarchyWindow(cg::Scene* node, cg::SceneNode* current, ImGuiTreeNodeFlags flag) {
	if (node->root->filhos.size() > 0) {
		auto open = ImGui::TreeNodeEx(node, _current == node ? flag | ImGuiTreeNodeFlags_Selected : flag, node->name());
		if (ImGui::IsItemClicked())
			_current = node;
		
		/*if (ImGui::IsItemClicked(1))
			ImGui::OpenPopup("RightClickPopup");
		if (ImGui::BeginPopup("RightClickPopup"))
		{
			if (ImGui::MenuItem(node->root->name()))
			{

			}
			ImGui::EndPopup();

		}	
		*/
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
ObjectNameInput(NameableObject* object)
{
  const int bufferSize{128};
  static NameableObject* current;
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
ColorEdit3(const char* label, Color& color)
{
  return ImGui::ColorEdit3(label, (float*)&color);
}

inline bool
DragVec3(const char* label, vec3f& v)
{
  return DragFloat3(label, (float*)&v, 0.1f, 0.0f, 0.0f, "%.2g");
}

void
TransformEdit(Transform* transform)
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
P2::sceneGui()
{
  auto scene = (Scene*)_current;

  ImGui::ObjectNameInput(_current);
  ImGui::Separator();
  if (ImGui::CollapsingHeader("Colors"))
  {
    ImGui::ColorEdit3("Background", scene->backgroundColor);
    ImGui::ColorEdit3("Ambient Light", scene->ambientLight);
  }
}

inline void
P2::inspectPrimitive(Primitive& primitive)
{
  char buffer[16];

  snprintf(buffer, 16, "%s", primitive.meshName());
  ImGui::InputText("Mesh", buffer, 16, ImGuiInputTextFlags_ReadOnly);
  if (ImGui::BeginDragDropTarget())
  {
    if (auto* payload = ImGui::AcceptDragDropPayload("PrimitiveMesh"))
    {
      auto mit = *(MeshMapIterator*)payload->Data;
      primitive.setMesh(mit->second, mit->first);
    }
    ImGui::EndDragDropTarget();
  }
  ImGui::SameLine();
  if (ImGui::Button("...###PrimitiveMesh"))
    ImGui::OpenPopup("PrimitiveMeshPopup");
  if (ImGui::BeginPopup("PrimitiveMeshPopup"))
  {
    auto& meshes = Assets::meshes();

    if (!meshes.empty())
    {
      for (auto mit = meshes.begin(); mit != meshes.end(); ++mit)
        if (ImGui::Selectable(mit->first.c_str()))
          primitive.setMesh(Assets::loadMesh(mit), mit->first);
      ImGui::Separator();
    }
    for (auto mit = _defaultMeshes.begin(); mit != _defaultMeshes.end(); ++mit)
      if (ImGui::Selectable(mit->first.c_str()))
        primitive.setMesh(mit->second, mit->first);
    ImGui::EndPopup();
  }
  ImGui::ColorEdit3("Mesh Color", (float*)&primitive.color);
}

inline void
P2::sceneObjectGui()
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
				//ImGui::TreeNodeEx(primitiva, ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen, ("Numero de Triangulos: " + std::to_string(primitiva->vertexCount() / 3)).c_str());
			}

		}

	}
}
inline void
P2::objectGui()
{
  if (_current == nullptr)
    return;
  if (dynamic_cast<SceneObject*>(_current))
  {
    sceneObjectGui();
    return;
  }
  if (dynamic_cast<Scene*>(_current))
    sceneGui();
}

inline void
P2::inspectorWindow()
{
  ImGui::Begin("Inspector");
  objectGui();
  ImGui::End();
}

inline void
P2::cameraGui()
{
  if (ImGui::Button("Set Default View"))
    _camera->setDefaultView(float(width()) / float(height()));
  ImGui::Separator();

  vec3f temp;

  ImGui::ObjectNameInput(_camera);
  temp = _camera->position();
  if (ImGui::DragVec3("Position", temp))
    _camera->setPosition(temp);
  temp = _camera->eulerAngles();
  if (ImGui::DragVec3("Rotation", temp))
    _camera->setEulerAngles(temp);

  static const char* projectionNames[]{"Perspective", "Orthographic"};
  auto cp = _camera->projectionType();

  if (ImGui::BeginCombo("Projection", projectionNames[cp]))
  {
    for (auto i = 0; i < IM_ARRAYSIZE(projectionNames); ++i)
    {
      bool selected = cp == i;

      if (ImGui::Selectable(projectionNames[i], selected))
        cp = (Camera::ProjectionType)i;
      if (selected)
        ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }
  _camera->setProjectionType(cp);
  if (cp == Camera::Perspective)
  {
    auto fov = _camera->viewAngle();

    if (ImGui::SliderFloat("View Angle",
      &fov,
      MIN_ANGLE,
      MAX_ANGLE,
      "%.0f deg",
      1.0f))
      _camera->setViewAngle(fov <= MIN_ANGLE ? MIN_ANGLE : fov);
  }
  else
  {
    auto h = _camera->height();

    if (ImGui::DragFloat("Height",
      &h,
      MIN_HEIGHT * 10.0f,
      MIN_HEIGHT,
      math::Limits<float>::inf()))
      _camera->setHeight(h <= 0 ? MIN_HEIGHT : h);
  }

  float n;
  float f;

  _camera->clippingPlanes(n, f);

  if (ImGui::DragFloatRange2("Clipping Planes",
    &n,
    &f,
    MIN_DEPTH,
    MIN_DEPTH,
    math::Limits<float>::inf(),
    "Near: %.2f",
    "Far: %.2f"))
  {
    if (n <= 0)
      n = MIN_DEPTH;
    if (f - n < MIN_DEPTH)
      f = n + MIN_DEPTH;
    _camera->setClippingPlanes(n, f);
  }
}

inline void
P2::assetsWindow()
{
  ImGui::Begin("Assets");
  if (ImGui::CollapsingHeader("Meshes"))
  {
    auto& meshes = Assets::meshes();

    for (auto mit = meshes.begin(); mit != meshes.end(); ++mit)
    {
      auto meshName = mit->first.c_str();
      auto selected = false;

      ImGui::Selectable(meshName, &selected);
      if (ImGui::BeginDragDropSource())
      {
        Assets::loadMesh(mit);
        ImGui::Text(meshName);
        ImGui::SetDragDropPayload("PrimitiveMesh", &mit, sizeof(mit));
        ImGui::EndDragDropSource();
      }
    }
  }
  ImGui::Separator();
  if (ImGui::CollapsingHeader("Textures"))
  {
    // p4
  }
  ImGui::End();
}

inline void
P2::rendererWindow()
{
  ImGui::Begin("Renderer");
  if (ImGui::CollapsingHeader("Camera"))
    cameraGui();
  ImGui::End();
}

inline void
P2::fileMenu()
{
  if (ImGui::MenuItem("New"))
  {
    // TODO
  }
  if (ImGui::MenuItem("Open...", "Ctrl+O"))
  {
    // TODO
  }
  ImGui::Separator();
  if (ImGui::MenuItem("Save", "Ctrl+S"))
  {
    // TODO
  }
  if (ImGui::MenuItem("Save As..."))
  {
    // TODO
  }
  ImGui::Separator();
  if (ImGui::MenuItem("Exit", "Alt+F4"))
  {
    shutdown();
  }
}

inline bool
showStyleSelector(const char* label)
{
  static int style = 1;

  if (!ImGui::Combo(label, &style, "Classic\0Dark\0Light\0"))
    return false;
  switch (style)
  {
    case 0: ImGui::StyleColorsClassic();
      break;
    case 1: ImGui::StyleColorsDark();
      break;
    case 2: ImGui::StyleColorsLight();
      break;
  }
  return true;
}

inline void
P2::showOptions()
{
  ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.6f);
  showStyleSelector("Color Theme##Selector");
  ImGui::ColorEdit3("Selected Wireframe", _selectedWireframeColor);
  ImGui::PopItemWidth();
}

inline void
P2::mainMenu()
{
  if (ImGui::BeginMainMenuBar())
  {
    if (ImGui::BeginMenu("File"))
    {
      fileMenu();
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Tools"))
    {
      if (ImGui::BeginMenu("Options"))
      {
        showOptions();
        ImGui::EndMenu();
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }
}

void
P2::gui()
{
  mainMenu();
  hierarchyWindow();
  inspectorWindow();
  assetsWindow();
  rendererWindow();

  /*
  static bool demo = true;
  ImGui::ShowDemoWindow(&demo);
  */
}

inline void
renderMesh(GLMesh* mesh, GLuint mode)
{
  glPolygonMode(GL_FRONT_AND_BACK, mode);
  glDrawElements(GL_TRIANGLES, mesh->vertexCount(), GL_UNSIGNED_INT, 0);
}

constexpr auto CAMERA_RES = 0.01f;
constexpr auto ZOOM_SCALE = 1.01f;

void
P2::render()
{
  const auto& bc = _scene->backgroundColor;

  glClearColor(bc.r, bc.g, bc.b, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  if (!_box->visible)
    return;
  if (_moveFlags)
  {
    const auto delta = _camera->distance() * CAMERA_RES;
    auto d = vec3f::null();

    if (_moveFlags.isSet(MoveBits::Forward))
      d.z -= delta;
    if (_moveFlags.isSet(MoveBits::Back))
      d.z += delta;
    if (_moveFlags.isSet(MoveBits::Left))
      d.x -= delta;
    if (_moveFlags.isSet(MoveBits::Right))
      d.x += delta;
    if (_moveFlags.isSet(MoveBits::Up))
      d.y += delta;
    if (_moveFlags.isSet(MoveBits::Down))
      d.y -= delta;
    _camera->translate(d);
  }
  _program.setUniformMat4("vpMatrix", vpMatrix(_camera));
  _program.setUniformVec4("ambientLight", _scene->ambientLight);
  _program.setUniformVec3("lightPosition", _camera->position());

  auto m = glMesh(_primitive->mesh());

  if (nullptr == m)
    return;

  auto t = _primitive->transform();
  auto normalMatrix = mat3f{t->worldToLocalMatrix()}.transposed();

  _program.setUniformMat4("transform", t->localToWorldMatrix());
  _program.setUniformMat3("normalMatrix", normalMatrix);
  _program.setUniformVec4("color", _primitive->color);
  _program.setUniform("flatMode", (int)0);
  m->bind();
  renderMesh(m, GL_FILL);
  if (_current != _box)
    return;
  _program.setUniformVec4("color", _selectedWireframeColor);
  _program.setUniform("flatMode", (int)1);
  renderMesh(m, GL_LINE);
}

bool
P2::windowResizeEvent(int width, int height)
{
  _camera->setAspectRatio(float(width) / float(height));
  return true;
}

bool
P2::keyInputEvent(int key, int action, int mods)
{
  auto active = action != GLFW_RELEASE && mods == GLFW_MOD_ALT;

  switch (key)
  {
    case GLFW_KEY_W:
      _moveFlags.enable(MoveBits::Forward, active);
      break;
    case GLFW_KEY_S:
      _moveFlags.enable(MoveBits::Back, active);
      break;
    case GLFW_KEY_A:
      _moveFlags.enable(MoveBits::Left, active);
      break;
    case GLFW_KEY_D:
      _moveFlags.enable(MoveBits::Right, active);
      break;
    case GLFW_KEY_Q:
      _moveFlags.enable(MoveBits::Up, active);
      break;
    case GLFW_KEY_Z:
      _moveFlags.enable(MoveBits::Down, active);
      break;
  }
  return false;
}

bool
P2::scrollEvent(double, double yOffset)
{
  if (ImGui::GetIO().WantCaptureMouse)
    return false;
  _camera->zoom(yOffset < 0 ? 1.0f / ZOOM_SCALE : ZOOM_SCALE);
  return true;
}

bool
P2::mouseButtonInputEvent(int button, int actions, int mods)
{
  if (ImGui::GetIO().WantCaptureMouse)
    return false;
  (void)mods;

  auto active = actions == GLFW_PRESS;

  if (button == GLFW_MOUSE_BUTTON_RIGHT)
    _dragFlags.enable(DragBits::Rotate, active);
  else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
    _dragFlags.enable(DragBits::Pan, active);
  if (_dragFlags)
    cursorPosition(_pivotX, _pivotY);
  return true;
}

bool
P2::mouseMoveEvent(double xPos, double yPos)
{
  if (!_dragFlags)
    return false;
  _mouseX = (int)xPos;
  _mouseY = (int)yPos;

  const auto dx = (_pivotX - _mouseX);
  const auto dy = (_pivotY - _mouseY);

  _pivotX = _mouseX;
  _pivotY = _mouseY;
  if (dx != 0 || dy != 0)
  {
    if (_dragFlags.isSet(DragBits::Rotate))
    {
      const auto da = -_camera->viewAngle() * CAMERA_RES;
      _camera->rotateYX(dx * da, dy * da, isKeyPressed(GLFW_KEY_LEFT_ALT));
    }
    if (_dragFlags.isSet(DragBits::Pan))
    {
      const auto dt = -_camera->distance() * CAMERA_RES;
      _camera->translate(-dt * math::sign(dx), dt * math::sign(dy), 0);
    }
  }
  return true;
}
