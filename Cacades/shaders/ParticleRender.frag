#version 330 core

in ParticleQuad
{
    flat int type;
    vec2 uv;
} fs_in;

out vec4 FragColor;

#pragma include "EnumParticleType.glh"

void main()
{
    switch(fs_in.type)
    {
      case PARTICLE_RAY: return;
      case PARTICLE_EMITTER: FragColor = vec4(1, 1, 0, 1); break;
      case PARTICLE_WATER_FLOWING:
      case PARTICLE_WATER_FALLING: FragColor = vec4(0, 0, 0.4, 1);  break;
      case PARTICLE_MIST_COLLISION:
      case PARTICLE_MIST_FALLING: FragColor = vec4(1, 1, 1, 1);  break;
      default: FragColor = vec4(0, 1, 1, 1); break;
    }
}