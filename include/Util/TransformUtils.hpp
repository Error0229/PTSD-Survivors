#ifndef UTIL_TRANSFORM_UTILS_HPP
#define UTIL_TRANSFORM_UTILS_HPP

#include "Core/Drawable.hpp"

#include "Util/Transform.hpp"
#include "pch.hpp"

namespace Util {

/**
 * @brief Converts a Transform object into uniform buffer data.
 *
 * Converts transform data in Core::UniformBuffer format.
 * Call it and pass the returned value to Core::UniformBuffer->setData().
 *
 * @param transform The Transform object to be converted.
 * @param size The size of the object.
 * @param zIndex The z-index of the transformation.
 * @return A Matrices object representing the uniform buffer data.
 *
 */
Core::Matrices ConvertToUniformBufferData(const Util::Transform &transform,
                                          const glm::vec2 &size, float zIndex);

/**
 * @brief Compute the view-projection matrix for 2D rendering.
 *
 * Uses the current PTSD_Config window dimensions with an orthographic
 * projection centered at (0,0).
 */
glm::mat4 ComputeViewProjection();

} // namespace Util

#endif // UTIL_TRANSFORM_UTILS_HPP
