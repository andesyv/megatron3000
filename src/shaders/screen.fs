#version 430

in vec2 fragCoord;

uniform mat4 MVP = mat4(1.0);

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

        vec3 p = rayOrigin + rayDir * bounds.x;
        fragColor = vec4(abs(p), 1.);
        return;
    }

    fragColor = vec4(abs(rayDir), 1.);
}