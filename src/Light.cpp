#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

class Light {
public:
    glm::vec3 position;
    glm::vec3 color;

    Light(glm::vec3 pos, glm::vec3 col) : position(pos), color(col) {}

    void setPosition(const glm::vec3& pos) {
        position = pos;
    }

    void setColor(const glm::vec3& col) {
        color = col;
    }
};
