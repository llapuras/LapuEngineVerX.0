#version 330 core

#define ITERATIONS_RAYMARCH 33
#define ITERATIONS_NORMAL 25
#define WATER_DEPTH 2

out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoords;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;
uniform sampler2D depthMap;
uniform float heightScale;
uniform float time;
uniform float DRAG_MULT = 0.1;

//camera
uniform vec3 viewPos;

vec2 _uv;
vec2 texCoords = fs_in.TexCoords;
float focus = 5.;
float depth = time;
float dist = smoothstep(0.0, 2., depth-focus)*3.*dFdx(texCoords.x);
vec3 color = vec3(1.0);


// returns vec2 with wave height in X and its derivative in Y
vec2 wavedx(vec2 position, vec2 direction, float speed, float frequency, float timeshift) {
    float x = dot(direction, position) * frequency + timeshift * speed;
    float wave = exp(sin(x) - 1.0);
    float dx = wave * cos(x);
    return vec2(wave, -dx);
}

float getwaves(vec2 position, int iterations){
	float iter = 0.0;
    float phase = 12.0;
    float speed = 2.0;
    float weight = 1.0;
    float w = 0.0;
    float ws = 0.0;
    for(int i=0;i<iterations;i++){
        vec2 p = vec2(sin(iter), cos(iter));
        vec2 res = wavedx(position, p, speed, phase, time);
        position += normalize(p) * res.y * weight * DRAG_MULT;
        w += res.x * weight;
        iter += 12.0;
        ws += weight;
        weight = mix(weight, 0.0, 0.2);
        phase *= 1.18;
        speed *= 1.07;
    }
    return w / ws;
}

float raymarchwater(vec3 camera, vec3 start, vec3 end, float depth){
    vec3 pos = start;
    float h = 0.0;
    float hupper = depth;
    float hlower = 0.0;
    vec2 zer = vec2(0.0);
    vec3 dir = normalize(end - start);
    for(int i=0;i<318;i++){
        h = getwaves(pos.xz * 0.1, ITERATIONS_RAYMARCH) * depth - depth;
        if(h + 0.01 > pos.y) {
            return distance(pos, viewPos);
        }
        pos += dir * (pos.y - h);
    }
    return -1.0;
}


float H = 0.0;
vec3 normal(vec2 pos, float e, float depth){
    vec2 ex = vec2(e, 0);
    H = getwaves(pos.xy * 0.1, ITERATIONS_NORMAL) * depth;
    vec3 a = vec3(pos.x, H, pos.y);
    return normalize(cross(normalize(a-vec3(pos.x - e, getwaves(pos.xy * 0.1 - ex.xy * 0.1, ITERATIONS_NORMAL) * depth, pos.y)), 
                           normalize(a-vec3(pos.x, getwaves(pos.xy * 0.1 + ex.yx * 0.1, ITERATIONS_NORMAL) * depth, pos.y + e))));
}

vec3 getRay(vec2 uv){
    uv = (uv - 2.0);
	vec3 proj = normalize(vec3(uv.x, uv.y, 1.0) + vec3(uv.x, uv.y, -1.0) * pow(length(uv), 2.0) * 0.05);	
	vec3 ray = proj;
    return ray;
}

float intersectPlane(vec3 origin, vec3 direction, vec3 point, vec3 normal)
{ 
    return clamp(dot(point - origin, normal) / dot(direction, normal), -1.0, 9991999.0); 
}

vec3 getatm(vec3 ray, float roughness){
    vec3 sharp = mix(vec3( 0.0293, 0.0698, 0.1717) * 10.0, vec3(3.0), pow(1.0 - ray.y, 8.0));
    vec3 rough = vec3(vec3( 0.0293, 0.0698, 0.1717) + vec3(1.0));
    return mix(sharp, rough, roughness);
}


vec3 getColor(vec2 uv){
	vec3 ray = getRay(uv);
    
    if(ray.y >= -0.01){
        vec3 C = getatm(ray, 0.0) * 1.0 * 2.0;
     	return C; 
    }
    
	vec3 wfloor = vec3(0.0, -WATER_DEPTH, 0.0);
	vec3 wceil = vec3(0.0, 0.0, 0.0);
	vec3 orig = vec3(0.0, 2.0, 0.0);
	float hihit = intersectPlane(orig, ray, wceil, vec3(0.0, 1.0, 0.0));
	float lohit = intersectPlane(orig, ray, wfloor, vec3(0.0, 1.0, 0.0));
    vec3 hipos = orig + ray * hihit;
    vec3 lopos = orig + ray * lohit;
	float dist = raymarchwater(orig, hipos, lopos, WATER_DEPTH);
    vec3 pos = orig + ray * dist;

	vec3 N = normal(pos.xz, 0.01, WATER_DEPTH);
    vec2 velocity = N.xz * (1.0 - N.y);
    vec3 R = reflect(ray, N);
    float roughness = 1.0 - 1.0 / (dist * 0.01 + 1.0);
    N = normalize(mix(N, vec3(0.0, 1.0, 0.0), roughness));
    R = normalize(mix(R, N, roughness));
    R.y = abs(R.y);
    float fresnel = (0.04 + (1.0-0.04)*(pow(1.0 - max(0.0, dot(-N, ray)), 5.0)));
	
    vec3 C = fresnel * (getatm(R, roughness)) * 2.0;
    
	return C;
}

vec3 gammacorrect(vec3 c){
    return pow(c, vec3(1.0 / 2.4));
}

vec3 render(vec2 uv){
 	vec3 ray = getRay(uv);
    vec3 C = getColor(uv) * 0.3;
    return gammacorrect(C);  
}

void main()
{           
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    texCoords = fs_in.TexCoords;

    vec3 normal = texture(normalMap, texCoords).rgb;
    normal = normalize(normal * 2.0 - 1.0);  
	
	_uv = texCoords + vec2(cos(time), sin(time))*dist;
    color += texture(diffuseMap, _uv).rgb;

    vec3 ambient = 0.1 * color;
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
    vec3 specular = vec3(0.2) * spec;

    //FragColor = vec4(ambient + diffuse + specular, 1.0);
	FragColor = vec4(render(texCoords),1.0) + vec4(ambient,1.0);
}
