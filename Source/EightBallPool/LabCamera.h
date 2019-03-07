#pragma once
#include <include/glm.h>
#include <include/math.h>

namespace Laborator
{
	class Camera
	{
		public:
			Camera()
			{
				position = glm::vec3(0, 2, 5);
				forward = glm::vec3(0, 0, -1);
				up		= glm::vec3(0, 1, 0);
				right	= glm::vec3(1, 0, 0);
				distanceToTarget = 0.5;
			}

			Camera(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up)
			{
				Set(position, center, up);
				distanceToTarget2 = 0.5;
			}

			~Camera()
			{ }

			// Update camera
			void Set(const glm::vec3 &position, const glm::vec3 &center, const glm::vec3 &up)
			{
				this->position = position;
				forward = glm::normalize(center-position);
				right	= glm::cross(forward, up);
				this->up = glm::cross(right,forward);
				distanceToTarget = sqrt((center.x-position.x)*(center.x - position.x) + (center.y - position.y)*(center.y - position.y) + (center.z - position.z)*(center.z - position.z));
			}

			void SetPosition(const glm::vec3 &position) {
				this->position = position;
			}

			void MoveForward(float distance)
			{
				glm::vec3 dir = glm::normalize(glm::vec3(forward.x, 0, forward.z));
			}

			void TranslateForward(float distance)
			{
				this->position += glm::normalize(forward) * distance;
			}

			void TranslateUpword(float distance)
			{
				this->position += glm::normalize(glm::vec3(0,1,0)) * distance;
			}

			void TranslateRight(float distance)
			{
				this->position += glm::normalize(glm::cross(forward, up)) * distance;
			}

			void RotateFirstPerson_OX(float angle)
			{
				glm::vec4 newVector = glm::rotate(glm::mat4(1.0f), angle, right) * glm::vec4(forward, 1);
				forward = glm::normalize(glm::vec3(newVector));

				newVector = glm::rotate(glm::mat4(1.0f), angle, right) * glm::vec4(up, 1);
				up = glm::normalize(glm::vec3(newVector));
			}

			void RotateFirstPerson_OY(float angle)
			{
				glm::vec4 newVector = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0)) * glm::vec4(forward, 1);
				forward = glm::normalize(glm::vec3(newVector));

				newVector = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0, 1, 0)) * glm::vec4(right, 1);
				right = glm::normalize(glm::vec3(newVector));

				up = glm::normalize(glm::cross(right, forward));
			}

			void RotateFirstPerson_OZ(float angle)
			{

			}

			void RotateThirdPerson_OX(float angle)
			{
				this->position += forward * distanceToTarget;

				glm::vec4 newVector = glm::rotate(glm::mat4(1.0f), angle, right) * glm::vec4(forward, 1);
				forward = glm::normalize(glm::vec3(newVector));

				newVector = glm::rotate(glm::mat4(1.0f), angle, right) * glm::vec4(up, 1);
				up = glm::normalize(glm::vec3(newVector));

				this->position -= forward * distanceToTarget;
			}

			void RotateThirdPerson_OY(float angle)
			{
				TranslateForward(distanceToTarget);
				RotateFirstPerson_OY(angle);
				TranslateForward(-distanceToTarget);
			}

			void RotateThirdPerson_OZ(float angle)
			{
			}

			glm::mat4 GetViewMatrix()
			{
				return glm::lookAt(position, position + forward, up);
			}

			glm::vec3 GetTargetPosition()
			{
				return position + forward * distanceToTarget;
			}

		public:
			float distanceToTarget;
			float distanceToTarget2;
			glm::vec3 position;
			glm::vec3 forward;
			glm::vec3 right;
			glm::vec3 up;
		};
}