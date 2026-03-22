in vec2 uv;

out vec4 fragColor;

uniform sampler2D surface;

void main() {
    vec4 texColor = texture(surface, uv);

    if (texColor.a < 0.01)
        discard;

    fragColor = texColor;
}
