/*
* Basic camera class
*
* Copyright (C) 2016 by Sascha Willems - www.saschawillems.de
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <string>
#include <vector>
#include <array>

//#define GLM_FORCE_MESSAGES

#define GLM_FORCE_ALIGNED
#define GLM_FORCE_XYZW_ONLY
#define GLM_FORCE_CTOR_INIT GLM_CTOR_INITIALISATION
//#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//#define GLM_FORCE_LEFT_HANDED

#if (__has_include(<glm/glm.hpp>))
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#else
#include "glm/glm/glm.hpp"
#include "glm/glm/gtc/matrix_transform.hpp"
#include "glm/glm/gtc/type_ptr.hpp"
#endif // 1

#include "keycodes.hpp"
#ifndef GLM_OPERATOR

namespace glm
{
#define GLM_OPERATOR
	//vec
	static bool operator<(glm::ivec4 const& v1, glm::ivec4 const& v2)
	{
		std::array<int, 4> va[2] = { {v1.x,v1.y,v1.z,v1.w}, {v2.x,v2.y,v2.z,v2.w} };
		bool ret = va[0] < va[1];
		return ret;
	}
	static bool operator<(glm::vec4 const& v1, glm::vec4 const& v2)
	{
		std::array<double, 4> va[2] = { {v1.x,v1.y,v1.z,v1.w}, {v2.x,v2.y,v2.z,v2.w} };
		bool ret = va[0] < va[1];
		return ret;
	}
	static bool operator<(glm::vec3 const& v1, glm::vec3 const& v2)
	{
		bool yr = (v1.z < v2.z) || (v1.y < v2.y || (v1.y == v2.y && v1.x < v2.x));
		std::array<double, 3> va[2] = { {v1.x,v1.y,v1.z}, {v2.x,v2.y,v2.z} };
		bool ret = va[0] < va[1];
		return ret;
	}
	static bool operator<(glm::vec2 const& v1, glm::vec2 const& v2)
	{
		bool yr = (v1.y < v2.y || (v1.y == v2.y && v1.x < v2.x));
		std::array<double, 2> va[2] = { {v1.x,v1.y},{v2.x,v2.y} };
		bool ret = va[0] < va[1];
		return ret;
	}
	static bool operator>(glm::vec3 const& v1, glm::vec3 const& v2)
	{
		bool yr = (v1.z > v2.z) || (v1.y > v2.y || (v1.y == v2.y && v1.x > v2.x));
		std::array<double, 3> va[2] = { {v1.x,v1.y,v1.z},{v2.x,v2.y,v2.z} };
		bool ret = va[1] < va[0];
		return ret;
	}
	static bool operator>(glm::vec2 const& v1, glm::vec2 const& v2)
	{
		bool yr = (v1.y > v2.y || (v1.y == v2.y && v1.x > v2.x));
		std::array<double, 3> va[2] = { {v1.x,v1.y},{v2.x,v2.y} };
		bool ret = va[1] < va[0];
		return ret;
	}
	/*
		bool operator>=(glm::vec3 const & v1, glm::vec3 const & v2)
		{
			bool yr = (v1.z >= v2.z) || (v1.y >= v2.y || (v1.y == v2.y&& v1.x >= v2.x));
			std::array<double, 3> va[2] = { {v1.x,v1.y,v1.z},{v2.x,v2.y,v2.z} };
			bool ret = va[0] < va[1];
			return ret;
		}
		bool operator>=(glm::vec2 const & v1, glm::vec2 const & v2)
		{
			bool yr = (v1.y >= v2.y || (v1.y == v2.y&& v1.x >= v2.x));
			std::array<double, 3> va[2] = { {v1.x,v1.y,v1.z},{v2.x,v2.y,v2.z} };
			bool ret = va[0] < va[1];
			return ret;
		}*/
		//uvec
	static bool operator<(glm::uvec3 const& v1, glm::uvec3 const& v2)
	{
		return (v1.z < v2.z) || (v1.y < v2.y || (v1.y == v2.y && v1.x < v2.x));
	}
	static bool operator<(glm::uvec2 const& v1, glm::uvec2 const& v2)
	{
		return (v1.y < v2.y || (v1.y == v2.y && v1.x < v2.x));
	}
	static bool operator>(glm::uvec3 const& v1, glm::uvec3 const& v2)
	{
		return (v1.z > v2.z) || (v1.y > v2.y || (v1.y == v2.y && v1.x > v2.x));
	}
	static bool operator>(glm::uvec2 const& v1, glm::uvec2 const& v2)
	{
		return (v1.y > v2.y || (v1.y == v2.y && v1.x > v2.x));
	}
	static bool operator>=(glm::uvec3 const& v1, glm::uvec3 const& v2)
	{
		return (v1.z >= v2.z) || (v1.y >= v2.y || (v1.y == v2.y && v1.x >= v2.x));
	}
	static bool operator>=(glm::uvec2 const& v1, glm::uvec2 const& v2)
	{
		return (v1.y >= v2.y || (v1.y == v2.y && v1.x >= v2.x));
	}
	//ivec
	static bool operator<(glm::ivec3 const& v1, glm::ivec3 const& v2)
	{
		return (v1.z < v2.z) || (v1.y < v2.y || (v1.y == v2.y && v1.x < v2.x));
	}
	static bool operator<(glm::ivec2 const& v1, glm::ivec2 const& v2)
	{
		return (v1.y < v2.y || (v1.y == v2.y && v1.x < v2.x));
	}
	static bool operator>(glm::ivec3 const& v1, glm::ivec3 const& v2)
	{
		return (v1.z > v2.z) || (v1.y > v2.y || (v1.y == v2.y && v1.x > v2.x));
	}
	static bool operator>(glm::ivec2 const& v1, glm::ivec2 const& v2)
	{
		return (v1.y > v2.y || (v1.y == v2.y && v1.x > v2.x));
	}
	static bool operator>=(glm::ivec3 const& v1, glm::ivec3 const& v2)
	{
		return (v1.z >= v2.z) || (v1.y >= v2.y || (v1.y == v2.y && v1.x >= v2.x));
	}
	static bool operator>=(glm::ivec2 const& v1, glm::ivec2 const& v2)
	{
		return (v1.y >= v2.y || (v1.y == v2.y && v1.x >= v2.x));
	}
};
#endif // !GLM_OPERATOR
namespace hz
{

	template <typename T>
	void swapmin(T& a, T& b)
	{
		if (a > b)
		{
			a = a ^ b;
			b = b ^ a;
			a = a ^ b;
		}
	}

#ifndef __RES__H__
#define __RES__H__
	class Res
	{
	public:
		int _type = 0;
		int64_t _inc_ = 0;
		std::string _name;
	public:
		Res()
		{
		}

		virtual ~Res()
		{
		}

		static void destroy(Res* p)
		{
			if (p)
			{
				p->release();
			}
		}

		void release()
		{
			if (--_inc_ < 0)
			{
				delete this;
			}
		}

		int64_t addRef()
		{
			return ++_inc_;
		}
		void set_name(const std::string& n)
		{
			_name = n;
		}
	private:

	};
#endif // !__RES_H__


	//------------------------------------------------------------------------------------------------------------------------------------------------------------
#ifndef NO_Camera
	class Camera :public Res
	{
	public:
		// Use to adjust mouse rotation speed
		float rotationSpeed = 1.0f;
		float movementSpeed = 1.0f;
		// Use to adjust mouse zoom speed
		float zoomSpeed = 1.0f;
		float zoom = 0;
		glm::vec3 rotation = glm::vec3();
		glm::vec3 cameraPos = glm::vec3();
		glm::vec2 mousePos = {};
		int mstype = 0;
	private:
		//perspective
		float fov, _aspect;

		float znear, zfar;
		float _width = 0, _height = 0;
		void updateViewMatrix()
		{
			glm::mat4 rotM = glm::mat4();
			glm::mat4 transM;

			rotM = glm::rotate(rotM, glm::radians(_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
			rotM = glm::rotate(rotM, glm::radians(_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
			rotM = glm::rotate(rotM, glm::radians(_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

			transM = glm::translate(glm::mat4(), _position);

			if (type == CameraType::firstperson)
			{
				matrices._view = rotM * transM;
			}
			else
			{
				matrices._view = transM * rotM;
			}
		};
	public:
		enum CameraType {
			lookat = 1, firstperson, orthographic
		};
		CameraType type = CameraType::lookat;

		glm::vec3 _rotation = glm::vec3();
		glm::vec3 _position = glm::vec3();

		float _rotationSpeed = 1.0f;
		float _movementSpeed = 1.0f;

		struct
		{
			glm::mat4 _perspective = glm::mat4();
			glm::mat4 _view = glm::mat4();
			glm::mat4 _world = glm::mat4();
			glm::mat4 _orthographic = glm::mat4();
		} matrices;
		glm::mat4 _tem;

		struct
		{
			bool left = false;
			bool right = false;
			bool up = false;
			bool down = false;
			bool space = false;
		} keyss;
		bool keys[256] = { false };
		bool is_top = true;
	public:
		Camera() {}
		~Camera() {}

		static Camera* create()
		{
			return new Camera();
		}
		bool moving()
		{
			return keys[KEY_W] || keys[KEY_S] || keys[KEY_A] || keys[KEY_D];
			//return keys.left || keys.right || keys.up || keys.down;
		}
		void set_top(bool is)
		{
			if (is_top != is)
				setOrtho(_width, _height);
			is_top = is;
		}
		void setOrtho(float width, float height, float znear = 0.0f, float zfar = 10000.0f)
		{
			_width = width, _height = height;
			//set_ortho_gl3(width, height);
			//set_ortho_dx12(width, height);

			matrices._orthographic = is_top ? glm::ortho(0.0f, width, height, 0.0f, znear, zfar) : glm::ortho(0.0f, width, 0.0f, height, znear, zfar);
		}
		void set_ortho_gl3(float width, float height)
		{
			/*
				Setup viewport, orthographic projection matrix
				Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right).
				DisplayMin is typically (0,0) for single viewport apps.
			 */
			float x = 0.0f, y = 0.0f;
			float L = x;
			float R = x + width;
			float T = y;
			float B = y + height;
			const float ortho_projection[4][4] =
			{
				{ 2.0f / (R - L),   0.0f,         0.0f,   0.0f },
				{ 0.0f,         2.0f / (T - B),   0.0f,   0.0f },
				{ 0.0f,         0.0f,        -1.0f,   0.0f },
				{ (R + L) / (L - R),  (T + B) / (B - T),  0.0f,   1.0f },
			};
			_width = width, _height = height;
			memcpy(&matrices._orthographic, ortho_projection, sizeof(ortho_projection));
		}
		void set_ortho_dx12(float width, float height)
		{
			// Setup orthographic projection matrix into our constant buffer
			// Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right).
			float x = 0.0f, y = 0.0f;
			float L = x;
			float R = x + width;
			float T = y;
			float B = y + height;
			float mvp[4][4] =
			{
				{ 2.0f / (R - L),   0.0f,           0.0f,       0.0f },
				{ 0.0f,         2.0f / (T - B),     0.0f,       0.0f },
				{ 0.0f,         0.0f,           0.5f,       0.0f },
				{ (R + L) / (L - R),  (T + B) / (B - T),    0.5f,       1.0f },
			};
			_width = width, _height = height;
			memcpy(&matrices._orthographic, mvp, sizeof(mvp));
		}
		void setPerspective(float fov, float aspect, float znear, float zfar)
		{
			_aspect = aspect;
			this->fov = fov;
			this->znear = znear;
			this->zfar = zfar;
			matrices._perspective = glm::perspective(glm::radians(fov), aspect, znear, zfar);
		};

		void updateAspectRatio(float aspect)
		{
			_aspect = aspect;
			matrices._perspective = glm::perspective(glm::radians(fov), aspect, znear, zfar);
		}

		void setPosition(glm::vec3& position)
		{
			this->_position = position;
			updateViewMatrix();
		}

		void setRotation(glm::vec3& rotation)
		{
			this->_rotation = rotation;
			updateViewMatrix();
		};
		void setTranslate(glm::vec3& pos)
		{
			this->_position = pos;
			updateViewMatrix();
		}

		void rotate(glm::vec3 delta)
		{
			this->_rotation += delta;
			if (_rotation.x > 360)
			{
				_rotation.x = 0;
			}
			if (_rotation.y > 360)
			{
				_rotation.y = 0;
			}
			if (_rotation.z > 360)
			{
				_rotation.z = 0;
			}
			updateViewMatrix();
		}
		glm::vec3 getPosition()
		{
			return _position;
		}
		void translate(glm::vec3 delta)
		{
			this->_position += delta;
			updateViewMatrix();
		}
		glm::mat4* getMatrices(int t, bool isleft = true)
		{
			if (!t)
				t = type;
			if (isleft)
			{
				_tem = matrices._view * (((CameraType)t == CameraType::orthographic) ? matrices._orthographic : matrices._perspective);
			}
			else
			{
				_tem = (((CameraType)t == CameraType::orthographic) ? matrices._orthographic : matrices._perspective) * matrices._view;

			}	return &_tem;
		}
		glm::mat4* getMatrices(bool isleft = false)
		{
			if (isleft)
			{
				_tem = matrices._view * matrices._perspective;
			}
			else
			{
				_tem = matrices._perspective * matrices._view;

			}
			return &_tem;
		}

		glm::mat4* getOrthographic()
		{
			return &matrices._orthographic;
		}
		glm::mat4* getPerspective()
		{
			return &matrices._perspective;
		}
		void update(float deltaTime)
		{
			if (type == CameraType::firstperson)
			{
				if (moving())
				{
					glm::vec3 camFront;
					camFront.x = -cos(glm::radians(_rotation.x)) * sin(glm::radians(_rotation.y));
					camFront.y = sin(glm::radians(_rotation.x));
					camFront.z = cos(glm::radians(_rotation.x)) * cos(glm::radians(_rotation.y));
					camFront = glm::normalize(camFront);

					glm::vec3 forward;
					forward.x = -sin(glm::radians(_rotation.y)) * cos(glm::radians(_rotation.x));
					forward.y = 0;
					forward.z = cos(glm::radians(_rotation.y)) * cos(glm::radians(_rotation.x));
					forward = glm::normalize(forward);

					float moveSpeed = deltaTime * _movementSpeed;
					//上下
					//position += glm::normalize(glm::cross(camFront, glm::vec3(1.0f, 0.0f, 0.0f))) * moveSpeed;
					glm::vec3 tp = camFront * moveSpeed;
					if (keys[KEY_W])
					{
						_position += moveSpeed * forward;
					}
					if (keys[KEY_S])
					{
						_position -= moveSpeed * forward;
					}
					if (keys[KEY_A])
						_position -= glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f))) * moveSpeed;
					if (keys[KEY_D])
						_position += glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f))) * moveSpeed;

					updateViewMatrix();
				}
			}
		}

		// Update camera passing separate axis data (gamepad)
		// Returns true if view or position has been changed
		bool updatePad(glm::vec2 axisLeft, glm::vec2 axisRight, float deltaTime)
		{
			bool retVal = false;

			if (type == CameraType::firstperson)
			{
				// Use the common console thumbstick layout		
				// Left = view, right = move

				const float deadZone = 0.0015f;
				const float range = 1.0f - deadZone;

				glm::vec3 camFront;
				camFront.x = -cos(glm::radians(rotation.x)) * sin(glm::radians(rotation.y));
				camFront.y = sin(glm::radians(rotation.x));
				camFront.z = cos(glm::radians(rotation.x)) * cos(glm::radians(rotation.y));
				camFront = glm::normalize(camFront);

				float moveSpeed = deltaTime * _movementSpeed * 2.0f;
				float rotSpeed = deltaTime * _rotationSpeed * 50.0f;

				// Move
				if (fabsf(axisLeft.y) > deadZone)
				{
					float pos = (fabsf(axisLeft.y) - deadZone) / range;
					_position -= camFront * pos * ((axisLeft.y < 0.0f) ? -1.0f : 1.0f) * moveSpeed;
					retVal = true;
				}
				if (fabsf(axisLeft.x) > deadZone)
				{
					float pos = (fabsf(axisLeft.x) - deadZone) / range;
					_position += glm::normalize(glm::cross(camFront, glm::vec3(0.0f, 1.0f, 0.0f))) * pos * ((axisLeft.x < 0.0f) ? -1.0f : 1.0f) * moveSpeed;
					retVal = true;
				}

				// Rotate
				if (fabsf(axisRight.x) > deadZone)
				{
					float pos = (fabsf(axisRight.x) - deadZone) / range;
					_rotation.y += pos * ((axisRight.x < 0.0f) ? -1.0f : 1.0f) * rotSpeed;
					retVal = true;
				}
				if (fabsf(axisRight.y) > deadZone)
				{
					float pos = (fabsf(axisRight.y) - deadZone) / range;
					_rotation.x -= pos * ((axisRight.y < 0.0f) ? -1.0f : 1.0f) * rotSpeed;
					retVal = true;
				}
			}
			else
			{
				// todo: move code from example base class for look-at
			}

			if (retVal)
			{
				updateViewMatrix();
			}

			return retVal;
		}
		void lookAt()
		{
			/*	matrices._world = glm::lookAt(glm::vec3(-5, 3, -10),  // Camera is at (-5,3,-10), in World Space
					glm::vec3(0, 0, 0),     // and looks at the origin
					glm::vec3(0, -1, 0)     // Head is up (set to 0,-1,0 to look upside-down)
				);*/
		}
		glm::vec3 getMouse(float x, float y)
		{
			float scale_h = tanf(fov) * znear;   //投影窗口高度的一半
			float scale_w = _aspect * scale_h;

			glm::vec3 aim = { scale_w * (static_cast<float>(x) / static_cast<float>(_width) * 2.f - 1.f),
				scale_h * (1.f - static_cast<float>(y) / static_cast<float>(_height) * 2.f),znear };
			return aim;
		}
		glm::mat4 SetView(const glm::vec3& eye, const glm::vec3& z_target, const glm::vec3& up, glm::mat4& ViewTranspose)
		{
			glm::vec3 N = z_target;
			N = glm::normalize(N);
			glm::vec3 U = up;
			U = glm::normalize(U);
			U = glm::cross(U, z_target);
			glm::vec3 V = glm::cross(N, U);

			/*以下四行是坐标系旋转矩阵，将物体先进行坐标系平移，再进行坐标系旋转*/
			glm::mat4 data;
			data[0][0] = U.x;  data[0][1] = U.y;  data[0][2] = U.z; data[0][3] = 0.0f;
			data[1][0] = V.x;  data[1][1] = V.y;  data[1][2] = V.z; data[1][3] = 0.0f;
			data[2][0] = N.x;  data[2][1] = N.y;  data[2][2] = N.z; data[2][3] = 0.0f;
			data[3][0] = 0.0f; data[3][1] = 0.0f; data[3][2] = 0.0f; data[3][3] = 1.0f;

			glm::mat4 trans = glm::translate(glm::mat4(), glm::vec3(-eye.x, -eye.y, -eye.z));	//坐标系平移矩阵

			glm::mat4 transTranspose = glm::translate(glm::mat4(), glm::vec3(eye.x, eye.y, eye.z));//坐标系平移矩阵的逆
			glm::mat4 vi = glm::transpose(matrices._view);;//生成坐标系旋转矩阵的逆

			ViewTranspose = transTranspose * vi; //观察矩阵的逆 view-1 = (view)-1 = (trans*(*this))-1 = (*this)-1*(trans)-1

			return matrices._view * trans;//观察矩阵
		}
		bool TestRayOBBIntersection(
			glm::vec3 ray_origin,        // Ray origin, in world space  
			glm::vec3 ray_direction,     // Ray direction (NOT target position!), in world space. Must be normalize()'d.  
			glm::vec3 aabb_min,          // Minimum X,Y,Z coords of the mesh when not transformed at all.  
			glm::vec3 aabb_max,          // Maximum X,Y,Z coords. Often aabb_min*-1 if your mesh is centered, but it's not always the case.  
			glm::mat4 ModelMatrix,       // Transformation applied to the mesh (which will thus be also applied to its bounding box)  
			float& intersection_distance // Output : distance between ray_origin and the intersection with the OBB  
		)
		{

			// Intersection method from Real-Time Rendering and Essential Mathematics for Games  

			float tMin = 0.0f;
			float tMax = 100000.0f;

			glm::vec3 OBBposition_worldspace(ModelMatrix[3].x, ModelMatrix[3].y, ModelMatrix[3].z);

			glm::vec3 delta = OBBposition_worldspace - ray_origin;

			// Test intersection with the 2 planes perpendicular to the OBB's X axis  
			{
				glm::vec3 xaxis(ModelMatrix[0].x, ModelMatrix[0].y, ModelMatrix[0].z);
				float e = glm::dot(xaxis, delta);
				float f = glm::dot(ray_direction, xaxis);

				if (fabs(f) > 0.001f) { // Standard case  

					float t1 = (e + aabb_min.x) / f; // Intersection with the "left" plane  
					float t2 = (e + aabb_max.x) / f; // Intersection with the "right" plane  
													 // t1 and t2 now contain distances betwen ray origin and ray-plane intersections  

													 // We want t1 to represent the nearest intersection,   
													 // so if it's not the case, invert t1 and t2  
					if (t1 > t2) {
						float w = t1; t1 = t2; t2 = w; // swap t1 and t2  
					}

					// tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)  
					if (t2 < tMax)
						tMax = t2;
					// tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)  
					if (t1 > tMin)
						tMin = t1;

					// And here's the trick :  
					// If "far" is closer than "near", then there is NO intersection.  
					// See the images in the tutorials for the visual explanation.  
					if (tMax < tMin)
						return false;

				}
				else { // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"  
					if (-e + aabb_min.x > 0.0f || -e + aabb_max.x < 0.0f)
						return false;
				}
			}


			// Test intersection with the 2 planes perpendicular to the OBB's Y axis  
			// Exactly the same thing than above.  
			{
				glm::vec3 yaxis(ModelMatrix[1].x, ModelMatrix[1].y, ModelMatrix[1].z);
				float e = glm::dot(yaxis, delta);
				float f = glm::dot(ray_direction, yaxis);

				if (fabs(f) > 0.001f) {

					float t1 = (e + aabb_min.y) / f;
					float t2 = (e + aabb_max.y) / f;

					if (t1 > t2) { float w = t1; t1 = t2; t2 = w; }

					if (t2 < tMax)
						tMax = t2;
					if (t1 > tMin)
						tMin = t1;
					if (tMin > tMax)
						return false;

				}
				else {
					if (-e + aabb_min.y > 0.0f || -e + aabb_max.y < 0.0f)
						return false;
				}
			}


			// Test intersection with the 2 planes perpendicular to the OBB's Z axis  
			// Exactly the same thing than above.  
			{
				glm::vec3 zaxis(ModelMatrix[2].x, ModelMatrix[2].y, ModelMatrix[2].z);
				float e = glm::dot(zaxis, delta);
				float f = glm::dot(ray_direction, zaxis);

				if (fabs(f) > 0.001f) {

					float t1 = (e + aabb_min.z) / f;
					float t2 = (e + aabb_max.z) / f;

					if (t1 > t2) { float w = t1; t1 = t2; t2 = w; }

					if (t2 < tMax)
						tMax = t2;
					if (t1 > tMin)
						tMin = t1;
					if (tMin > tMax)
						return false;

				}
				else {
					if (-e + aabb_min.z > 0.0f || -e + aabb_max.z < 0.0f)
						return false;
				}
			}

			intersection_distance = tMin;
			return true;

		}
		/*
		基于slab的碰撞检测：在2D空间中slab可以理解为平行于坐标轴的两条直线间的区域，3D空间中为平行于xy平面（或者yz面，xz面）的两个平面之间的区域。

先推导2D空间中的情况，如果一条射线与有长方形构成的AABB区域相交，则该射线在x-slab和y-slab之间的线段有相交，如下图所示：
3D空间中射线与AABB包围盒的碰撞检测

	  进而可以推导出3D空间中的情况，3D空间中判断射线是否与长方体构成的AABB包围盒相交的方法是，判断该射线在x-slab,y-slab,z-slab之间的部分是否有公共区域。
	 射线的方程可以表示：p0+t*D,其中p0为射线的原点,D为射线的方向，假设射线在x-slab之间的部分t分别为t1和t2,y-slab之间的部分为t3和t4,z-slab之间的部分为t5和t6，剩下的问题就是判断（t1,t2）,(t3,t4),(t5, t6)在数轴上是否有公共部分，即把三维问题降解为一维问题去考虑，关键问题是如何求得参数t.
	 我们知道，平面的方程为 X*n = d，其中X为平面上的点，n为法向量,d为原点到平面的有向距离，射线和平面相交，即有公共点，另上述两式相等得出(p0 + t *D)*n = d，得出t = (d - n*p0)/n * D.因为slab平面方程中的法向量有两个分量为零，所以上面的公式可以进行简化：
假设p0(px,py,pz),D(Dx,Dy,Dz)
则x-slab之间线段的t值为：（d-px）/Dx
y-slab、z-slab以此类推。
	剩下的问题就是求三对t值之间是否有公共部分,方法为如果max(t1, t3, t5)大于min(t2, t4, t6)，则没有公共部分，即射线与AABB包围盒不相交，否则相交。

  代码如下：
//box struct
struct Box
{
 glm::vec3 min;
 glm::vec3 max;
}boxes[3];
//simple Ray struct
struct Ray
{
 glm::vec3 origin, direction;
 float t;
 Ray()
 {
  t = std::numeric_limits::max();
  origin = glm::vec3(0);
  direction = glm::vec3(0);
 }
}eyeRay;
//ray Box intersection code
glm::vec2 intersectBox(const Ray& ray, const Box& cube)
{
 glm::vec3 inv_dir = 1.0f / ray.direction;
 glm::vec3 tMin = (cube.min - ray.origin) * inv_dir;
 glm::vec3 tMax = (cube.max - ray.origin) * inv_dir;
 glm::vec3 t1 = glm::min(tMin, tMax);
 glm::vec3 t2 = glm::max(tMin, tMax);
 float tNear = max(max(t1.x, t1.y), t1.z);
 float tFar = min(min(t2.x, t2.y), t2.z);
 return glm::vec2(tNear, tFar);
}

如果返回值tNear > tFar则不相交，否则射线与AABB包围盒相交。
		*/
	};
#endif
#if 0
	if (camera.firstperson)
	{
		switch (wParam)
		{
		case KEY_W:
			camera.keys.up = true;
			break;
		case KEY_S:
			camera.keys.down = true;
			break;
		case KEY_A:
			camera.keys.left = true;
			break;
		case KEY_D:
			camera.keys.right = true;
			break;
		}
	}

	keyPressed((uint32_t)wParam);
	break;
	case WM_KEYUP:
		if (camera.firstperson)
		{
			switch (wParam)
			{
			case KEY_W:
				camera.keys.up = false;
				break;
			case KEY_S:
				camera.keys.down = false;
				break;
			case KEY_A:
				camera.keys.left = false;
				break;
			case KEY_D:
				camera.keys.right = false;
				break;
			}
		}
		break;
	case WM_RBUTTONDOWN:
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
		mousePos.x = (float)LOWORD(lParam);
		mousePos.y = (float)HIWORD(lParam);
		break;
	case WM_MOUSEWHEEL:
	{
		short wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
		zoom += (float)wheelDelta * 0.005f * zoomSpeed;
		camera.translate(glm::vec3(0.0f, 0.0f, (float)wheelDelta * 0.005f * zoomSpeed));
		viewUpdated = true;
		break;
	}
	case WM_MOUSEMOVE:
		if (wParam & MK_RBUTTON)
		{
			int32_t posx = LOWORD(lParam);
			int32_t posy = HIWORD(lParam);
			zoom += (mousePos.y - (float)posy) * .005f * zoomSpeed;
			camera.translate(glm::vec3(-0.0f, 0.0f, (mousePos.y - (float)posy) * .005f * zoomSpeed));
			mousePos = glm::vec2((float)posx, (float)posy);
			viewUpdated = true;
		}
		if (wParam & MK_LBUTTON)
		{
			int32_t posx = LOWORD(lParam);
			int32_t posy = HIWORD(lParam);
			rotation.x += (mousePos.y - (float)posy) * 1.25f * rotationSpeed;
			rotation.y -= (mousePos.x - (float)posx) * 1.25f * rotationSpeed;
			camera.rotate(glm::vec3((mousePos.y - (float)posy) * camera.rotationSpeed, -(mousePos.x - (float)posx) * camera.rotationSpeed, 0.0f));
			mousePos = glm::vec2((float)posx, (float)posy);
			viewUpdated = true;
		}
		if (wParam & MK_MBUTTON)
		{
			int32_t posx = LOWORD(lParam);
			int32_t posy = HIWORD(lParam);
			cameraPos.x -= (mousePos.x - (float)posx) * 0.01f;
			cameraPos.y -= (mousePos.y - (float)posy) * 0.01f;
			camera.translate(glm::vec3(-(mousePos.x - (float)posx) * 0.01f, -(mousePos.y - (float)posy) * 0.01f, 0.0f));
			viewUpdated = true;
			mousePos.x = (float)posx;
			mousePos.y = (float)posy;
		}
		break;
#endif
}
#endif
