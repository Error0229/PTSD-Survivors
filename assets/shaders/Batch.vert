layout(location = 0) in vec3 vertPosition;
layout(location = 1) in vec2 vertUv;

out vec2 uv;

uniform mat4 viewProjection;

void main() {
    gl_Position = viewProjection * vec4(vertPosition, 1);
    uv = vertUv;
}
