#version 460 core

float near = 0.01;
float far = 100;

in vec3 sent_near_point;
in vec3 sent_far_point;
in mat4 sent_view;
in mat4 sent_proj;

out vec4 final_color;

// source: https://asliceofrendering.com/scene%20helper/2020/01/05/InfiniteGrid/

vec4 grid(vec3 fragPos3D, float scale)
{
    vec2 coord = fragPos3D.xz * scale;
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;

    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);

    vec4 color = vec4(0.2, 0.2, 0.2, 1.0 - min(line, 1.0));

    if(fragPos3D.x > -0.1 * minimumx && fragPos3D.x < 0.1 * minimumx)
    {
        color.z = 1.0;
    }

    if(fragPos3D.z > -0.1 * minimumz && fragPos3D.z < 0.1 * minimumz)
    {
        color.x = 1.0;
    }

    return color;
}

float computeLinearDepth(float depth, vec3 pos)
{
    float clip_space_depth = (depth) * 2.0 - 1.0;
    float linearDepth = (2.0 * near * far) / (far + near - clip_space_depth * (far - near));
    return linearDepth / far;
}

void main()
{
    float t = -sent_near_point.y / (sent_far_point.y - sent_near_point.y);
    vec3 fragPos3D = sent_near_point + t * (sent_far_point - sent_near_point);

    vec4 clip_space_pos = sent_proj * sent_view * vec4(fragPos3D.xyz, 1.0);

    float depth = clip_space_pos.z / clip_space_pos.w;

    float linearDepth = computeLinearDepth(depth, fragPos3D);
    float fading = max(0, (0.5 - linearDepth));

    gl_FragDepth = depth;

    final_color = (grid(fragPos3D, 10) + grid(fragPos3D, 1)) * float(t > 0);
    final_color.a *= fading;
}
