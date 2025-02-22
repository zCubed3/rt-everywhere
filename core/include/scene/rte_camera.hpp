//
// Copyright (c) 2025 Liam Reese
//

#ifndef RTEVERYWHERE_RTE_CAMERA_HPP
#define RTEVERYWHERE_RTE_CAMERA_HPP

// TODO: Orthographic and equirectangular cameras?

#include <data/rte_transform.hpp>

/// @brief The "shape" or type of camera this is, changes projection scheme
enum class rteCameraType {
    /// @brief A perspective camera
    Perspective
};

/// @brief Defines a viewpoint into the world
class rteCamera {

protected:
    bool isDirty = false;

    glm::mat4x4 invViewMatrix;

    glm::mat4x4 projectionMatrix;
    glm::mat4x4 unprojectMatrix;

public:
    rteTransform transform;

    rteCameraType cameraType = rteCameraType::Perspective;

    /// @brief The aspect ratio of this camera
    float aspect = 1.0F;

    /// @brief The vertical field of view of this camera, expressed in degrees
    float fovY = 60.0F;

    void ComputeMatrix();

    /// @brief Converts an NDC space coordinate into the direction of a ray coming out of the camera
    glm::vec3 NDCToRayDirection(const glm::vec2& ndc) const;

    /// @brief Converts a clip space coordinate into the direction of a ray coming out of the camera
    glm::vec3 ClipToRayDirection(const glm::vec2& clip) const;

};

#endif //RTEVERYWHERE_RTE_CAMERA_HPP
