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
// OVERVIEW: Camera.cpp
// ========
// Source file for camera.
//
// Author(s): Paulo Pagliosa (and your name)
// Last revision: 15/09/2018

#include "Camera.h"
#include "math/Matrix4x4.h"
#include "math/Vector3.h"
#include <iostream>
namespace cg
{ // begin namespace cg


//////////////////////////////////////////////////////////
//
// Camera implementation
// ======
uint32_t Camera::_nextId;

inline const char*
Camera::defaultName()
{
  static char name[16];

  snprintf(name, sizeof name, "Camera %d", ++_nextId);
  return name;
}

Camera::Camera():
  NameableObject{defaultName()}
{
  setDefaultView();
}

void
Camera::setPosition(const vec3f& value)
//[]---------------------------------------------------[]
//|  Set the camera's position                          |
//|                                                     |
//|  Setting the camera's position will not change      |
//|  neither the direction of projection nor the        |
//|  distance between the position and the focal point. |
//|  The focal point will be moved along the direction  |
//|  of projection.                                     |
//[]---------------------------------------------------[]
{
	/* Ser� que precisa setar o VUP aqui ?*/
	setDistance();
	setDelta();
	_position = value;
	_focalPoint = _position + delta;
	updateViewMatrix();


}

void
Camera::setEulerAngles(const vec3f& value)
{
	_eulerAngles = value;
	/*
	std::cout << "EA X: " << _eulerAngles.x << "  ";
	std::cout << "EA Y: " << _eulerAngles.y << "  ";
	std::cout << "EA Z: " << _eulerAngles.z << std::endl << std::endl;
	*/
	setRotation(quatf::eulerAngles(value));
}

void
Camera::setRotation(const quatf& value)
{
	/* 
		N�o usei o Delta por que a rota��o � a mesma,
		ent�o a dist�ncia se manter�.
	*/
	_rotation = value;
	_focalPoint = _rotation.rotate(focalPoint());
	_position = _rotation.rotate(_position);
	_vup = _rotation.rotate(_vup).versor();
	updateViewMatrix();	
}

void
Camera::setProjectionType(ProjectionType value)
//[]---------------------------------------------------[]
//|  Set the camera's projection type                   |
//[]---------------------------------------------------[]
{
	_projectionType = value;	
}

void
Camera::setDistance()
//[]---------------------------------------------------[]
//|  Set the camera's distance                          |
//|                                                     |
//|  Setting the distance between the position and      |
//|  focal point will move the focal point along the    |
//|  direction of projection.                           |
//[]---------------------------------------------------[]
{
	_distance = (position() - focalPoint()).length();
}

void
Camera::setViewAngle(float value)
//[]---------------------------------------------------[]
//|  Set the camera's view angle                        |
//[]---------------------------------------------------[]
{
	_viewAngle = value;	
}

void
Camera::setHeight(float value)
//[]---------------------------------------------------[]
//|  Set the camera's view_height                       |
//[]---------------------------------------------------[]
{
	_height = value;
}

void
Camera::setAspectRatio(float value)
//[]---------------------------------------------------[]
//|  Set the camera's aspect ratio                      |
//[]---------------------------------------------------[]
{
  // TODO
  // A = W / H . H � recebido via interface.

}

void
Camera::setClippingPlanes(float F, float B)
//[]---------------------------------------------------[]
//|  Set the distance of the clippling planes           |
//[]---------------------------------------------------[]
{
  // TODO
	_B = B;
	_F = F;

}

void
Camera::rotateYX(float ay, float ax, bool orbit)
//[]---------------------------------------------------[]
//|  Rotate YX                                          |
//|                                                     |
//|  If orbit is true, then it is a composition of an   |
//|  azimuth of ay with an elevation of ax, in this     |
//|  order. Otherwise, it is a composition of a yaw of  |
//|  ay with a pitch of ax, in this order.              |
//[]---------------------------------------------------[]
{
  // TODO
}

void
Camera::zoom(float zoom)
//[]---------------------------------------------------[]
//|  Zoom                                               |
//|                                                     |
//|  Change the view angle (or height) of the camera so |
//|  that more or less of a scene occupies the view     |
//|  window. A value > 1 is a zoom-in. A value < 1 is   |
//|  zoom-out.                                          |
//[]---------------------------------------------------[]
{
	if (_projectionType == Camera::Perspective) {
		setViewAngle(viewAngle() / zoom);
		updatePerspectiveProjectionMatrix();
	}
	else {

	}
  // TODO
}

void
Camera::translate(float dx, float dy, float dz)
//[]---------------------------------------------------[]
//|  Translate the camera                               |
//[]---------------------------------------------------[]
{
	setDistance();
	setDelta();
	_position.x += dx;
	_position.y += dy;
	_position.z += dz;
	_focalPoint = _position + delta;
	updateViewMatrix();
}

void
Camera::setDefaultView(float aspect)
//[]---------------------------------------------------[]
//|  Set default view                                   |
//[]---------------------------------------------------[]
{
  _position.set(0.0f, 0.0f, 10.0f);
  _eulerAngles.set(0.0f);
  _rotation = quatf::identity();
  _focalPoint.set(0.0f, 0.0f, 0.0f);
  _distance = 10.0f;
  _aspectRatio = aspect;
  _projectionType = Perspective;
  _viewAngle = 60.0f;
  _height = 10.0f;
  _F = 0.01f;
  _B = 1000.0f;
  _vup = vec3f::up();
  updateViewMatrix();
  updatePerspectiveProjectionMatrix(); // Precisa arrumar isso.
  // TODO: update view and projection matrices.
}

} // end namespace cg
