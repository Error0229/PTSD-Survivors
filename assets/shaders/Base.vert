layout(location = 0) in vec2 vertPosition;
layout(location = 1) in vec2 vertUv;

out vec2 uv;

layout(std140) uniform Matrices {
    mat4 model;
    mat4 viewProjection;
};

void main() {
    gl_Position = viewProjection * model * vec4(vertPosition, 0, 1);
    uv = vertUv;
}
