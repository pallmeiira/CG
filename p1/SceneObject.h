//[]---------------------------------------------------------------[]
//|                                                                 |
//| Copyright (C) 2018 Orthrus Group.                               |
//|                                                                 |
//| This software is provided 'as-is', without any express or       |
//| implied warranty. In no event will the authors be held liable   |
//| for any damages arising from the use of this software.          |
//|                                                                 |
//| Permission is granted to anyone to use this software for any    |
//| purpose, including commercial applications, and to alter it and |
//| redistribute it freely, subject to the following restrictions:  |
//|                                                                 |
//| 1. The origin of this software must not be misrepresented; you  |
//| must not claim that you wrote the original software. If you use |
//| this software in a product, an acknowledgment in the product    |
//| documentation would be appreciated but is not required.         |
//|                                                                 |
//| 2. Altered source versions must be plainly marked as such, and  |
//| must not be misrepresented as being the original software.      |
//|                                                                 |
//| 3. This notice may not be removed or altered from any source    |
//| distribution.                                                   |
//|                                                                 |
//[]---------------------------------------------------------------[]
//
// OVERVIEW: SceneObject.h
// ========
// Class definition for scene object.
//
// Author(s): Paulo Pagliosa (and your name)
// Last revision: 25/08/2018

#ifndef __SceneObject_h
#define __SceneObject_h

#include "SceneNode.h"
#include "Primitive.h"
#include "Transform.h"
#include <list>

namespace cg
{ // begin namespace cg

// Forward definition
class Scene;


/////////////////////////////////////////////////////////////////////
//
// SceneObject: scene object class
// ===========
class SceneObject: public SceneNode
{
public:
  bool visible{true};
  std::list<Reference<cg::SceneObject>> filhos;
  std::list<Reference<cg::Component>> componentes;

  /// Constructs an empty scene object.
  SceneObject(const char* name, Scene* scene):
    SceneNode{name},
    _scene{scene},
    _parent{}
  {
    makeUse(&_transform);
  }

  // Constrói um objeto com uma lista de filhos 
  SceneObject(const char* name, Scene* scene, std::list<Reference<cg::SceneObject>> filhos) :
	  SceneNode{ name },
	  _scene{ scene },
	  _parent{},
	  filhos{filhos}
  {
	  makeUse(&_transform);
  }
  
  // Constrói um objeto com uma lista de filhos e lista de componentes 
  SceneObject(const char* name, Scene* scene, std::list<Reference<cg::SceneObject>> filhos, std::list<Reference<cg::Component>> componentes) :
	  SceneNode{ name },
	  _scene{ scene },
	  _parent{},
	  filhos{ filhos },
	 componentes{ componentes }
  {
	  adicionaTransform();
	  makeUse(&_transform);
  }
  /// Returns the scene which this scene object belong to.
  auto scene() const
  {
    return _scene;
  }

  /// Returns the parent of this scene object.
  auto parent() const
  {
    return _parent;
  }

  /// Sets the parent of this scene object.
  void setParent(SceneObject* parent);
  
  /// Sets a name 
  const char* setNome ();

  /// Remove o filho de um pai
  void removeNoh();
  
  ///
  Component * getPrimitive();
  /// Adiciona um componente
  void adicionaTransform();

  auto transform()
  {
    return &_transform;
  }
  /// Cria um novo filho
  SceneObject * novoFilho(const char*);
  SceneObject * novoFilho(const char*, Primitive*);
  
private:
  Scene* _scene;
  SceneObject* _parent;
  Transform _transform;

  friend class Scene;

}; // SceneObject

/// Returns the transform of a component.
inline Transform*
Component::transform() // declared in Component.h
{
  return sceneObject()->transform();
}

/// Returns the parent of a transform.
inline Transform*
Transform::parent() const // declared in Transform.h
{
  if (auto p = sceneObject()->parent())
    return p->transform();
   return nullptr;
}

} // end namespace cg

#endif // __SceneObject_h
