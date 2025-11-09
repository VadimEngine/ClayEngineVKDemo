
#include <clay/application/desktop/AppDesktop.h>
#include "scenes/galaxy/SunEntity.h"

namespace galaxy {

SunEntity::SunEntity(clay::BaseScene& scene, clay::ecs::EntityManager& entityManager) 
    : mEntityManager_(entityManager) {
    mEntityId_ = mEntityManager_.createEntity();
    clay::ecs::Transform transform;
    transform.mPosition_ = {0.f, 0.f, 0.f};
    mEntityManager_.addTransform(mEntityId_, transform);
    clay::ecs::ModelRenderable modelRenderable;
    modelRenderable.mColor_ = {1,1,1,1};
    modelRenderable.modelHandle = scene.getApp().getResources().getHandle<clay::Model>("Sun");

    // translation matrix
    glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), {0,0,0});
    //rotation matrix
    glm::mat4 rotationMat = glm::mat4_cast(glm::angleAxis(glm::radians(20.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
    // scale matrix
    glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), {mRadius_, mRadius_, mRadius_});

    modelRenderable.localModelMat = translationMat * rotationMat * scaleMat;
    mEntityManager_.addModelRenderable(mEntityId_, modelRenderable);
}

SunEntity::~SunEntity() {}

float SunEntity::getRadius() const {
    return mRadius_;
}

void SunEntity::setRadius(float radius) {
    mRadius_ = radius;
    // TODO update renderable's local scale
    //mpRenderable_->setScale({mRadius_, mRadius_, mRadius_});
}

glm::vec4 SunEntity::getColor() const {
    return mEntityManager_.mModelRenderable[mEntityId_].mColor_;
}

void SunEntity::setColor(const glm::vec4& color) {
    mEntityManager_.mModelRenderable[mEntityId_].mColor_ = color;
}

float SunEntity::getRotationSpeed() const {
    return mRotationSpeed_;
}

void SunEntity::setRotationSpeed(float rotation) {
    mRotationSpeed_ = rotation;
}

void SunEntity::render(vk::CommandBuffer cmdBuffer, clay::Resources& resources) {}

void SunEntity::setPosition(const glm::vec3& newPosition) {
    mEntityManager_.mTransforms[mEntityId_].mPosition_ = newPosition;
}

glm::vec3 SunEntity::getPosition() const {
    return mEntityManager_.mTransforms[mEntityId_].mPosition_;
}

} // namespace galaxy
