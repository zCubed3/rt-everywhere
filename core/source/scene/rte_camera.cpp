//
// Copyright (c) 2025 Liam Reese
//

#include <scene/rte_camera.hpp>

void rteCamera::ComputeMatrix() {

    this->transform.ComputeMatrix();
    this->invViewMatrix = glm::inverse(this->transform.GetMatrix());

    this->projectionMatrix = glm::perspective(glm::radians(fovY), aspect, 0.001F, 100.0F);
    this->unprojectMatrix = glm::inverse(this->projectionMatrix);

}

glm::vec3 rteCamera::NDCToRayDirection(const glm::vec2& ndc) const {

    glm::vec2 clip = (ndc - 0.5F) * 2.0F;
    clip.y *= -1.0F;

    return ClipToRayDirection(clip);

}

glm::vec3 rteCamera::ClipToRayDirection(const glm::vec2& clip) const {

    glm::vec3 direction;

    direction = unprojectMatrix * glm::vec4(clip, 0.0F, 1.0F);
    direction = transform.GetMatrixCached() * glm::vec4(direction, 0.0F);
    direction = glm::normalize(direction);

    return direction;

}
