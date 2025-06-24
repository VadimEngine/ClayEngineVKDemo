
#include <clay/application/desktop/AppDesktop.h>
#include "scenes/galaxy/MoonEntity.h"

namespace galaxy {

MoonEntity::MoonEntity(clay::BaseScene& scene) {
    mpRenderable_ = new clay::ModelRenderable(
        scene.getApp().getResources().getResource<clay::Model>("SolidTorus")
    );
    addRenderable(mpRenderable_);
}

MoonEntity::~MoonEntity() {}

float MoonEntity::getRadius() const {
    return mRadius_;
}

void MoonEntity::setRadius(float radius) {
    mRadius_ = radius;
    mpRenderable_->setScale({mRadius_, mRadius_, mRadius_});
}

glm::vec4 MoonEntity::getColor() const {
    return mpRenderable_->getColor();
}

void MoonEntity::setColor(const glm::vec4& color) {
    mpRenderable_->setColor(color);
}

float MoonEntity::getRotationSpeed() const {
    return mRotationSpeed_;
}

void MoonEntity::setRotationSpeed(float rotation) {
    mRotationSpeed_ = rotation;
}

} // namespace galaxy
