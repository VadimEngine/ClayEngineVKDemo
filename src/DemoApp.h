#pragma once
// clay
#include <clay/application/desktop/AppDesktop.h>

class DemoApp : public clay::AppDesktop {
public:
    struct AppProp{
        struct Meshes {
            clay::Handle<clay::Mesh> sphere;
            clay::Handle<clay::Mesh> cube;
            clay::Handle<clay::Mesh> plane;
            clay::Handle<clay::Mesh> torus;
            clay::Handle<clay::Mesh> planeCircle;
            clay::Handle<clay::Mesh> human;
        } mMeshes;

        struct Materials {
            clay::Handle<clay::Material> solid;
            clay::Handle<clay::Material> flat;
            clay::Handle<clay::Material> vTexture;
            clay::Handle<clay::Material> spriteSheet;
            clay::Handle<clay::Material> tilemapInstanced;
            clay::Handle<clay::Material> rpgMap;
            clay::Handle<clay::Material> moon;
            clay::Handle<clay::Material> sun;
            clay::Handle<clay::Material> earth;
            clay::Handle<clay::Material> stars;
            clay::Handle<clay::Material> cloudSkyNoDepth;
            clay::Handle<clay::Material> cloudSkyDepth;
            clay::Handle<clay::Material> animation;
        } mMaterials;

        struct Models {
            clay::Handle<clay::Model> solidSphere;
            clay::Handle<clay::Model> vSphere;
            clay::Handle<clay::Model> solidPlane;
            clay::Handle<clay::Model> solidTorus;
            clay::Handle<clay::Model> sun;
            clay::Handle<clay::Model> moon;
            clay::Handle<clay::Model> earth;
            clay::Handle<clay::Model> solidCircle;
            clay::Handle<clay::Model> cube;
            clay::Handle<clay::Model> human;
            clay::Handle<clay::Model> skyPlane;
        } mModels;
    } mAppProp;


    DemoApp(clay::Window& window);

    ~DemoApp();

    void loadResources() override;
};
