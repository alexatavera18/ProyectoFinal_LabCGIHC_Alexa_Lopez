#version 330 core
#define MAX_POINT_LIGHTS 128

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

uniform DirLight dirLight;
uniform int numLights;                         // cantidad real que subes desde C++
uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform Material material;
uniform float alphaMul;                        // multiplicador extra para alpha
uniform vec3 viewPos;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 color;

// Luz direccional
vec3 CalcDirLight(DirLight L, vec3 N, vec3 V, vec3 kd, vec3 ks)
{
    vec3 Ldir = normalize(-L.direction);
    float diff = max(dot(N, Ldir), 0.0);

    vec3 R = reflect(-Ldir, N);
    float spec = pow(max(dot(V, R), 0.0), material.shininess);

    vec3 amb = L.ambient * kd;
    vec3 dif = L.diffuse * diff * kd;
    vec3 spe = L.specular * spec * ks;
    return amb + dif + spe;
}

// Luz puntual
vec3 CalcPointLight(PointLight L, vec3 N, vec3 P, vec3 V, vec3 kd, vec3 ks)
{
    vec3 Ldir = normalize(L.position - P);
    float dist = length(L.position - P);
    float att  = 1.0 / (L.constant + L.linear * dist + L.quadratic * dist * dist);

    float diff = max(dot(N, Ldir), 0.0);
    vec3 R = reflect(-Ldir, N);
    float spec = pow(max(dot(V, R), 0.0), material.shininess);

    vec3 amb = L.ambient * kd;
    vec3 dif = L.diffuse * diff * kd;
    vec3 spe = L.specular * spec * ks;
    return (amb + dif + spe) * att;
}

void main()
{
    // Texturas y vectores base
    vec4 dTex = texture(material.diffuse, TexCoords);
    vec3 kd   = dTex.rgb;
    vec3 ks   = texture(material.specular, TexCoords).rgb;

    vec3 N = normalize(Normal);
    vec3 V = normalize(viewPos - FragPos);

    // Iluminacion direccional (tipo atardecer, piso ambiental)
    // Ajusta el "ambientFloor" si aún lo ves muy oscuro o muy brillante.
    const vec3 ambientFloor = vec3(0.07, 0.06, 0.05);
    vec3 dirCol = CalcDirLight(dirLight, N, V, kd, ks);
    vec3 result = max(dirCol, ambientFloor * kd);

    // Puntos
    int count = clamp(numLights, 0, MAX_POINT_LIGHTS);
    for (int i = 0; i < count; ++i)
        result += CalcPointLight(pointLights[i], N, FragPos, V, kd, ks);

    // Alpha
    float a = clamp(dTex.a * alphaMul, 0.0, 1.0);
    color = vec4(result, a);
}
