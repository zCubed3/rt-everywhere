//
// Copyright (c) 2025 Liam Reese
//

#ifndef RTEVERYWHERE_RTE_TRANSFORM_HPP
#define RTEVERYWHERE_RTE_TRANSFORM_HPP

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <functional>

/// @brief Defines a transformation in 3D space, used to cache transform matrices
struct rteTransform {

protected:
    bool isDirty = true;

    glm::mat4x4 cachedMatrix = glm::mat4x4(1.0F);

    glm::vec3 position = glm::vec3(0, 0, 0);
    glm::quat rotation = glm::identity<glm::quat>();
    glm::vec3 scale = glm::vec3(1, 1, 1);

public:

    glm::mat4x4 ComputeMatrix();

    //
    // Setters
    //
    void SetPosition(const glm::vec3& position);
    void SetRotation(const glm::quat& rotation);
    void SetScale(const glm::vec3& scale);

    /// @brief Sets the rotation using a set of euler angles
    void SetEulerRotation(const glm::vec3& euler);

    //
    // Getters
    //

    [[nodiscard]]
    glm::vec3 GetPosition() const  {
        return position;
    }

    [[nodiscard]]
    glm::quat GetRotation() const  {
        return rotation;
    }

    [[nodiscard]]
    glm::vec3 GetScale() const  {
        return scale;
    }

    /// @brief Gets the euler representation of the current rotation (this is not cached!)
    [[nodiscard]]
    glm::vec3 GetEulerRotation() const {
        return glm::eulerAngles(rotation);
    }

    //// @brief Returns the the current or last cached matrix, if you need atomicity please use GetMatrixCached
    [[nodiscard]]
    glm::mat4x4 GetMatrix() {
        if (isDirty) {
            isDirty = false;
            return ComputeMatrix();
        }

        return cachedMatrix;
    }

    //// @brief Returns the last cached matrix, if you need to account for caching, use GetMatrix
    [[nodiscard]]
    glm::mat4x4 GetMatrixCached() const {
        return cachedMatrix;
    }

};

#endif //RTEVERYWHERE_RTE_TRANSFORM_HPP
