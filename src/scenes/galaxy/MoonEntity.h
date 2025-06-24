
#include <clay/application/common/BaseScene.h>
#include <clay/entity/Entity.h>
#include <clay/entity/render/ModelRenderable.h>

namespace galaxy {

class MoonEntity : public clay::Entity {
public:
    MoonEntity(clay::BaseScene& scene);

    ~MoonEntity();

    float getRadius() const;

    void setRadius(float radius);

    glm::vec4 getColor() const;

    void setColor(const glm::vec4& color);

    float getRotationSpeed() const;

    void setRotationSpeed(float rotation);

private:
    float mRadius_ = 0.25f;
    float mOrbitRadius_ = 1.f;
    float mRotationSpeed_ = 12.f;
    clay::ModelRenderable* mpRenderable_ = nullptr;
};
    
} // namespace galaxy
