#version 430

#define RAYMARCH_STEPS 100
#define EPSILON 0.001

in vec2 fragCoord;

uniform mat4 MVP = mat4(1.0);
layout(binding = 0) uniform sampler3D volume;

out vec4 fragColor;

// https://www.iquilezles.org/www/articles/intersectors/intersectors.htm
vec2 boxIntersection(vec3 ro, vec3 rd, vec3 boxSize) 
{
    vec3 m = 1.0/rd;
    vec3 n = m*ro;
    vec3 k = abs(m)*boxSize;
    vec3 t1 = -n - k;
    vec3 t2 = -n + k;
    float tN = max( max( t1.x, t1.y ), t1.z );
    float tF = min( min( t2.x, t2.y ), t2.z );
    if( tN>tF || tF < 0.0) return vec2(-1.0); // no intersection
    // outNormal = -sign(rdd)*step(t1.yzx,t1.xyz)*step(t1.zxy,t1.xyz);
    return vec2( tN, tF );
}

float tf(vec3 p) {
    p += 0.5; // Shift uv's so we go from [-0.5, 0.5] to [0, 1.0]
    return texture(volume, p).r * 0.1;
}

void main() {
    vec4 near = MVP * vec4(fragCoord, -1., 1.);
    near /= near.w;

    vec4 far = MVP * vec4(fragCoord, 1., 1.);
    far /= far.w;

    vec3 rayOrigin = near.xyz;
    vec3 rayDir = normalize(far.xyz - near.xyz);

    // Example bounding cube:
    vec2 bounds = boxIntersection(rayOrigin, rayDir, vec3(1., 1., 1.));
    if (0.0 <= bounds.y) {
        // Clamp to near plane
        bounds.x = max(bounds.x, 0.0);

        const float stepSize = (bounds.y - bounds.x) / float(RAYMARCH_STEPS);
        float depth = bounds.x;
        fragColor = vec4(0.0, 0.0, 0.0, 0.0);

        for (int i = 0; i < RAYMARCH_STEPS; ++i) {
            vec3 p = rayOrigin + rayDir * depth;
            float density = tf(p);
            vec3 color = vec3(0.8, 0.7, 0.2); // Uniform color over volume
            fragColor.rgb += (1.0 - fragColor.a) * color * density;
            fragColor.a += density;

            // Early exit
            if (1.0 < fragColor.a) {
                return;
            }
            depth += stepSize;
        }
    }

    if (fragColor.a < 0.1)
        fragColor = vec4(abs(rayDir), 1.);
}