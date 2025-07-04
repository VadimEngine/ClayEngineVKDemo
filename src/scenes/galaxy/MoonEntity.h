
#pragma once
// clay
#include <clay/ecs/EntityManager.h>
#include <clay/application/common/BaseScene.h>

namespace galaxy {

class MoonEntity {
public:
    MoonEntity(clay::BaseScene& scene, clay::ecs::EntityManager& entityManager);

    ~MoonEntity();

    float getRadius() const;

    void setRadius(float radius);

    glm::vec4 getColor() const;

    void setColor(const glm::vec4& color);

    float getRotationSpeed() const;

    void setRotationSpeed(float rotation);

    void render(VkCommandBuffer cmdBuffer, clay::Resources& resources);

    void setPosition(const glm::vec3& newPosition);

    glm::vec3 getPosition() const;

private:
    clay::ecs::EntityManager& mEntityManager_;
    clay::ecs::Entity mEntityId_;

    float mRadius_ = 0.25f;
    float mOrbitRadius_ = 1.f;
    float mRotationSpeed_ = 12.f;
};
    
} // namespace galaxy
