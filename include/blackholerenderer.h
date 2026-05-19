#ifndef BLACKHOLERENDERER_H
#define BLACKHOLERENDERER_H

#include "shader.h"
#include "camera.h"
#include "particlesystem.h"
#include "postprocessor.h"
#include "blackhole.h"

class BlackHoleRenderer {
public:
    BlackHoleRenderer(int width, int height);
    void update(float dt);
    void render(Camera &camera, float time);
    void handleResize(int width, int height);

private:
    BlackHole blackHole;
    ParticleSystem particleSystem;
    PostProcessor postProcessor;
    
    Shader progBG;
    Shader progPart;
    Shader progDist;
    Shader progFinal;
    Shader progBlur;
    Shader progBright;

    void renderScene(Camera &camera, float time);
};

#endif
