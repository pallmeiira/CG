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
// OVERVIEW: SceneObject.cpp
// ========
// Source file for scene object.
//
// Author(s): Paulo Pagliosa (and your name)
// Last revision: 25/08/2018

#include "SceneObject.h"
#include "Primitive.h"
#include <list>
#include <string>

namespace cg
{ // begin namespace cg

/////////////////////////////////////////////////////////////////////
//
// SceneObject implementation
// ===========
void
SceneObject::setParent(SceneObject* parent)
{
	this->_parent = parent;
}

// IMPLEMENTAR NOME DOS OBJETOS AQUI
const char*
setNome() {
	const char* c = "Filho";
	return (c + std::to_string(1)).c_str();
}

/// Remove o filho de um pai --> NOT WORKING
void
SceneObject::removeNoh() {
	
	delete(this);
	
}

/// Adiciona um componente
/* Função para adicionar um Componenete do tipo -> Da pra colocar direto no construtor ?
 * transform na lista de componenetes do objeto criado
 */
void 
SceneObject::adicionaTransform() {
	this->componentes.push_back(this->transform());
}

/* Função para adicionar filhos --> Da pra refatorar
 * até agora ela adiciona dois "tipos" de filhos.
 * Quando o filho é do tipo Box recebe uma primitiva para ele.
 */
/// Cria um novo filho
SceneObject * 
SceneObject::novoFilho(const char* tipo) {
	static int idObj;
	SceneObject * filho = new SceneObject((tipo + std::to_string(idObj)).c_str(), this->scene(), {}, {});
	filho->setParent(this);
	this->filhos.push_back(filho);
	idObj++;
	return filho;
}

/// Cria um novo filho
SceneObject * 
SceneObject::novoFilho(const char* tipo, Primitive* primitive) {
	static int idBox;
	SceneObject * filho = new SceneObject((tipo + std::to_string(idBox)).c_str(), this->scene(), {}, { primitive });
	filho->setParent(this);
	this->filhos.push_back(filho);
	idBox++;
	return filho;
}
/*	Procura uma componenete num SceneObject
*	se encontrar, retorna a componente, caso contrário
*	retorna nullptr.
*/
Component * 
SceneObject::getPrimitive() {
	for (auto &c : this->componentes) {
		if (std::strcmp(c->typeName(), "Primitive") == 0) {
			return c;
		}
	}
	return nullptr;
}

} // end namespace cg
