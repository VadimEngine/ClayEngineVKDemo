#include <clay/application/desktop/AppDesktop.h>
#include "scenes/galaxy/PlanetEntity.h"

namespace galaxy {

PlanetEntity::PlanetEntity(clay::BaseScene& scene, clay::ecs::EntityManager& entityManager)
    : mEntityManager_(entityManager) {
    mEntityId_ = mEntityManager_.createEntity();
    clay::ecs::Transform transform;
    transform.mPosition_ = {mOrbitRadius_, 0.f, 0.f};
    mEntityManager_.addTransform(mEntityId_, transform);
    clay::ecs::ModelRenderable modelRenderable;
    modelRenderable.mColor_ = {1,1,1,1};
    modelRenderable.modelHandle = scene.getApp().getResources().mModelsPool_.getHandle("Earth");

    // translation matrix
    glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), {0,0,0});
    //rotation matrix
    glm::mat4 rotationMat = glm::mat4_cast(glm::angleAxis(glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    // scale matrix
    glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), {mRadius_, mRadius_, mRadius_});

    modelRenderable.localModelMat = translationMat * rotationMat * scaleMat;
    mEntityManager_.addModelRenderable(mEntityId_, modelRenderable);
}

PlanetEntity::~PlanetEntity() {

}

float PlanetEntity::getRadius() const {
    return mRadius_;
}

void PlanetEntity::setRadius(float radius) {
    mRadius_ = radius;
    // TODO update renderable's local scale
    //mpRenderable_->setScale({mRadius_, mRadius_, mRadius_});
}

glm::vec4 PlanetEntity::getColor() const {
    return mEntityManager_.mModelRenderable[mEntityId_].mColor_;
}

void PlanetEntity::setColor(const glm::vec4& color) {
    mEntityManager_.mModelRenderable[mEntityId_].mColor_ = color;
}

float PlanetEntity::getRotationSpeed() const {
    return mRotationSpeed_;
}

void PlanetEntity::setRotationSpeed(float rotation) {
    mRotationSpeed_ = rotation;
}

void PlanetEntity::render(VkCommandBuffer cmdBuffer, clay::Resources& resources) {

}

void PlanetEntity::setPosition(const glm::vec3& newPosition) {
    mEntityManager_.mTransforms[mEntityId_].mPosition_ = newPosition;
}

glm::vec3 PlanetEntity::getPosition() const {
    return mEntityManager_.mTransforms[mEntityId_].mPosition_;
}

} // namespace galaxy
