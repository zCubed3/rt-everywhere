//
// Copyright (c) 2025 Liam Reese
//

#include <scene/rte_transform.hpp>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

glm::mat4x4 rteTransform::ComputeMatrix() {

    glm::mat4x4 mat = glm::mat4x4(1.0F);

    mat = glm::translate(mat, position);
    mat *= glm::mat4_cast(rotation);
    mat = glm::scale(mat, scale);

    this->cachedMatrix = mat;

    return this->cachedMatrix;

}

void rteTransform::SetPosition(const glm::vec3 &position) {
    this->position = position;
    isDirty = true;
}

void rteTransform::SetRotation(const glm::quat &rotation) {
    this->rotation = rotation;
    isDirty = true;
}

void rteTransform::SetScale(const glm::vec3 &scale) {
    this->scale = scale;
    isDirty = true;
}

void rteTransform::SetEulerRotation(const glm::vec3 &euler) {
    this->rotation = glm::normalize(glm::quat(euler));
}