#version 430

layout(location = 0) in vec3 inWorldPos;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNormal;
layout(location = 3) in vec2 inUV;

// We output a single color to the color buffer
layout(location = 0) out vec4 frag_color;

////////////////////////////////////////////////////////////////
/////////////// Instance Level Uniforms ////////////////////////
////////////////////////////////////////////////////////////////

// Represents a collection of attributes that would define a material
// For instance, you can think of this like material settings in 
// Unity
struct Material {
	sampler2D Diffuse;
	float     Shininess;
};
// Create a uniform for the material
uniform Material u_Material;

////////////////////////////////////////////////////////////////
///////////// Application Level Uniforms ///////////////////////
////////////////////////////////////////////////////////////////

#include "fragments/multiple_point_lights.glsl"

////////////////////////////////////////////////////////////////
/////////////// Frame Level Uniforms ///////////////////////////
////////////////////////////////////////////////////////////////

// The position of the camera in world space
uniform vec3  u_CamPos;

// https://learnopengl.com/Advanced-Lighting/Advanced-Lighting


//calculates the rim lighting value based on a given diffuse vector
vec3 CalcRimLight(vec4 diffuseVector){
	//first we will calculate the intensity of the rim lighting effect
	vec3 eyeVector = normalize(-inWorldPos);
	float rimLightIntensity = dot(eyeVector, -inNormal);
	rimLightIntensity = 1.0 - rimLightIntensity;

	float exposure = 0.5;
	rimLightIntensity *= exposure;
	
	//now we multiply the diffuse value of the fragment by the intensity
	vec3 rim = vec3(rimLightIntensity * (diffuseVector.rgb));
	//that's all we're gonna do here :)
	return rim;
}


void main() {
	// Normalize our input normal
	vec3 normal = normalize(inNormal);

	// Use the lighting calculation that we included from our partial file
	vec3 lightAccumulation = CalcAllLightContribution(inWorldPos, normal, u_CamPos, u_Material.Shininess);

	// Get the albedo from the diffuse / albedo map
	vec4 textureColor = texture(u_Material.Diffuse, inUV);

	// combine for the final result
	vec3 result = lightAccumulation  * inColor * textureColor.rgb;
	
	vec3 rimLighting = CalcRimLight(textureColor);
	result += rimLighting;

	frag_color = vec4(result, textureColor.a);
}