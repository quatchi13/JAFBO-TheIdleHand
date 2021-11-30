#version 410

layout(location = 0) in vec3 inPos1;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec3 inNorm1;
layout(location = 3) in vec2 inUV;
layout(location = 4) in vec3 inPos2;
layout(location = 5) in vec3 inNorm2;

layout(location = 0) out vec3 outWorldPos;
layout(location = 1) out vec3 outColor;
layout(location = 2) out vec3 outNormal;
layout(location = 3) out vec2 outUV;

// Complete MVP
uniform mat4 u_ModelViewProjection;
// Just the model transform, we'll do worldspace lighting
uniform mat4 u_Model;
// Normal Matrix for transforming normals
uniform mat3 u_NormalMatrix;

//a uniform so that we can perform LERP
uniform float u_t;

void main() {

	gl_Position = u_ModelViewProjection * vec4(mix(inPos1, inPos2, u_t), 1.0);

	// Lecture 5
	// Pass vertex pos in world space to frag shader
	outWorldPos = (u_Model * vec4((mix(inPos1, inPos2, u_t), 1.0))).xyz;//mix(inPos1, inPos2, u_t), 1.0))).xyz;

	// Normals
	outNormal = u_NormalMatrix * mix(inNorm1, inNorm2, u_t);

	// Pass our UV coords to the fragment shader
	outUV = inUV;

	///////////
	outColor = inColor;

	
}