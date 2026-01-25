#version 330 core
layout (location = 0) in vec2 aPos;

uniform vec2 position;
uniform vec2 screenSize;
uniform float radius;

void main() {
    vec2 cirPos = aPos * radius + position;
    vec2 normalCoor = (cirPos / screenSize) * 2.0f - 1.0f; // Normalize to 0-1. Scale to 0-2 and shift to range -1 to 1
    
    normalCoor.y = -normalCoor.y;

    gl_Position = vec4(normalCoor, 0.0, 1.0);
}