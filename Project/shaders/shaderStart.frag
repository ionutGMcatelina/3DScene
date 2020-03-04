#version 410 core

in vec3 normal;
in vec4 fragPosEye;
in vec4 fragPosLightSpace;
in vec2 fragTexCoords;
in vec4 position;

out vec4 fColor;

//lighting
uniform	mat3 normalMatrix;
uniform mat3 lightDirMatrix;
uniform	vec3 lightColor;
uniform	vec3 lightDir;

uniform	vec3 spotPos;
uniform bool spotlightOn;

uniform sampler2D diffuseTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowMap;

uniform vec3 ConeDirection; // adding spotlight attributes 

float SpotCosCutoff = 0.97f; // how wide the spot is, as a cosine 
float SpotExponent = 2.0f; // control light fall-off in the spot

vec3 ambient;
float ambientStrength = 0.2f;
vec3 diffuse;
vec3 specular;
float specularStrength = 0.5f;
float shininess = 64.0f;
float ConstantAttenuation = 2.0f;
float LinearAttenuation = 1.0f;
float QuadraticAttenuation = 1.0f;

vec3 o;

void computeLightComponents()
{		
	vec3 cameraPosEye = vec3(0.0f);//in eye coordinates, the viewer is situated at the origin
	
	//transform normal
	vec3 normalEye = normalize(normalMatrix * normal);	
	
	//compute light direction
	vec3 lightDirN = normalize(lightDirMatrix * lightDir);	

	//compute view direction 
	vec3 viewDirN = normalize(cameraPosEye - fragPosEye.xyz);
	
	//compute half vector
	vec3 halfVector = normalize(lightDirN + viewDirN);
		
	//compute ambient light
	ambient = ambientStrength * lightColor;
	
	//compute diffuse light
	diffuse = max(dot(normalEye, lightDirN), 0.0f) * lightColor;
	
	//compute specular light
	float specCoeff = pow(max(dot(halfVector, normalEye), 0.0f), shininess);
	specular = specularStrength * specCoeff * lightColor;
}

float computeFog() { 
	float fogDensity = 0.005f; 
	float fragmentDistance = length(fragPosEye);
	float fogFactor = exp(-pow(fragmentDistance * fogDensity, 2)); 
	return clamp(fogFactor, 0.0f, 1.0f); 
}

float computeShadow()
{	
	// perform perspective divide
    vec3 normalizedCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    if(normalizedCoords.z > 1.0f)
        return 0.0f;
    // Transform to [0,1] range
    normalizedCoords = normalizedCoords * 0.5f + 0.5f;
    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, normalizedCoords.xy).r;    
    // Get depth of current fragment from light's perspective
    float currentDepth = normalizedCoords.z;
    // Check whether current frag pos is in shadow
    float bias = 0.005f;
    float shadow = currentDepth - bias > closestDepth  ? 1.0f : 0.0f;

    return shadow;	
}

void main() 
{
	computeLightComponents();
	
	float shadow = computeShadow();
	
	//modulate with diffuse map
	ambient *= vec3(texture(diffuseTexture, fragTexCoords));
	diffuse *= vec3(texture(diffuseTexture, fragTexCoords));
	//modulate woth specular map
	specular *= vec3(texture(specularTexture, fragTexCoords));
	
	//modulate with shadow
	vec3 color = min((ambient + (1.0f - shadow)*diffuse) + (1.0f - shadow)*specular, 1.0f);
    
	
	vec3 lightDirection = spotPos - position.xyz; 
	float lightDistance = length(lightDirection); 
	lightDirection = lightDirection / lightDistance;
	
	float fogFactor = computeFog();
	
	float attenuation = (ConstantAttenuation + LinearAttenuation * lightDistance + QuadraticAttenuation * lightDistance * lightDistance);
		// how close are we to being in the spot?
	float spotCos = dot(lightDirection, -ConeDirection);
	float epsilon = spotCos - SpotCosCutoff;
	
	
	// attenuate more, based on spot-relative position
	if (spotCos < SpotCosCutoff)
		attenuation = 0.0f;
	else
		//attenuation *= abs(pow(spotCos, SpotExponent));
		attenuation *= clamp((spotCos - SpotCosCutoff) / SpotCosCutoff, 0.0, 1.0);
	
	vec4 fogColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);

	//compute view direction 
	vec3 viewDirN = normalize(vec3(0.0f) - fragPosEye.xyz);
	vec3 halfVector = normalize(lightDirection + viewDirN);
	float spotDiffuse = max(0.0, dot(normal, lightDirection)); 
	float spotSpecular = max(0.0, dot(normal, halfVector));

	
	if (spotDiffuse == 0.0) 
		spotSpecular = 0.0;
	else 
		spotSpecular = pow(spotSpecular, shininess) * specularStrength;
	vec3 scatteredLight = ambient + lightColor * spotDiffuse * attenuation; 
	vec3 reflectedLight = lightColor * spotSpecular * attenuation; 
	vec3 rgb = min(ambient * scatteredLight, vec3(1.0));
	
	if (spotlightOn)
		color += rgb;
	fColor = fogColor * (1 - fogFactor) + vec4(color, 1.0f) * fogFactor;
}
