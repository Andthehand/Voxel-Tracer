#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/constants.hpp>

namespace VoxelTracer {
    class VoxelCamera {
    public:
        VoxelCamera(const glm::vec3& pos, const glm::vec3& rot, const glm::vec2& ext, float f)
            : position(pos), rotation(rot), extent(ext), fov(f) {
        }

        void look_direction(const glm::vec3& direction) {
            double z = atan2(-direction.x, direction.y);
            double x = atan2(direction.z, glm::length(glm::vec2(direction.x, direction.y)));
            rotation = glm::vec3(x, 0.0, z);
        }

        void look_at(const glm::vec3& target) {
            glm::vec3 direction = target - position;
            look_direction(direction);
        }

        glm::mat4 rotation_matrix() const {
            glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
            glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 rotZ = glm::rotate(glm::mat4(1.0f), rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
            return rotZ * rotX * rotY;
        }

        glm::mat4 translation_matrix() const {
            glm::mat4 translation = glm::mat4(1.0f);
            translation[3][0] = position.x;
            translation[3][1] = position.y;
            translation[3][2] = position.z;
            return translation;
        }

        glm::mat4 pixel_to_ray_matrix() const {
            float aspect = extent.x / extent.y;
            float tan_fov = tan(glm::radians(fov * 0.5f));

            glm::mat4 center_pixel = glm::mat4(1.0f);
            center_pixel[2][0] = 0.5; // m13
            center_pixel[2][1] = 0.5; // m23

            glm::mat4 pixel_to_uv = glm::mat4(1.0f);
            pixel_to_uv[0][0] = 2.0f / extent.x;  // m11
            pixel_to_uv[1][1] = -2.0f / extent.y; // m22
            pixel_to_uv[2][0] = -1.0f;            // m13
            pixel_to_uv[2][1] = 1.0f;             // m23

            glm::mat4 uv_to_view = glm::mat4(1.0f);
            uv_to_view[0][0] = tan_fov * std::max(1.0f, aspect);
            uv_to_view[1][1] = tan_fov / std::min(1.0f, aspect);

            glm::mat4 swap_yz = glm::mat4(1.0f);
            swap_yz[1][1] = 0.0f;
            swap_yz[2][2] = 0.0f;
            swap_yz[1][2] = 1.0f;
            swap_yz[2][1] = 1.0f;

            glm::mat4 rotationMat = rotation_matrix();
            glm::mat4 translationMat = translation_matrix();

            return translationMat * rotationMat * swap_yz * uv_to_view * pixel_to_uv * center_pixel;
        }
    public:
        glm::vec3 position;
        glm::vec3 rotation; // rotation in radians
        glm::vec2 extent;
        float fov; // in degrees
    };
}
