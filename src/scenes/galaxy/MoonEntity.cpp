
#include <clay/application/desktop/AppDesktop.h>
#include "scenes/galaxy/MoonEntity.h"

namespace galaxy {

MoonEntity::MoonEntity(clay::BaseScene& scene, clay::ecs::EntityManager& entityManager)
    : mEntityManager_(entityManager) {
    mEntityId_ = mEntityManager_.createEntity();
    clay::ecs::Transform transform;
    transform.mPosition_ = {5 + mOrbitRadius_, 0.f, 0.f};
    mEntityManager_.addTransform(mEntityId_, transform);
    clay::ecs::ModelRenderable modelRenderable;
    modelRenderable.mColor_ = {1,1,1,1};
    modelRenderable.modelHandle = scene.getApp().getResources().getHandle<clay::Model>("Moon");

    // translation matrix
    glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), {0,0,0});
    //rotation matrix
    glm::mat4 rotationMat = glm::identity<glm::mat4>();
    // scale matrix
    glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), {mRadius_, mRadius_, mRadius_});

    modelRenderable.localModelMat = translationMat * rotationMat * scaleMat;
    mEntityManager_.addModelRenderable(mEntityId_, modelRenderable);
}

MoonEntity::~MoonEntity() {}

float MoonEntity::getRadius() const {
    return mRadius_;
}

void MoonEntity::setRadius(float radius) {
    mRadius_ = radius;
    // TODO update renderable's local scale
    // mEntityManager_.mModelRenderable[mEntityId_]. 
}

glm::vec4 MoonEntity::getColor() const {
    return mEntityManager_.mModelRenderable[mEntityId_].mColor_;
}

void MoonEntity::setColor(const glm::vec4& color) {
    mEntityManager_.mModelRenderable[mEntityId_].mColor_ = color;
}

float MoonEntity::getRotationSpeed() const {
    return mRotationSpeed_;
}

void MoonEntity::setRotationSpeed(float rotation) {
    mRotationSpeed_ = rotation;
}

void MoonEntity::render(VkCommandBuffer cmdBuffer, clay::Resources& resources) {
    // clay::ecs::Transform& transform = mEntityManager_.mTransforms[mEntityId_];
    // clay::ecs::ModelRenderable& model = mEntityManager_.mModelRenderable[mEntityId_];

    // glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), transform.mPosition_);
    // const glm::mat4 rotationMatrix = glm::mat4_cast(transform.mOrientation_);
    // glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), transform.mScale_);

    // struct PushConstants {
    //     glm::mat4 model;
    //     glm::vec4 color; // optional depending on material
    // } push;
    // push.model = translationMat * rotationMatrix * scaleMat * model.localModelMat;
    // push.color = model.mColor_;

    // model.mModel_->render(cmdBuffer, &push, sizeof(push));
}

void MoonEntity::setPosition(const glm::vec3& newPosition) {
    mEntityManager_.mTransforms[mEntityId_].mPosition_ = newPosition;
}

glm::vec3 MoonEntity::getPosition() const {
    return mEntityManager_.mTransforms[mEntityId_].mPosition_;
}

} // namespace galaxy
