#include <Logging.h>
#include <iostream>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <filesystem>
#include <json.hpp>
#include <fstream>
#include <sstream>
#include <typeindex>
#include <optional>
#include <string>

// GLM math library
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <GLM/gtx/common.hpp> // for fmod (floating modulus)

// Graphics
#include "Graphics/IndexBuffer.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/VertexArrayObject.h"
#include "Graphics/Shader.h"
#include "Graphics/Texture2D.h"
#include "Graphics/TextureCube.h"
#include "Graphics/VertexTypes.h"

// Utilities
#include "Utils/MeshBuilder.h"
#include "Utils/MeshFactory.h"
#include "Utils/ObjLoader.h"
#include "Utils/ImGuiHelper.h"
#include "Utils/ResourceManager/ResourceManager.h"
#include "Utils/FileHelpers.h"
#include "Utils/JsonGlmHelpers.h"
#include "Utils/StringUtils.h"
#include "Utils/GlmDefines.h"
#include "Utils/AudioEngine.h"
#include "Utils/AudioSource.h"

// Gameplay
#include "Gameplay/Material.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Scene.h"

// Components
#include "Gameplay/Components/IComponent.h"
#include "Gameplay/Components/Camera.h"
#include "Gameplay/Components/RotatingBehaviour.h"
#include "Gameplay/Components/JumpBehaviour.h"
#include "Gameplay/Components/RenderComponent.h"
#include "Gameplay/Components/MaterialSwapBehaviour.h"
#include "Gameplay/Components/MoveBehaviour.h"
#include "Gameplay/Components/InteractableObjectBehaviour.h"
#include "Gameplay/Components/SkinManager.h"
#include "Gameplay/Components/CharacterController.h"
#include "Gameplay/Components/MainMenu.h"
#include "Gameplay/Components/InterpolationBehaviour.h"
#include "Gameplay/Components/MorphRenderComponent.h"
#include "Gameplay/Components/MorphAnimationManager.h"
#include "Gameplay/Components/Pointer.h"
#include "Gameplay/Components/LocomotionBehaviour.h"
#include "Gameplay/Components/ObjectLinking.h"
#include "Gameplay/Components/SimpleScreenBehaviour.h"
#include "Gameplay/Components/WarpBehaviour.h"



// Physics
#include "Gameplay/Physics/RigidBody.h"
#include "Gameplay/Physics/Colliders/BoxCollider.h"
#include "Gameplay/Physics/Colliders/PlaneCollider.h"
#include "Gameplay/Physics/Colliders/SphereCollider.h"
#include "Gameplay/Physics/Colliders/ConvexMeshCollider.h"
#include "Gameplay/Physics/TriggerVolume.h"
#include "Graphics/DebugDraw.h"
#include "Gameplay/Components/TriggerVolumeEnterBehaviour.h"
#include "Gameplay/Components/SimpleCameraControl.h"


//#define LOG_GL_NOTIFICATIONS

/*
	Handles debug messages from OpenGL
	https://www.khronos.org/opengl/wiki/Debug_Output#Message_Components
	@param source    Which part of OpenGL dispatched the message
	@param type      The type of message (ex: error, performance issues, deprecated behavior)
	@param id        The ID of the error or message (to distinguish between different types of errors, like nullref or index out of range)
	@param severity  The severity of the message (from High to Notification)
	@param length    The length of the message
	@param message   The human readable message from OpenGL
	@param userParam The pointer we set with glDebugMessageCallback (should be the game pointer)
*/
void GlDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
	std::string sourceTxt;
	switch (source) {
		case GL_DEBUG_SOURCE_API: sourceTxt = "DEBUG"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: sourceTxt = "WINDOW"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceTxt = "SHADER"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY: sourceTxt = "THIRD PARTY"; break;
		case GL_DEBUG_SOURCE_APPLICATION: sourceTxt = "APP"; break;
		case GL_DEBUG_SOURCE_OTHER: default: sourceTxt = "OTHER"; break;
	}
	switch (severity) {
		case GL_DEBUG_SEVERITY_LOW:          LOG_INFO("[{}] {}", sourceTxt, message); break;
		case GL_DEBUG_SEVERITY_MEDIUM:       LOG_WARN("[{}] {}", sourceTxt, message); break;
		case GL_DEBUG_SEVERITY_HIGH:         LOG_ERROR("[{}] {}", sourceTxt, message); break;
			#ifdef LOG_GL_NOTIFICATIONS
		case GL_DEBUG_SEVERITY_NOTIFICATION: LOG_INFO("[{}] {}", sourceTxt, message); break;
			#endif
		default: break;
	}
}  

// Stores our GLFW window in a global variable for now
GLFWwindow* window;
// The current size of our window in pixels
glm::ivec2 windowSize = glm::ivec2(1600, 900);
// The title of our GLFW window
std::string windowTitle = "The Idle Hand";

// using namespace should generally be avoided, and if used, make sure it's ONLY in cpp files
using namespace Gameplay;
using namespace Gameplay::Physics;

// The scene that we will be rendering
Scene::Sptr scene = nullptr;

void GlfwWindowResizedCallback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	windowSize = glm::ivec2(width, height);
	if (windowSize.x * windowSize.y > 0) {
		scene->MainCamera->ResizeWindow(width, height);
	}
}

/// <summary>
/// Handles intializing GLFW, should be called before initGLAD, but after Logger::Init()
/// Also handles creating the GLFW window
/// </summary>
/// <returns>True if GLFW was initialized, false if otherwise</returns>
bool initGLFW() {
	// Initialize GLFW
	if (glfwInit() == GLFW_FALSE) {
		LOG_ERROR("Failed to initialize GLFW");
		return false;
	}

	//Create a new GLFW window and make it current
	window = glfwCreateWindow(windowSize.x, windowSize.y, windowTitle.c_str(), nullptr, nullptr);
	glfwMakeContextCurrent(window);
	
	// Set our window resized callback
	glfwSetWindowSizeCallback(window, GlfwWindowResizedCallback);

	return true;
}

/// <summary>
/// Handles initializing GLAD and preparing our GLFW window for OpenGL calls
/// </summary>
/// <returns>True if GLAD is loaded, false if there was an error</returns>
bool initGLAD() {
	if (gladLoadGLLoader((GLADloadproc)glfwGetProcAddress) == 0) {
		LOG_ERROR("Failed to initialize Glad");
		return false;
	}
	return true;
}

/// <summary>
/// Draws a widget for saving or loading our scene
/// </summary>
/// <param name="scene">Reference to scene pointer</param>
/// <param name="path">Reference to path string storage</param>
/// <returns>True if a new scene has been loaded</returns>
/// 
bool DrawSaveLoadImGui(Scene::Sptr& scene, std::string& path) {
	// Since we can change the internal capacity of an std::string,
	// we can do cool things like this!
	ImGui::InputText("Path", path.data(), path.capacity());

	// Draw a save button, and save when pressed
	if (ImGui::Button("Save")) {
		scene->Save(path);

		std::string newFilename = std::filesystem::path(path).stem().string() + "-manifest.json";
		ResourceManager::SaveManifest(newFilename);
	}
	ImGui::SameLine();
	// Load scene from file button
	if (ImGui::Button("Load")) {
		// Since it's a reference to a ptr, this will
		// overwrite the existing scene!
		scene = nullptr;

		std::string newFilename = std::filesystem::path(path).stem().string() + "-manifest.json";
		ResourceManager::LoadManifest(newFilename);
		scene = Scene::Load(path);

		return true;
	}
	return false;
}

/// <summary>
/// Draws some ImGui controls for the given light
/// </summary>
/// <param name="title">The title for the light's header</param>
/// <param name="light">The light to modify</param>
/// <returns>True if the parameters have changed, false if otherwise</returns>
bool DrawLightImGui(const Scene::Sptr& scene, const char* title, int ix) {
	bool isEdited = false;
	bool result = false;
	Light& light = scene->Lights[ix];
	ImGui::PushID(&light); // We can also use pointers as numbers for unique IDs
	if (ImGui::CollapsingHeader(title)) {
		isEdited |= ImGui::DragFloat3("Pos", &light.Position.x, 0.01f);
		isEdited |= ImGui::ColorEdit3("Col", &light.Color.r);
		isEdited |= ImGui::DragFloat("Range", &light.Range, 0.1f);

		result = ImGui::Button("Delete");
	}
	if (isEdited) {
		scene->SetShaderLight(ix);
	}

	ImGui::PopID();
	return result;
}


/// <summary>
/// Creates a material that can be applied to objects
/// </summary>
/// <param name="matName">: the name of your material </param>
/// <param name="matShader">: the shader being used to render the material (currently basic or reflective)</param>
/// <param name="matTex">: the texture being used in the material</param>
/// <param name="shine">: the shininess of the material (0.0: not shiny at all, 1.0: shiny)</param>
/// <returns>a material with all of these components</returns>
Material::Sptr MakeMaterial(std::string matName, Shader::Sptr matShader, Texture2D::Sptr matTex, float shine) {
	Material::Sptr mat = ResourceManager::CreateAsset<Material>();
	{
		mat->Name = matName;
		mat->MatShader = matShader;
		mat->Texture = matTex;
		mat->Shininess = shine;
	}
	return mat;
}

/// <summary>
/// Creates an interactable GameObject
/// </summary>
/// <param name="interactableName">: the name of the GameObject</param>
/// <param name="interactableMesh">: the mesh that will be rendered for the GameObject</param>
/// <param name="interactableMat">: the material that will be rendered for the GameObject</param>
/// <param name="reward">: the material that will be sent to the Skin Manager</param>
/// <param name="interactablePos">: the position of the GameObject</param>
/// <param name="interactableRot">: the rotation of the GameObject (defaults to none)</param>
/// <param name="interactableScale">: the scale of the GameObject(defaults to none)</param>
/// <param name="interactableRange">: the range from which the object can be interacted with (defaults to 1.5)</param>
/// <param name="colliderOffset">: the offset of the interaction space, relative to the GameObject's position (defaults to none)</param>
/// <returns> An interactable Game Object with the above specifications</returns>
GameObject::Sptr MakeInteractableObject(
	std::string interactableName, MeshResource::Sptr interactableMesh, Material::Sptr interactableMat, 
	Material::Sptr reward, glm::vec3 interactablePos, glm::vec3 interactableRot = glm::vec3(0.f, 0.f, 0.f), 
	glm::vec3 interactableScale = glm::vec3(1.f, 1.f, 1.f), float interactableRange = 1.5, 
	glm::vec3 colliderOffset = glm::vec3(0.f, 0.f, 0.f)) {
	GameObject::Sptr interactable = scene->CreateGameObject(interactableName);
	{
		//gives the object a position, rotation, and scale
		interactable->SetPosition(interactablePos);
		interactable->SetRotation(interactableRot);
		interactable->SetScale(interactableScale);
		
		//lets us render the object 
		RenderComponent::Sptr renderer = interactable->Add<RenderComponent>();
		renderer->SetMesh(interactableMesh);
		renderer->SetMaterial(interactableMat);

		//gives the object a physics body which will allow for collisions and other fun stuff
		RigidBody::Sptr physicsBody = interactable->Add<RigidBody>();
		physicsBody->AddCollider(ConvexMeshCollider::Create());
		
		//gives the object a trigger with its own collider
		TriggerVolume::Sptr volume = interactable->Add<TriggerVolume>();
		SphereCollider::Sptr collider = SphereCollider::Create(interactableRange);
		collider->SetPosition(colliderOffset);
		volume->AddCollider(collider);

		//give the object behaviours that are activated when the object is interacted with
		//(this is the stuff that registers and displays interactions when they occurr)
		InteractableObjectBehaviour::Sptr behaviourOnInteraction = interactable->Add<InteractableObjectBehaviour>();
		behaviourOnInteraction->AddRewardMaterial(reward);
	}

	return interactable;
}

/// <summary>
/// Creates a static game object that does absolutely nothing :)
/// </summary>
/// <param name="oName: ">the identifiable name for our object</param>
/// <param name="oMesh: ">our object's mesh</param>
/// <param name="oMaterial: ">our object's material</param>
/// <param name="position: ">the position of the object</param>
/// <param name="rotation: ">the rotation of the object</param>
/// <param name="scale: ">the scale of the object</param>
/// <returns>a game object with the above specifications</returns>
GameObject::Sptr MakeStaticObject(std::string oName, MeshResource::Sptr oMesh, Material::Sptr oMaterial, glm::vec3 position, glm::vec3 rotation, glm::vec3 scale) {
	GameObject::Sptr staticObj = scene->CreateGameObject(oName);
	{
		staticObj->SetPosition(position);
		staticObj->SetRotation(rotation);
		staticObj->SetScale(scale);

		RenderComponent::Sptr renderer = staticObj->Add<RenderComponent>();
		renderer->SetMesh(oMesh);
		renderer->SetMaterial(oMaterial);

		RigidBody::Sptr physicsBody = staticObj->Add<RigidBody>();
		physicsBody->AddCollider(ConvexMeshCollider::Create());
	}
	return staticObj;
}


/// <summary>
/// A quick way to fill in the bare bones basics of a plane object (the inevitable stuff)
/// </summary>
/// <param name="name">the name of the object</param>
/// <param name="x">the x position</param>
/// <param name="y">the y position</param>
/// <param name="z">the z position</param>
/// <param name="rx">the x rotation</param>
/// <param name="ry">the y rotation</param>
/// <param name="rz">the z rotation</param>
/// <param name="dx"> screen width</param>
/// <param name="dy"> screen height</param>
/// <param name="floor"> what floor is the object seen in</param>
/// <param name="material">the material of the object</param>
/// <returns></returns>
GameObject::Sptr MakeBasicPlane(std::string name, float x, float y, float z, float rx, float ry, float rz, float dx, float dy, float floor, Material::Sptr material)
{
	GameObject::Sptr basicPlane = scene->CreateGameObject(name);
	{
		// Make a big tiled mesh
		MeshResource::Sptr mesh = ResourceManager::CreateAsset<MeshResource>();
		mesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(dx, dy)));
		mesh->GenerateMesh();

		// Create and attach a RenderComponent to the object to draw our mesh
		RenderComponent::Sptr renderer = basicPlane->Add<RenderComponent>();
		renderer->SetMesh(mesh);
		renderer->SetMaterial(material);
		renderer->floorTag = floor;

		basicPlane->SetPosition(glm::vec3(x, y, z));
		basicPlane->SetRotation(glm::vec3(rx, ry, rz));
		
	}
	return basicPlane;
}

/// <summary>
/// A quick way to fill in the bare bones basics of an object (the inevitable stuff)
/// </summary>
/// <param name="name">the name of the object</param>
/// <param name="x">the x position</param>
/// <param name="y">the y position</param>
/// <param name="z">the z position</param>
/// <param name="rx">the x rotation</param>
/// <param name="ry">the y rotation</param>
/// <param name="rz">the z rotation</param>
/// <param name="floor"> what floor is the object seen in</param>
/// <param name="material">the material of the object</param>
/// <param name="mesh">the mesh of the object</param>
/// <returns></returns>
GameObject::Sptr MakeBasic(std::string name, float x, float y, float z, float rx, float ry, float rz, float floor, Material::Sptr material, MeshResource::Sptr mesh)
{
	GameObject::Sptr basic = scene->CreateGameObject(name);
	{
		// Create and attach a RenderComponent to the object to draw our mesh
		RenderComponent::Sptr renderer = basic->Add<RenderComponent>();
		renderer->SetMesh(mesh);
		renderer->SetMaterial(material);
		renderer->floorTag = floor;

		basic->SetPosition(glm::vec3(x, y, z));
		basic->SetRotation(glm::vec3(rx, ry, rz));

	}
	return basic;
}

int main() {
	Logger::Init(); // We'll borrow the logger from the toolkit, but we need to initialize it

	//Initialize GLFW
	if (!initGLFW())
		return 1;

	//Initialize GLAD
	if (!initGLAD())
		return 1;

	// Let OpenGL know that we want debug output, and route it to our handler function
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(GlDebugMessage, nullptr);

	// Initialize our ImGui helper
	ImGuiHelper::Init(window);

	// Initialize our resource manager
	ResourceManager::Init();

	// Register all our resource types so we can load them from manifest files
	ResourceManager::RegisterType<Texture2D>();
	ResourceManager::RegisterType<TextureCube>();
	ResourceManager::RegisterType<Shader>();
	ResourceManager::RegisterType<Material>();
	ResourceManager::RegisterType<MeshResource>();

	// Register all of our component types so we can load them from files
	ComponentManager::RegisterType<Camera>();
	ComponentManager::RegisterType<RenderComponent>();
	ComponentManager::RegisterType<RigidBody>();
	ComponentManager::RegisterType<TriggerVolume>();
	ComponentManager::RegisterType<RotatingBehaviour>();
	ComponentManager::RegisterType<JumpBehaviour>();
	ComponentManager::RegisterType<MaterialSwapBehaviour>();
	ComponentManager::RegisterType<TriggerVolumeEnterBehaviour>();
	ComponentManager::RegisterType<SimpleCameraControl>();
	ComponentManager::RegisterType<MoveBehaviour>();
	ComponentManager::RegisterType<InteractableObjectBehaviour>();
	ComponentManager::RegisterType<SkinManager>();
	ComponentManager::RegisterType<CharacterController>();
	ComponentManager::RegisterType<MainMenu>();
	ComponentManager::RegisterType<InterpolationBehaviour>();
	ComponentManager::RegisterType<MorphRenderComponent>();
	ComponentManager::RegisterType<MorphAnimationManager>();
	ComponentManager::RegisterType<Pointer>();
	ComponentManager::RegisterType<LocomotionBehaviour>();
	ComponentManager::RegisterType<ObjectLinking>();
	ComponentManager::RegisterType<SimpleScreenBehaviour>();
	ComponentManager::RegisterType<WarpBehaviour>();




	// GL states, we'll enable depth testing and backface fulling
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

	bool loadScene = false;
	// For now we can use a toggle to generate our scene vs load from file
	if (loadScene) {
		ResourceManager::LoadManifest("manifest.json");
		scene = Scene::Load("scene.json");

		// Call scene awake to start up all of our components
		scene->Window = window;
		scene->Awake();
	} 
	else {
		// This shader will handle reflective materials
		Shader::Sptr reflectiveShader = ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shader.glsl" },  
			{ ShaderPartType::Fragment, "shaders/frag_environment_reflective.glsl" }  
		}); 

		// This shader handles our basic materials without reflections (cause they expensive)
		Shader::Sptr basicShader = ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/newshader.glsl" },
			{ ShaderPartType::Fragment, "shaders/frag_blinn_phong_textured.glsl" }
		});

		//This shader handles morph animations (use at own risk!)
		Shader::Sptr morphShader = ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string>{
			{ShaderPartType::Vertex, "shaders/morph_vert.glsl"},
			{ ShaderPartType::Fragment, "shaders/frag_blinn_phong_textured.glsl" }
		});

		//Meshes
		MeshResource::Sptr theHandMesh = ResourceManager::CreateAsset<MeshResource>("meshes/hand.obj");
		MeshResource::Sptr bedroomMesh = ResourceManager::CreateAsset<MeshResource>("meshes/megaBedroom.obj");
		MeshResource::Sptr bbPosterMesh = ResourceManager::CreateAsset<MeshResource>("meshes/boyband_poster.obj");
		MeshResource::Sptr bbPosterMesh2 = ResourceManager::CreateAsset<MeshResource>("meshes/boyband_poster2.obj");
		MeshResource::Sptr paintcanMesh = ResourceManager::CreateAsset<MeshResource>("meshes/paintCan.obj");
		MeshResource::Sptr shroombaFrame0 = ResourceManager::CreateAsset<MeshResource>("animated meshes/shroomba/shroombaMesh-0.obj");
		MeshResource::Sptr shroombaFrame1 = ResourceManager::CreateAsset<MeshResource>("animated meshes/shroomba/shroombaMesh-1.obj");
		MeshResource::Sptr shroombaFrame2 = ResourceManager::CreateAsset<MeshResource>("animated meshes/shroomba/shroombaMesh-2.obj");
		MeshResource::Sptr shroombaFrame3 = ResourceManager::CreateAsset<MeshResource>("animated meshes/shroomba/shroombaMesh-3.obj");
		MeshResource::Sptr radioFrame0 = ResourceManager::CreateAsset<MeshResource>("animated meshes/radio/radioMesh-0.obj");
		MeshResource::Sptr radioFrame1 = ResourceManager::CreateAsset<MeshResource>("animated meshes/radio/radioMesh-1.obj");
		MeshResource::Sptr radioFrame2 = ResourceManager::CreateAsset<MeshResource>("animated meshes/radio/radioMesh-2.obj");
		MeshResource::Sptr radioFrame3 = ResourceManager::CreateAsset<MeshResource>("animated meshes/radio/radioMesh-3.obj");
		MeshResource::Sptr radioFrame4 = ResourceManager::CreateAsset<MeshResource>("animated meshes/radio/radioMesh-4.obj");
		MeshResource::Sptr radioFrame5 = ResourceManager::CreateAsset<MeshResource>("animated meshes/radio/radioMesh-5.obj");
		MeshResource::Sptr radioFrame6 = ResourceManager::CreateAsset<MeshResource>("animated meshes/radio/radioMesh-6.obj");
		MeshResource::Sptr homeworkFrame0 = ResourceManager::CreateAsset<MeshResource>("animated meshes/homework/homeworkMesh-0.obj");
		MeshResource::Sptr homeworkFrame1 = ResourceManager::CreateAsset<MeshResource>("animated meshes/homework/homeworkMesh-1.obj");
		MeshResource::Sptr homeworkFrame2 = ResourceManager::CreateAsset<MeshResource>("animated meshes/homework/homeworkMesh-2.obj");
		MeshResource::Sptr homeworkMessFrame1 = ResourceManager::CreateAsset<MeshResource>("animated meshes/homework/messyHomework.obj");
		MeshResource::Sptr handIdle0 = ResourceManager::CreateAsset<MeshResource>("animated meshes/hand/handIdleMesh-0.obj");
		MeshResource::Sptr handIdle3 = ResourceManager::CreateAsset<MeshResource>("animated meshes/hand/handIdleMesh-3.obj");
		MeshResource::Sptr handIdle4 = ResourceManager::CreateAsset<MeshResource>("animated meshes/hand/handIdleMesh-4.obj");
		MeshResource::Sptr handIdle5 = ResourceManager::CreateAsset<MeshResource>("animated meshes/hand/handIdleMesh-5.obj");

		MeshResource::Sptr roomTwoMesh = ResourceManager::CreateAsset<MeshResource>("meshes/MasterBedroomMegaModel.obj");
		MeshResource::Sptr VanityMesh = ResourceManager::CreateAsset<MeshResource>("meshes/Vanity.obj");
		MeshResource::Sptr MasterBedMesh = ResourceManager::CreateAsset<MeshResource>("meshes/MasterBed.obj");
		MeshResource::Sptr JeweleryBoxMesh = ResourceManager::CreateAsset<MeshResource>("meshes/Jewelery_Box.obj");
		MeshResource::Sptr FlowerMesh = ResourceManager::CreateAsset<MeshResource>("animated meshes/Flower/FlowerMesh0.obj");

		MeshResource::Sptr megaBathroomMesh = ResourceManager::CreateAsset<MeshResource>("meshes/BathroomMeshes/megaBathroom.obj");
		MeshResource::Sptr soapMesh = ResourceManager::CreateAsset<MeshResource>("meshes/BathroomMeshes/soap.obj");
		MeshResource::Sptr duckyMesh = ResourceManager::CreateAsset<MeshResource>("meshes/BathroomMeshes/ducky.obj");
		MeshResource::Sptr flatDuckyMesh = ResourceManager::CreateAsset<MeshResource>("meshes/BathroomMeshes/flatDucky.obj");
		MeshResource::Sptr toiletMesh = ResourceManager::CreateAsset<MeshResource>("meshes/BathroomMeshes/toilet.obj");

		MeshResource::Sptr megaKitchenMesh = ResourceManager::CreateAsset<MeshResource>("meshes/KitchenMeshes/megaKitchen.obj");
		MeshResource::Sptr trashMesh = ResourceManager::CreateAsset<MeshResource>("meshes/KitchenMeshes/Trash.obj");
		MeshResource::Sptr cakeMesh = ResourceManager::CreateAsset<MeshResource>("meshes/KitchenMeshes/cake.obj");
		MeshResource::Sptr mouseTrapMesh = ResourceManager::CreateAsset<MeshResource>("meshes/KitchenMeshes/mouseTrap.obj");
		MeshResource::Sptr bowlOfFruitMesh = ResourceManager::CreateAsset<MeshResource>("meshes/KitchenMeshes/bowlOfFruit.obj");

		MeshResource::Sptr megaLivingroomMesh = ResourceManager::CreateAsset<MeshResource>("meshes/LivingroomMeshes/LivingRoomMega.obj");
		MeshResource::Sptr glassTableMesh = ResourceManager::CreateAsset<MeshResource>("meshes/LivingroomMeshes/glassTable.obj");
		MeshResource::Sptr brokenTableMesh = ResourceManager::CreateAsset<MeshResource>("meshes/LivingroomMeshes/glassTableBroken.obj");
		MeshResource::Sptr picturesMesh = ResourceManager::CreateAsset<MeshResource>("meshes/LivingroomMeshes/pictures.obj");
		MeshResource::Sptr plantFrame0 = ResourceManager::CreateAsset<MeshResource>("animated meshes/Plant/plantMesh0.obj");
		MeshResource::Sptr plantFrame1 = ResourceManager::CreateAsset<MeshResource>("animated meshes/Plant/plantMesh1.obj");
		MeshResource::Sptr plantFrame2 = ResourceManager::CreateAsset<MeshResource>("animated meshes/Plant/plantMesh2.obj");
		MeshResource::Sptr plantFrame3 = ResourceManager::CreateAsset<MeshResource>("animated meshes/Plant/plantMesh3.obj");
		MeshResource::Sptr plantFrame4 = ResourceManager::CreateAsset<MeshResource>("animated meshes/Plant/plantMesh4.obj");
		MeshResource::Sptr monsterPlantFrame0 = ResourceManager::CreateAsset<MeshResource>("animated meshes/PlantMonster/plantMonsterMesh0.obj");
		MeshResource::Sptr monsterPlantFrame1 = ResourceManager::CreateAsset<MeshResource>("animated meshes/PlantMonster/plantMonsterMesh1.obj");
		MeshResource::Sptr monsterPlantFrame2 = ResourceManager::CreateAsset<MeshResource>("animated meshes/PlantMonster/plantMonsterMesh2.obj");
		MeshResource::Sptr remoteMesh = ResourceManager::CreateAsset<MeshResource>("meshes/LivingroomMeshes/remote.obj");
		MeshResource::Sptr TVMesh = ResourceManager::CreateAsset<MeshResource>("meshes/LivingroomMeshes/TV.obj");
		MeshResource::Sptr monsterPlantMesh = ResourceManager::CreateAsset<MeshResource>("meshes/LivingroomMeshes/plantMonster.obj");
		
		MeshResource::Sptr megaBasementMesh = ResourceManager::CreateAsset<MeshResource>("meshes/BasementMeshes/megaBasement.obj");
		MeshResource::Sptr teslaCoilMesh = ResourceManager::CreateAsset<MeshResource>("meshes/BasementMeshes/Teslacoil.obj");
		MeshResource::Sptr frankenstineMesh = ResourceManager::CreateAsset<MeshResource>("meshes/BasementMeshes/FrankenstineFixed.obj");
		MeshResource::Sptr frankensteinLessMesh = ResourceManager::CreateAsset<MeshResource>("meshes/BasementMeshes/FrankenstineGone.obj");
		MeshResource::Sptr flasksMesh = ResourceManager::CreateAsset<MeshResource>("meshes/BasementMeshes/flasks.obj");
		MeshResource::Sptr boiledOverFlaskMesh = ResourceManager::CreateAsset<MeshResource>("meshes/BasementMeshes/boiledOverFlasks.obj");
		MeshResource::Sptr chainsawMesh = ResourceManager::CreateAsset<MeshResource>("meshes/BasementMeshes/chainsaw.obj");
		MeshResource::Sptr boilerMesh = ResourceManager::CreateAsset<MeshResource>("meshes/BasementMeshes/boiler.obj");
		MeshResource::Sptr boilerFrame0 = ResourceManager::CreateAsset<MeshResource>("animated meshes/Boiler/boilerMesh0.obj");
		MeshResource::Sptr boilerFrame1 = ResourceManager::CreateAsset<MeshResource>("animated meshes/Boiler/boilerMesh1.obj");
		MeshResource::Sptr boilerFrame2 = ResourceManager::CreateAsset<MeshResource>("animated meshes/Boiler/boilerMesh2.obj");
		MeshResource::Sptr boilerFrame3 = ResourceManager::CreateAsset<MeshResource>("animated meshes/Boiler/boilerMesh3.obj");
		MeshResource::Sptr boilerFrame4 = ResourceManager::CreateAsset<MeshResource>("animated meshes/Boiler/boilerMesh4.obj");
		

		//Textures
		Texture2D::Sptr    handDefault = ResourceManager::CreateAsset<Texture2D>("textures/Hand.png");
		Texture2D::Sptr    handMusic = ResourceManager::CreateAsset<Texture2D>("textures/HandMusic.png");
		Texture2D::Sptr    handHomework = ResourceManager::CreateAsset<Texture2D>("textures/HandHomework.png");
		Texture2D::Sptr    handShroom = ResourceManager::CreateAsset<Texture2D>("textures/HandShroom1.png");
		Texture2D::Sptr    handBoyBand = ResourceManager::CreateAsset<Texture2D>("textures/HandBoyBand.png");
		Texture2D::Sptr    handRainbow = ResourceManager::CreateAsset<Texture2D>("textures/HandRainbow.png");
		Texture2D::Sptr    handAngry = ResourceManager::CreateAsset<Texture2D>("textures/HandAngy.png");
		Texture2D::Sptr    handBling = ResourceManager::CreateAsset<Texture2D>("textures/HandBling.png");
		Texture2D::Sptr    handBurned = ResourceManager::CreateAsset<Texture2D>("textures/HandBurned.png");
		Texture2D::Sptr    handCake = ResourceManager::CreateAsset<Texture2D>("textures/HandCake.png");
		Texture2D::Sptr    handClean = ResourceManager::CreateAsset<Texture2D>("textures/HandClean.png");
		Texture2D::Sptr    handClown = ResourceManager::CreateAsset<Texture2D>("textures/HandClown.png");
		Texture2D::Sptr    handCracked = ResourceManager::CreateAsset<Texture2D>("textures/HandCracked.png");
		Texture2D::Sptr    handDucky = ResourceManager::CreateAsset<Texture2D>("textures/HandDucky.png");
		Texture2D::Sptr    handElectric = ResourceManager::CreateAsset<Texture2D>("textures/HandElecrified.png");
		Texture2D::Sptr    handFrank = ResourceManager::CreateAsset<Texture2D>("textures/HandFrankenstine.png");
		Texture2D::Sptr    handFruit = ResourceManager::CreateAsset<Texture2D>("textures/HandFruit.png");
		Texture2D::Sptr    handGreen = ResourceManager::CreateAsset<Texture2D>("textures/HandGreenThumb.png");
		Texture2D::Sptr    handKiller = ResourceManager::CreateAsset<Texture2D>("textures/HandKiller.png");
		Texture2D::Sptr    handMouse = ResourceManager::CreateAsset<Texture2D>("textures/HandMouse.png");
		Texture2D::Sptr    handRobe = ResourceManager::CreateAsset<Texture2D>("textures/HandRobe.png");
		Texture2D::Sptr    handScience = ResourceManager::CreateAsset<Texture2D>("textures/HandScientist.png");
		Texture2D::Sptr    handPoo = ResourceManager::CreateAsset<Texture2D>("textures/HandPoo.png");
		Texture2D::Sptr    handTech = ResourceManager::CreateAsset<Texture2D>("textures/HandTech.png");
		Texture2D::Sptr    handTrash = ResourceManager::CreateAsset<Texture2D>("textures/HandTrash.png");
		Texture2D::Sptr    handZombie = ResourceManager::CreateAsset<Texture2D>("textures/HandZombie.png");
		Texture2D::Sptr    paintedOverPoster = ResourceManager::CreateAsset<Texture2D>("textures/megaTextMessy.png");

		Texture2D::Sptr    missingTex = ResourceManager::CreateAsset<Texture2D>("textures/MissingTexture.png");
		Texture2D::Sptr    rightWallTex = ResourceManager::CreateAsset<Texture2D>("textures/RightWall.png");
		Texture2D::Sptr    rewardSkin = ResourceManager::CreateAsset<Texture2D>("textures/forggyBonds.png");
		Texture2D::Sptr    shroomTex = ResourceManager::CreateAsset<Texture2D>("textures/Shroomba.png");
		Texture2D::Sptr    bedroomTex = ResourceManager::CreateAsset<Texture2D>("textures/MegaText.png");
		Texture2D::Sptr    radioTex = ResourceManager::CreateAsset<Texture2D>("textures/Radio.png");
		Texture2D::Sptr    radioTex2 = ResourceManager::CreateAsset<Texture2D>("textures/change_radio.png");
		Texture2D::Sptr    homeworkTex = ResourceManager::CreateAsset<Texture2D>("textures/Homework.png");
		Texture2D::Sptr    bbPosterTex = ResourceManager::CreateAsset<Texture2D>("textures/Poster.png");
		Texture2D::Sptr    paintcanTex = ResourceManager::CreateAsset<Texture2D>("textures/PaintCan.png");
		Texture2D::Sptr    menuTex = ResourceManager::CreateAsset<Texture2D>("textures/Menu.png");
		Texture2D::Sptr    menuPointerTex = ResourceManager::CreateAsset<Texture2D>("textures/MenuPointer.png");
		Texture2D::Sptr    ListTex = ResourceManager::CreateAsset<Texture2D>("textures/List.png");
		Texture2D::Sptr    pauseTex = ResourceManager::CreateAsset<Texture2D>("textures/Pause.png");
		Texture2D::Sptr    lineTex = ResourceManager::CreateAsset<Texture2D>("textures/Line.png");
		Texture2D::Sptr    secretTex = ResourceManager::CreateAsset<Texture2D>("textures/SecretTextOne.png");

		Texture2D::Sptr    booksInteractTex = ResourceManager::CreateAsset<Texture2D>("textures/BooksInteract.png");
		Texture2D::Sptr    radioInteractTex = ResourceManager::CreateAsset<Texture2D>("textures/RadioInteract.png");
		Texture2D::Sptr    posterInteractTex = ResourceManager::CreateAsset<Texture2D>("textures/PosterInteract.png");
		Texture2D::Sptr    shroombaInteractTex = ResourceManager::CreateAsset<Texture2D>("textures/ShroombaInteract.png");
		Texture2D::Sptr    paintInteractTex = ResourceManager::CreateAsset<Texture2D>("textures/PaintInteract.png");
		Texture2D::Sptr    bedInteractTex = ResourceManager::CreateAsset<Texture2D>("textures/Bed.png");
		Texture2D::Sptr    alchemyInteractTex = ResourceManager::CreateAsset<Texture2D>("textures/Alchemy Lab.png");
		Texture2D::Sptr    boilerInteractTex = ResourceManager::CreateAsset<Texture2D>("textures/Boiler.png");
		Texture2D::Sptr    cakeInteractTex = ResourceManager::CreateAsset<Texture2D>("textures/Cake.png");
		Texture2D::Sptr    chainsawInteractTex = ResourceManager::CreateAsset<Texture2D>("textures/ChainSaw.png");
		Texture2D::Sptr    duckyInteractTex = ResourceManager::CreateAsset<Texture2D>("textures/Ducky.png");
		Texture2D::Sptr    flowerInteractTex = ResourceManager::CreateAsset<Texture2D>("textures/Flower.png");
		Texture2D::Sptr    frankInteractTex = ResourceManager::CreateAsset<Texture2D>("textures/Frankenstine.png");
		Texture2D::Sptr    fruitInteractTex = ResourceManager::CreateAsset<Texture2D>("textures/Fruit.png");
		Texture2D::Sptr    jewleryInteractTex = ResourceManager::CreateAsset<Texture2D>("textures/Jewelry Box.png");
		Texture2D::Sptr    mouseInteractTex = ResourceManager::CreateAsset<Texture2D>("textures/MouseTrap.png");
		Texture2D::Sptr    picturesInteractTex = ResourceManager::CreateAsset<Texture2D>("textures/Pictures.png");
		Texture2D::Sptr    plantInteractTex = ResourceManager::CreateAsset<Texture2D>("textures/Plant.png");
		Texture2D::Sptr    remoteInteractTex = ResourceManager::CreateAsset<Texture2D>("textures/Remote.png");
		Texture2D::Sptr    toiletInteractTex = ResourceManager::CreateAsset<Texture2D>("textures/Toilet.png");
		Texture2D::Sptr    soapInteractTex = ResourceManager::CreateAsset<Texture2D>("textures/Soap.png");
		Texture2D::Sptr    tableInteractTex = ResourceManager::CreateAsset<Texture2D>("textures/Table.png");
		Texture2D::Sptr    teslaInteractTex = ResourceManager::CreateAsset<Texture2D>("textures/TeslaCoil.png");
		Texture2D::Sptr    trashInteractTex = ResourceManager::CreateAsset<Texture2D>("textures/Trash.png");
		Texture2D::Sptr    vanityInteractTex = ResourceManager::CreateAsset<Texture2D>("textures/Vanity.png");


		Texture2D::Sptr    winTex = ResourceManager::CreateAsset<Texture2D>("textures/win.png");
		Texture2D::Sptr    ePrTex = ResourceManager::CreateAsset<Texture2D>("textures/EnterPrompt.png");
		Texture2D::Sptr    eTex = ResourceManager::CreateAsset<Texture2D>("textures/E.png");

		Texture2D::Sptr    bedroom_mListTex = ResourceManager::CreateAsset<Texture2D>("textures/ListMasterBedroom.png");
		Texture2D::Sptr    FlowerTex = ResourceManager::CreateAsset<Texture2D>("textures/FlowerBase.png");
		Texture2D::Sptr    JeweleryBoxTex = ResourceManager::CreateAsset<Texture2D>("textures/Jewelery_Box.png");
		Texture2D::Sptr    MasterBedTex = ResourceManager::CreateAsset<Texture2D>("textures/Masterbed.png");
		Texture2D::Sptr    VanityTex = ResourceManager::CreateAsset<Texture2D>("textures/VanityBase.png");
		Texture2D::Sptr    MasterBedroomTex = ResourceManager::CreateAsset<Texture2D>("textures/MasterBedroom.png");

		Texture2D::Sptr bathroomListTex = ResourceManager::CreateAsset<Texture2D>("textures/ListBathroom.png");
		Texture2D::Sptr megaBathroomTex = ResourceManager::CreateAsset<Texture2D>("textures/BathroomTextures/BathroomTexture.png");
		Texture2D::Sptr soapTex = ResourceManager::CreateAsset<Texture2D>("textures/BathroomTextures/soap.png");
		Texture2D::Sptr duckyTex = ResourceManager::CreateAsset<Texture2D>("textures/BathroomTextures/Ducky.png");
		Texture2D::Sptr toiletTex = ResourceManager::CreateAsset<Texture2D>("textures/BathroomTextures/toilet.png");

		Texture2D::Sptr kitchenListTex = ResourceManager::CreateAsset<Texture2D>("textures/ListKitchen.png");
		Texture2D::Sptr megaKitchenTex = ResourceManager::CreateAsset<Texture2D>("textures/KitchenTextures/KitchenTexture.png");
		Texture2D::Sptr trashTex = ResourceManager::CreateAsset<Texture2D>("textures/KitchenTextures/Trash.png");
		Texture2D::Sptr cakeTex = ResourceManager::CreateAsset<Texture2D>("textures/KitchenTextures/Cake.png");
		Texture2D::Sptr mouseTrapTex = ResourceManager::CreateAsset<Texture2D>("textures/KitchenTextures/MouseTrap.png");
		Texture2D::Sptr bowlOfFruitTex = ResourceManager::CreateAsset<Texture2D>("textures/KitchenTextures/FruitBowl.png");

		Texture2D::Sptr livingroomListTex = ResourceManager::CreateAsset<Texture2D>("textures/ListLivingroom.png");
		Texture2D::Sptr megaLivingroomTex = ResourceManager::CreateAsset<Texture2D>("textures/LivingroomTextures/MegaModelTexture.png");
		Texture2D::Sptr glassTableTex = ResourceManager::CreateAsset<Texture2D>("textures/LivingroomTextures/glassTable.png");
		Texture2D::Sptr picturesTex = ResourceManager::CreateAsset<Texture2D>("textures/LivingroomTextures/pictures.png");
		Texture2D::Sptr clownPicturesTex = ResourceManager::CreateAsset<Texture2D>("textures/LivingroomTextures/clownpictures.png");
		Texture2D::Sptr plantTex = ResourceManager::CreateAsset<Texture2D>("textures/LivingroomTextures/Plant.png");
		Texture2D::Sptr remoteTex = ResourceManager::CreateAsset<Texture2D>("textures/LivingroomTextures/remote.png");
		Texture2D::Sptr TVTex = ResourceManager::CreateAsset<Texture2D>("textures/LivingroomTextures/TV.png");
		Texture2D::Sptr BrokenTVTex = ResourceManager::CreateAsset<Texture2D>("textures/LivingroomTextures/BrokenTV.png");
		Texture2D::Sptr monsterPlantTex = ResourceManager::CreateAsset<Texture2D>("textures/LivingroomTextures/plantMonster.png");

		Texture2D::Sptr basementListTex = ResourceManager::CreateAsset<Texture2D>("textures/ListBasement.png");
		Texture2D::Sptr megaBasementTex = ResourceManager::CreateAsset<Texture2D>("textures/BasementTextures/BasementTexture.png");
		Texture2D::Sptr teslaCoilTex = ResourceManager::CreateAsset<Texture2D>("textures/BasementTextures/TeslaCoil.png");
		Texture2D::Sptr teslaCoilOnTex = ResourceManager::CreateAsset<Texture2D>("textures/BasementTextures/TeslaCoilOn.png");
		Texture2D::Sptr frankenstineTex = ResourceManager::CreateAsset<Texture2D>("textures/BasementTextures/Frankensine.png");
		Texture2D::Sptr flasksTex = ResourceManager::CreateAsset<Texture2D>("textures/BasementTextures/Flasks.png");
		Texture2D::Sptr boiledOverFlasksTex = ResourceManager::CreateAsset<Texture2D>("textures/BasementTextures/ExplosionFlasks.png");
		Texture2D::Sptr chainsawTex = ResourceManager::CreateAsset<Texture2D>("textures/BasementTextures/Chainsaw.png");
		Texture2D::Sptr boilerTex = ResourceManager::CreateAsset<Texture2D>("textures/BasementTextures/boilerFixed.png");

		
		// Here we'll load in the cubemap, as well as a special shader to handle drawing the skybox
		TextureCube::Sptr testCubemap = ResourceManager::CreateAsset<TextureCube>("cubemaps/map/map.jpg");
		Shader::Sptr      skyboxShader = ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/skybox_vert.glsl" },
			{ ShaderPartType::Fragment, "shaders/skybox_frag.glsl" }
		});   
		 
		// Create an empty scene
		scene = std::make_shared<Scene>();
		
		// Setting up our enviroment map
		scene->SetSkyboxTexture(testCubemap);
		scene->SetSkyboxShader(skyboxShader);
		// Since the skybox I used was for Y-up, we need to rotate it 90 deg around the X-axis to convert it to z-up
		scene->SetSkyboxRotation(glm::rotate(MAT4_IDENTITY, glm::half_pi<float>(), glm::vec3(1.0f, 0.0f, 0.0f)));

		//Materials
		Material::Sptr handDefaultMaterial = MakeMaterial("Hand Default", morphShader, handDefault, 0.1);
		Material::Sptr handMusicMaterial = MakeMaterial("Hand Music", morphShader, handMusic, 0.1);
		Material::Sptr handHomeworkMaterial = MakeMaterial("Hand Homework", morphShader, handHomework, 0.1);
		Material::Sptr handBoyBandMaterial = MakeMaterial("Hand BoyBand", morphShader, handBoyBand, 0.1);
		Material::Sptr handShroomMaterial = MakeMaterial("Hand Shroom", morphShader, handShroom, 0.1);
		Material::Sptr handRainbowMaterial = MakeMaterial("Hand Rainbow", morphShader, handRainbow, 0.1);
		Material::Sptr handAngryMaterial = MakeMaterial("Hand Angry", morphShader, handAngry, 0.1);
		Material::Sptr handBlingMaterial = MakeMaterial("Hand Bling", morphShader, handBling, 0.1);
		Material::Sptr handBurnedMaterial = MakeMaterial("Hand Burned", morphShader, handBurned, 0.1);
		Material::Sptr handCakeMaterial = MakeMaterial("Hand Cake", morphShader, handCake, 0.1);
		Material::Sptr handCleanMaterial = MakeMaterial("Hand Clean", morphShader, handClean, 0.1);
		Material::Sptr handClownMaterial = MakeMaterial("Hand Clown", morphShader, handClown, 0.1);
		Material::Sptr handCrackedMaterial = MakeMaterial("Hand Cracked", morphShader, handCracked, 0.1);
		Material::Sptr handDuckyMaterial = MakeMaterial("Hand Ducky", morphShader, handDucky, 0.1);
		Material::Sptr handElectricMaterial = MakeMaterial("Hand Electric", morphShader, handElectric, 0.1);
		Material::Sptr handFrankMaterial = MakeMaterial("Hand Frank", morphShader, handFrank, 0.1);
		Material::Sptr handFruitMaterial = MakeMaterial("Hand Fruit", morphShader, handFruit, 0.1);
		Material::Sptr handGreenMaterial = MakeMaterial("Hand Green", morphShader, handGreen, 0.1);
		Material::Sptr handKillerMaterial = MakeMaterial("Hand Killer", morphShader, handKiller, 0.1);
		Material::Sptr handMouseMaterial = MakeMaterial("Hand Mouse", morphShader, handMouse, 0.1);
		Material::Sptr handPooMaterial = MakeMaterial("Hand Poo", morphShader, handPoo, 0.1);
		Material::Sptr handRobeMaterial = MakeMaterial("Hand Robe", morphShader, handRobe, 0.1);
		Material::Sptr handScienceMaterial = MakeMaterial("Hand Science", morphShader, handScience, 0.1);
		Material::Sptr handTechMaterial = MakeMaterial("Hand Tech", morphShader, handTech, 0.1);
		Material::Sptr handTrashMaterial = MakeMaterial("Hand Trash", morphShader, handTrash, 0.1);
		Material::Sptr handZombieMaterial = MakeMaterial("Hand Zombie", morphShader, handZombie, 0.1);

		Material::Sptr missingMaterial = MakeMaterial("Missing Texture", basicShader, missingTex, 0.1f);
		Material::Sptr rewardMaterial = MakeMaterial("Reward Material", reflectiveShader, rewardSkin, 0.5);
		Material::Sptr rightWallMaterial = MakeMaterial("Right Wall", basicShader, rightWallTex, 0.1f);
		Material::Sptr shroombaMaterial = MakeMaterial("Shroomba Material", morphShader, shroomTex, 0.1f);
		Material::Sptr bedroomMaterial = MakeMaterial("Bedroom Material", basicShader, bedroomTex, 0.2f);
		Material::Sptr paintedOverMaterial = MakeMaterial("Spooky Material", basicShader, paintedOverPoster, 0.2);
		Material::Sptr radioMaterial = MakeMaterial("Radio Material", morphShader, radioTex, 0.1f);
		Material::Sptr radioMaterial2 = MakeMaterial("Radio Material 2", morphShader, radioTex2, 0.1f);
		Material::Sptr homeworkMaterial = MakeMaterial("Homework Material", morphShader, homeworkTex, 0.1f);
		Material::Sptr bbPosterMaterial = MakeMaterial("Boyband Poster Material", basicShader, bbPosterTex, 0.1f);
		Material::Sptr bbPosterMaterial2 = MakeMaterial("Boyband Poster Material 2", basicShader, bbPosterTex, 0.1f);
		Material::Sptr paintcanMaterial = MakeMaterial("Paint Can Material", basicShader, paintcanTex, 0.1f);
		Material::Sptr menuMaterial = MakeMaterial("Menu Material", basicShader, menuTex, 0.1f);
		Material::Sptr menuPointerMaterial = MakeMaterial("Menu Pointer Material", basicShader, menuPointerTex, 0.1f);
		Material::Sptr pauseMaterial = MakeMaterial("Pause Material", basicShader, pauseTex, 0.1f);
		Material::Sptr lineMaterial = MakeMaterial("Line Material", basicShader, lineTex, 0.1f);
		Material::Sptr secretMaterial = MakeMaterial("Secret Material", basicShader, secretTex, 0.1f);


		Material::Sptr ListMaterial = MakeMaterial("Bedroom List Material", basicShader, ListTex, 0.1f);
		Material::Sptr bedroom_mListMaterial = MakeMaterial("Master B.Room List Material", basicShader, bedroom_mListTex, 0.1f);
		Material::Sptr bathroomListMaterial = MakeMaterial("Bathroom List Material", basicShader, bathroomListTex, 0.1f);
		Material::Sptr kitchenListMaterial = MakeMaterial("Kitchen List Material", basicShader, kitchenListTex, 0.1f);
		Material::Sptr livingroomListMaterial = MakeMaterial("Living Room List Material", basicShader, livingroomListTex, 0.1f);
		Material::Sptr basementListMaterial = MakeMaterial("Basement List Material", basicShader, basementListTex, 0.1f);
		std::vector<Material::Sptr>listMats{ ListMaterial, bedroom_mListMaterial, bathroomListMaterial, kitchenListMaterial, livingroomListMaterial, basementListMaterial };

		Material::Sptr radioInteractMaterial = MakeMaterial("Radio Interact Material", basicShader, radioInteractTex, 0.1f);
		Material::Sptr posterInteractMaterial = MakeMaterial("Poster Interact Material", basicShader, posterInteractTex, 0.1f);
		Material::Sptr paintInteractMaterial = MakeMaterial("Paint Interact Material", basicShader, paintInteractTex, 0.1f);
		Material::Sptr booksInteractMaterial = MakeMaterial("Books Interact Material", basicShader, booksInteractTex, 0.1f);
		Material::Sptr shroombaInteractMaterial = MakeMaterial("Shroomba Interact Material", basicShader, shroombaInteractTex, 0.1f);
		Material::Sptr alchemyInteractMaterial = MakeMaterial("Alchemy Interact Material", basicShader, alchemyInteractTex, 0.1f);
		Material::Sptr bedInteractMaterial = MakeMaterial("Bed Interact Material", basicShader, bedInteractTex, 0.1f);
		Material::Sptr boilerInteractMaterial = MakeMaterial("Boiler Interact Material", basicShader, boilerInteractTex, 0.1f);
		Material::Sptr cakeInteractMaterial = MakeMaterial("Cake Interact Material", basicShader, cakeInteractTex, 0.1f);
		Material::Sptr chainsawInteractMaterial = MakeMaterial("Chainsaw Interact Material", basicShader, chainsawInteractTex, 0.1f);
		Material::Sptr duckyInteractMaterial = MakeMaterial("Ducky Interact Material", basicShader, duckyInteractTex, 0.1f);
		Material::Sptr flowerInteractMaterial = MakeMaterial("Flower Interact Material", basicShader, flowerInteractTex, 0.1f);
		Material::Sptr frankInteractMaterial = MakeMaterial("Frankenstine Interact Material", basicShader, frankInteractTex, 0.1f);
		Material::Sptr fruitInteractMaterial = MakeMaterial("Fruit Interact Material", basicShader, fruitInteractTex, 0.1f);
		Material::Sptr jewleryInteractMaterial = MakeMaterial("Jewlery Interact Material", basicShader, jewleryInteractTex, 0.1f);
		Material::Sptr mouseInteractMaterial = MakeMaterial("Mousetrap Interact Material", basicShader, mouseInteractTex, 0.1f);
		Material::Sptr picturesInteractMaterial = MakeMaterial("Pictures Interact Material", basicShader, picturesInteractTex, 0.1f);
		Material::Sptr plantInteractMaterial = MakeMaterial("Plant Interact Material", basicShader, plantInteractTex, 0.1f);
		Material::Sptr remoteInteractMaterial = MakeMaterial("Remote Interact Material", basicShader, remoteInteractTex, 0.1f);
		Material::Sptr toiletInteractMaterial = MakeMaterial("Toilet Interact Material", basicShader, toiletInteractTex, 0.1f);
		Material::Sptr soapInteractMaterial = MakeMaterial("Soap Interact Material", basicShader, soapInteractTex, 0.1f);
		Material::Sptr tableInteractMaterial = MakeMaterial("Table Interact Material", basicShader, tableInteractTex, 0.1f);
		Material::Sptr teslaInteractMaterial = MakeMaterial("Tesla Coil Interact Material", basicShader, teslaInteractTex, 0.1f);
		Material::Sptr trashInteractMaterial = MakeMaterial("Trash Interact Material", basicShader, trashInteractTex, 0.1f);
		Material::Sptr vanityInteractMaterial = MakeMaterial("Vanity Interact Material", basicShader, vanityInteractTex, 0.1f);
		

		Material::Sptr winMaterial = MakeMaterial("Win Material", basicShader, winTex, 0.1f);
		Material::Sptr ePromptMaterial = MakeMaterial("Enter Prompt Material", basicShader, ePrTex, 0.1f);
		Material::Sptr eMaterial = MakeMaterial("Enter Prompt Material", basicShader, eTex, 0.1f);

		Material::Sptr flowerMaterial = MakeMaterial("Enter Prompt Material", basicShader, FlowerTex, 0.1f);
		Material::Sptr jeweleryBoxMaterial = MakeMaterial("Enter Prompt Material", basicShader, JeweleryBoxTex, 0.1f);
		Material::Sptr masterBedMaterial = MakeMaterial("Enter Prompt Material", basicShader, MasterBedTex, 0.1f);
		Material::Sptr vanityMaterial = MakeMaterial("Enter Prompt Material", basicShader, VanityTex, 0.1f);
		Material::Sptr masterBedroomMaterial = MakeMaterial("Enter Prompt Material", basicShader, MasterBedroomTex, 0.1f);

		Material::Sptr megaBathroomMaterial = MakeMaterial("Enter Prompt Material", basicShader, megaBathroomTex, 0.1f);
		Material::Sptr soapMaterial = MakeMaterial("Enter Prompt Material", basicShader, soapTex, 0.1f);
		Material::Sptr duckyMaterial = MakeMaterial("Enter Prompt Material", basicShader, duckyTex, 0.1f);
		Material::Sptr toiletMaterial = MakeMaterial("Enter Prompt Material", basicShader, toiletTex, 0.1f);

		Material::Sptr megaKitchenMaterial = MakeMaterial("Enter Prompt Material", basicShader, megaKitchenTex, 0.1f);
		Material::Sptr trashMaterial = MakeMaterial("Enter Prompt Material", basicShader, trashTex, 0.1f);
		Material::Sptr cakeMaterial = MakeMaterial("Enter Prompt Material", basicShader, cakeTex, 0.1f);
		Material::Sptr mouseTrapMaterial = MakeMaterial("Enter Prompt Material", basicShader, mouseTrapTex, 0.1f);
		Material::Sptr bowlOfFruitMaterial = MakeMaterial("Enter Prompt Material", basicShader, bowlOfFruitTex, 0.1f);

		Material::Sptr megaLivingroomMaterial = MakeMaterial("Mega Livingroom Material", basicShader, megaLivingroomTex, 0.1f);
		Material::Sptr glassTableMaterial = MakeMaterial("Glass Table Material", basicShader, glassTableTex, 0.1f);
		Material::Sptr picturesMaterial = MakeMaterial("Livingroom Pictures Material", basicShader, picturesTex, 0.1f);
		Material::Sptr clownPicsMaterial = MakeMaterial("Clown Pictures Material", basicShader, clownPicturesTex, 0.1f);
		Material::Sptr plantMaterial = MakeMaterial("Plant Material", morphShader, plantTex, 0.1f);
		Material::Sptr remoteMaterial = MakeMaterial("TV Remote Material", basicShader, remoteTex, 0.1f);
		Material::Sptr TVMaterial = MakeMaterial("TV Material", basicShader, TVTex, 0.1f);
		Material::Sptr brokenTVmaterial = MakeMaterial("Broken TV Material", basicShader, BrokenTVTex, 0.1f);
		Material::Sptr monsterPlantMaterial = MakeMaterial("Monster Plant Material", morphShader, monsterPlantTex, 0.1f);

		Material::Sptr megaBasementMaterial = MakeMaterial("Basement Material", basicShader, megaBasementTex, 0.1f);
		Material::Sptr teslaCoilMaterial = MakeMaterial("Tesla Coil Material", basicShader, teslaCoilTex, 0.1f);
		Material::Sptr teslaCoilOnMaterial = MakeMaterial("Tesla Coil On Material", basicShader, teslaCoilOnTex, 0.1f);
		Material::Sptr frankenstineMaterial = MakeMaterial("Frankenstein Material", basicShader, frankenstineTex, 0.1f);
		Material::Sptr flasksMaterial = MakeMaterial("Flasks Material", basicShader, flasksTex, 0.1f);
		Material::Sptr expFlasksMaterial = MakeMaterial("Exploded Flasks Material", basicShader, boiledOverFlasksTex, 0.1f);
		Material::Sptr chainsawMaterial = MakeMaterial("Chainsaw Material", basicShader, chainsawTex, 0.1f);
		Material::Sptr boilerMaterial = MakeMaterial("Boiler Material", morphShader, boilerTex, 0.1f);



		//Audio Setups


		AudioEngine* audioEngine = AudioEngine::instance();

		audioEngine->init();

		audioEngine->loadSound("background", "sounds/bkgbop.wav", true);

		audioEngine->loadSound("books", "sounds/Books_crashing.wav", true);
		audioEngine->loadSound("brush", "sounds/Brush_Strokes.wav", true);
		audioEngine->loadSound("velcro", "sounds/Velcro.wav", true);
		audioEngine->loadSound("bed", "sounds/Bed.wav", true);
		audioEngine->loadSound("boiler", "sounds/Boiler.wav", true);
		audioEngine->loadSound("cake", "sounds/Cake.wav", true);
		audioEngine->loadSound("chainsaw", "sounds/Chainsaw.wav", true);
		audioEngine->loadSound("ducky", "sounds/Ducky.wav", true);
		audioEngine->loadSound("flower", "sounds/Flower.wav", true);
		audioEngine->loadSound("frank", "sounds/Frankenstine.wav", true);
		audioEngine->loadSound("fruit", "sounds/Fruit.wav", true);
		audioEngine->loadSound("jewelry", "sounds/JewelryBox.wav", true);
		audioEngine->loadSound("lab", "sounds/Lab.wav", true);
		audioEngine->loadSound("mouse", "sounds/MouseTrap.wav", true);
		audioEngine->loadSound("pictures", "sounds/Pictures.wav", true);
		audioEngine->loadSound("plant", "sounds/Plant.wav", true);
		audioEngine->loadSound("remote", "sounds/Remote.wav", true);
		audioEngine->loadSound("soap", "sounds/Soap.wav", true);
		audioEngine->loadSound("table", "sounds/Table.wav", true);
		audioEngine->loadSound("tesla", "sounds/Tesla Coils.wav", true);
		audioEngine->loadSound("toilet", "sounds/Toilet.wav", true);
		audioEngine->loadSound("trash", "sounds/Trash.wav", true);
		audioEngine->loadSound("vanity", "sounds/Vanity.wav", true);

		audioEngine->loadSound("Bang", "sounds/Bang.wav", true);
		audioEngine->loadSound("Bing", "sounds/Bing.wav", true);
		audioEngine->loadSound("Bong", "sounds/Bong.wav", true);
		//audioEngine->playSoundByName("test");

		AudioSource sound;
		//sound.setSound("test");
		//sound.setPosition(FMOD_VECTOR{ 0,0,0 });
		//sound.play();
		
		//sound.setLooping(true);
		

		// Create some lights for our scene
		scene->Lights.resize(3);
		scene->Lights[0].Position = glm::vec3(0.0f, 1.0f, 17.23f);
		scene->Lights[0].Color = glm::vec3(1.0f, 1.0f, 1.0f);
		scene->Lights[0].Range = -8.5f;

		scene->Lights[1].Position = glm::vec3(-6.76f, 0.29f, 5.74f);
		scene->Lights[1].Color = glm::vec3(0.10196f, 0.73725f, 0.8f);
		scene->Lights[1].Range = 85.4f;

		scene->Lights[2].Position = glm::vec3(1.73f, 0.14f, 9.95f);
		scene->Lights[2].Color = glm::vec3(1.0f, 0.2f, 0.10196f);
		scene->Lights[2].Range = 90.9f;
		// We'll create a mesh that is a simple plane that we can resize later
		MeshResource::Sptr planeMesh = ResourceManager::CreateAsset<MeshResource>();
		planeMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(1.0f)));
		planeMesh->GenerateMesh();

		// Set up the scene's camera
		GameObject::Sptr camera = scene->CreateGameObject("Main Camera");
		{
			camera->SetPosition(glm::vec3(9.15f, 9.41f, 7.85f));
			camera->SetRotation(glm::vec3(80.351f, 0.0f, 142.0f));
			camera->SetScale(glm::vec3(0.69f, 0.769f, 0.83f));

			Camera::Sptr cam = camera->Add<Camera>();
			// Make sure that the camera is set as the scene's main camera!
			scene->MainCamera = cam;
		}

		GameObject::Sptr bedroomObject = MakeBasic("Bedroom Object", 0.f, 0.0f, 0.0f, 0.f, 0.0f, 0.0f, 1, bedroomMaterial, bedroomMesh);
		{	
			ObjectLinking::Sptr link = bedroomObject->Add<ObjectLinking>();
		}
		
		GameObject::Sptr masterBedroomObject = MakeBasic("Master Bedroom Object", 0.0f, 50.0f, -50.0f, 0.f, 0.0f, -10.0f, 2, masterBedroomMaterial, roomTwoMesh);
		{
			ObjectLinking::Sptr link = masterBedroomObject->Add<ObjectLinking>();
			masterBedroomObject->SetScale(glm::vec3(5.170f, 5.20f, 6.240f));
		}

		GameObject::Sptr megaBathroomObject = MakeBasic("Bathroom Object", 0.0f, 100.0f, -50.0f, 0.f, 0.0f, -36.0f, 3, megaBathroomMaterial, megaBathroomMesh);
		{
			ObjectLinking::Sptr link = megaBathroomObject->Add<ObjectLinking>();
			megaBathroomObject->SetScale(glm::vec3(1.5f, 1.5f, 1.5f));
		}

		GameObject::Sptr megaKitchenObject = MakeBasic("Kitchen Object", 0.0f, 150.0f, -50.0f, 0.0f, 0.0f, -9.0f, 4, megaKitchenMaterial, megaKitchenMesh);
		{
			megaKitchenObject->SetScale(glm::vec3(1.2, 1.2, 1.2));
			ObjectLinking::Sptr link = megaKitchenObject->Add<ObjectLinking>();
		}

		GameObject::Sptr megaLivingroomObject = MakeBasic("Livingroom Object", 0.0f, 200.0f, -50.0f, 00.0f, 0.0f, -10.0f, 5, megaLivingroomMaterial, megaLivingroomMesh);
		{
			megaLivingroomObject->SetScale(glm::vec3(1.3, 1.3, 1.3));
			ObjectLinking::Sptr link = megaLivingroomObject->Add<ObjectLinking>();
		}

		GameObject::Sptr megaBasementObject = MakeBasic("Basement Object", 0.0f, 250.0f, -50.0f, 00.0f, 0.0f, -24.0f, 6, megaBasementMaterial, megaBasementMesh);
		{
			//megaBasementObject->SetScale(glm::vec3(1.3, 1.3, 1.3));
			ObjectLinking::Sptr link = megaBasementObject->Add<ObjectLinking>();
		}

		GameObject::Sptr floorManager = scene->CreateGameObject("Floor Manager");
		{
			WarpBehaviour::Sptr warp = floorManager->Add<WarpBehaviour>();
			warp->roomOne = bedroomObject;
			warp->roomTwo = masterBedroomObject;
			warp->roomThree = megaBathroomObject;
			warp->roomFour = megaKitchenObject;
			warp->roomFive = megaLivingroomObject;
			warp->roomSix = megaBasementObject;
			warp->PushLists(listMats);

			TriggerVolume::Sptr volume = floorManager->Add<TriggerVolume>();
			SphereCollider::Sptr collider = SphereCollider::Create(1.8);
			collider->SetPosition(glm::vec3(3.5f, 3.5f, 0.f));
			volume->AddCollider(collider);
		}
		//global UI objects
		GameObject::Sptr prompt = MakeBasicPlane("Prompt", -1.38f, 8.19f, -11.82f, 80.351f, 0.0f, 142.00f, 4.0f, 1.0f, 0, eMaterial);

		GameObject::Sptr pointer = MakeBasicPlane("Pointer", 4.07f, 7.21f, 9.55f, 80.351f, 0.0f, 142.0f, 2.0f, 1.0f, 0, menuPointerMaterial);

		GameObject::Sptr extraScreen = MakeBasicPlane("Extra Screen", 5.87f, 5.79f, -6.9f, 80.351f, 0.0f, 142.00f, 18.0f, 10.0f, 0, missingMaterial);
		{
			InterpolationBehaviour::Sptr interp = extraScreen->Add<InterpolationBehaviour>();
			interp->AddBehaviourScript("interp_scripts/menu_behaviour.txt");
			interp->ToggleBehaviour("Lowering", false);
			interp->PauseOrResumeCurrentBehaviour();

			SimpleScreenBehaviour::Sptr feedbackScreen = extraScreen->Add<SimpleScreenBehaviour>();
			feedbackScreen->targetObjectives = 5;
			feedbackScreen->WinScreen = winMaterial;
		}

		GameObject::Sptr screen = MakeBasicPlane("Screen", 5.87f, 5.79f, 6.9f, 80.351f, 0.0f, 142.00f, 54.0f, 10.0f, 0, menuMaterial);
		{
			MainMenu::Sptr menu = screen->Add<MainMenu>();
			menu->MenuMaterial = menuMaterial;
			menu->PauseMaterial = pauseMaterial;
			menu->pointer = pointer;
			menu->fbScreen = extraScreen;

			InterpolationBehaviour::Sptr interp = screen->Add<InterpolationBehaviour>();
			interp->AddBehaviourScript("interp_scripts/menu_behaviour.txt");
			interp->ToggleBehaviour("Lowering", false);
			interp->PauseOrResumeCurrentBehaviour();

			ObjectLinking::Sptr oLink = screen->Add<ObjectLinking>();
		}

		GameObject::Sptr enterPrompt = MakeBasicPlane("Enter Prompt", 6.54f, 6.03f, 3.4f, 80.351f, 0.0f, 142.00f, 4.0f, 1.0f, 0, ePromptMaterial);
		{
			enterPrompt->Add<ObjectLinking>(screen);
			screen->Get<ObjectLinking>()->LinkObject(enterPrompt);
		}

		GameObject::Sptr list = MakeBasicPlane("List", 11.21f, -1.01f, 6.68f, 80.351f, 0.0f, 142.00f, 4.0f, 10.0f, 0, ListMaterial);
		{
			list->SetScale(glm::vec3(1.3, 1.3, 1.3));

		}

		GameObject::Sptr secretText = MakeBasicPlane("Secret Text", 10.19f, 1.32f, -9.46f, 80.351f, 0.0f, 142.00f, 3.0f, 1.0f, 1, secretMaterial);
		{
			secretText->SetScale(glm::vec3(1.3, 1.3, 1.3));

		}

		GameObject::Sptr lineOne = MakeBasicPlane("Line One", 10.77, -0.25, 109.25, 80.351f, 0.0f, 142.00f, 3.0f, 1.0f, 0, lineMaterial);
		{
			lineOne->SetScale(glm::vec3(1.3, 1.3, 1.3));

		}

		GameObject::Sptr lineTwo = MakeBasicPlane("Line Two", 11.22, -0.75, 107.65, 80.351f, 0.0f, 142.00f, 3.0f, 1.0f, 0, lineMaterial);
		{
			lineTwo->SetScale(glm::vec3(1.3, 1.3, 1.3));

		}

		GameObject::Sptr lineThree = MakeBasicPlane("Line Three", 11.33, -0.55, 105.47, 80.351f, 0.0f, 142.00f, 3.0f, 1.0f, 0, lineMaterial);
		{
			lineThree->SetScale(glm::vec3(1.3, 1.3, 1.3));

		}

		GameObject::Sptr lineFour = MakeBasicPlane("Line Four", 11.57, -0.3, 103.39, 80.351f, 0.0f, 142.00f, 3.0f, 1.0f, 0, lineMaterial);
		{
			lineFour->SetScale(glm::vec3(1.3, 1.3, 1.3));

		}

		GameObject::Sptr lineFive = MakeBasicPlane("Line Five", 11.84, -0.3, 101.74, 80.351f, 0.0f, 142.00f, 3.0f, 1.0f, 0, lineMaterial);
		{
			lineFive->SetScale(glm::vec3(1.3, 1.3, 1.3));

		}

		std::vector<GameObject::Sptr>lines{ lineOne, lineTwo, lineThree, lineFour, lineFive };
		floorManager->Get<WarpBehaviour>()->PushLines(lines);
		


		//Objects for the Master bed
		GameObject::Sptr flowerObject = MakeBasic("Flower Object", -8.18f, 55.6f, -50.830f, 0.f, 0.0f, -50.0f, 2, flowerMaterial, FlowerMesh);
		{
			TriggerVolume::Sptr volume = flowerObject->Add<TriggerVolume>();
			SphereCollider::Sptr collider = SphereCollider::Create(1.5);
			collider->SetPosition(glm::vec3(0.f));
			volume->AddCollider(collider);

			InteractableObjectBehaviour::Sptr interactions = flowerObject->Add<InteractableObjectBehaviour>();
			interactions->AddRewardMaterial(handZombieMaterial);
			interactions->AddSoundEffect("flower");

			interactions->AddFeedbackBehaviour((InteractionFeedback(flowerInteractMaterial, extraScreen)));
			InteractionTForm screenTF(InteractionTForm::tformt::pos, glm::vec3(5.87f, 5.79f, 6.9f));
			interactions->AddFeedbackBehaviour((InteractionFeedback(std::vector<InteractionTForm>{screenTF}, extraScreen)));
			
			interactions->AddFeedbackBehaviour((InteractionFeedback(0)));

			interactions->prompt = prompt;
			interactions->screen = extraScreen;

			flowerObject->SetScale(glm::vec3(2.0, 2.0, 2.0));
			flowerObject->Add<ObjectLinking>(masterBedroomObject);
			masterBedroomObject->Get<ObjectLinking>()->LinkObject(flowerObject);
		}

		GameObject::Sptr vanityObject = MakeBasic("Vanity Object", 6.32f, 38.92f, -45.75f, 0.f, 0.0f, -81.0f, 2, vanityMaterial, VanityMesh);
		{
			TriggerVolume::Sptr volume = vanityObject->Add<TriggerVolume>();
			SphereCollider::Sptr collider = SphereCollider::Create(1.5);
			collider->SetPosition(glm::vec3(0.f));
			volume->AddCollider(collider);

			InteractableObjectBehaviour::Sptr interactions = vanityObject->Add<InteractableObjectBehaviour>();
			interactions->AddRewardMaterial(handCrackedMaterial);
			interactions->AddSoundEffect("vanity");

			interactions->AddFeedbackBehaviour((InteractionFeedback(vanityInteractMaterial, extraScreen)));
			InteractionTForm screenTF(InteractionTForm::tformt::pos, glm::vec3(5.87f, 5.79f, 6.9f));
			interactions->AddFeedbackBehaviour((InteractionFeedback(std::vector<InteractionTForm>{screenTF}, extraScreen)));
			
			interactions->AddFeedbackBehaviour((InteractionFeedback(3)));
			
			interactions->prompt = prompt;
			interactions->screen = extraScreen;

			vanityObject->SetScale(glm::vec3(1.3, 1.3, 1.5));
			vanityObject->Add<ObjectLinking>(masterBedroomObject);
			masterBedroomObject->Get<ObjectLinking>()->LinkObject(vanityObject);
		}

		GameObject::Sptr masterBedObject = MakeBasic("Master Bed Object", -2.32f, 46.25f, -49.78f, 0.f, 0.0f, 11.0f, 2, masterBedMaterial, MasterBedMesh);
		{
			TriggerVolume::Sptr volume = masterBedObject->Add<TriggerVolume>();
			SphereCollider::Sptr collider = SphereCollider::Create(1.5);
			collider->SetPosition(glm::vec3(0.f));
			volume->AddCollider(collider);

			InteractableObjectBehaviour::Sptr interactions = masterBedObject->Add<InteractableObjectBehaviour>();
			interactions->AddRewardMaterial(handRobeMaterial);
			interactions->AddSoundEffect("bed");

			interactions->AddFeedbackBehaviour((InteractionFeedback(bedInteractMaterial, extraScreen)));
			InteractionTForm screenTF(InteractionTForm::tformt::pos, glm::vec3(5.87f, 5.79f, 6.9f));
			interactions->AddFeedbackBehaviour((InteractionFeedback(std::vector<InteractionTForm>{screenTF}, extraScreen)));
			
			interactions->AddFeedbackBehaviour((InteractionFeedback(1)));
			
			interactions->prompt = prompt;
			interactions->screen = extraScreen;

			
			masterBedObject->Add<ObjectLinking>(masterBedroomObject);
			masterBedroomObject->Get<ObjectLinking>()->LinkObject(masterBedObject);
		}

		GameObject::Sptr jeweleryBoxObject = MakeBasic("Jewelery Box Object", 8.18f, 40.5f, -45.65f, 0.f, 0.0f, 0.0f, 2, jeweleryBoxMaterial, JeweleryBoxMesh);
		{
			TriggerVolume::Sptr volume = jeweleryBoxObject->Add<TriggerVolume>();
			SphereCollider::Sptr collider = SphereCollider::Create(1.5);
			collider->SetPosition(glm::vec3(0.f));
			volume->AddCollider(collider);

			InteractableObjectBehaviour::Sptr interactions = jeweleryBoxObject->Add<InteractableObjectBehaviour>();
			interactions->AddRewardMaterial(handBlingMaterial);
			interactions->AddSoundEffect("jewelry");

			interactions->AddFeedbackBehaviour((InteractionFeedback(jewleryInteractMaterial, extraScreen)));
			InteractionTForm screenTF(InteractionTForm::tformt::pos, glm::vec3(5.87f, 5.79f, 6.9f));
			interactions->AddFeedbackBehaviour((InteractionFeedback(std::vector<InteractionTForm>{screenTF}, extraScreen)));
			
			interactions->AddFeedbackBehaviour((InteractionFeedback(2)));
			
			interactions->prompt = prompt;
			interactions->screen = extraScreen;

			jeweleryBoxObject->Add<ObjectLinking>(masterBedroomObject);
			masterBedroomObject->Get<ObjectLinking>()->LinkObject(jeweleryBoxObject);
		}


		//Objects for Bathroom
		GameObject::Sptr soapObject = MakeBasic("Soap Object", 2.72f, 89.66f, -44.56f, 90.f, 0.0f, -90.0f, 3, soapMaterial, soapMesh);
		{
			TriggerVolume::Sptr volume = soapObject->Add<TriggerVolume>();
			SphereCollider::Sptr collider = SphereCollider::Create(1.5);
			collider->SetPosition(glm::vec3(0.f));
			volume->AddCollider(collider);

			InteractableObjectBehaviour::Sptr interactions = soapObject->Add<InteractableObjectBehaviour>();
			interactions->AddRewardMaterial(handCleanMaterial);
			interactions->AddSoundEffect("soap");

			interactions->AddFeedbackBehaviour((InteractionFeedback(soapInteractMaterial, extraScreen)));
			InteractionTForm screenTF(InteractionTForm::tformt::pos, glm::vec3(5.87f, 5.79f, 6.9f));
			interactions->AddFeedbackBehaviour((InteractionFeedback(std::vector<InteractionTForm>{screenTF}, extraScreen)));

			interactions->AddFeedbackBehaviour((InteractionFeedback(2)));

			interactions->prompt = prompt;
			interactions->screen = extraScreen;

			soapObject->SetScale(glm::vec3(0.5, 0.5, 0.5));
			soapObject->Add<ObjectLinking>(megaBathroomObject);
			megaBathroomObject->Get<ObjectLinking>()->LinkObject(soapObject);
		}

		GameObject::Sptr duckObject = MakeBasic("Duck Object", -7.41f, 102.36f, -47.44f, 90.f, 0.0f, 9.0f, 3, duckyMaterial, duckyMesh);
		{
			TriggerVolume::Sptr volume = duckObject->Add<TriggerVolume>();
			SphereCollider::Sptr collider = SphereCollider::Create(1.5);
			collider->SetPosition(glm::vec3(0.f));
			volume->AddCollider(collider);

			InteractableObjectBehaviour::Sptr interactions = duckObject->Add<InteractableObjectBehaviour>();
			interactions->AddRewardMaterial(handDuckyMaterial);
			interactions->AddSoundEffect("ducky");

			interactions->AddFeedbackBehaviour((InteractionFeedback(duckyInteractMaterial, extraScreen)));
			InteractionTForm screenTF(InteractionTForm::tformt::pos, glm::vec3(5.87f, 5.79f, 6.9f));
			interactions->AddFeedbackBehaviour((InteractionFeedback(std::vector<InteractionTForm>{screenTF}, extraScreen)));
			interactions->AddFeedbackBehaviour((InteractionFeedback(flatDuckyMesh, duckObject)));
			interactions->AddFeedbackBehaviour((InteractionFeedback(1)));

			interactions->prompt = prompt;
			interactions->screen = extraScreen;

			duckObject->SetScale(glm::vec3(0.4, 0.4, 0.4));
			duckObject->Add<ObjectLinking>(megaBathroomObject);
			megaBathroomObject->Get<ObjectLinking>()->LinkObject(duckObject);
		}

		GameObject::Sptr toiletObject = MakeBasic("Toilet Object", -2.08f, 95.23f, -48.86f, 90.f, 0.0f, 49.0f, 3, toiletMaterial, toiletMesh);
		{
			TriggerVolume::Sptr volume = toiletObject->Add<TriggerVolume>();
			SphereCollider::Sptr collider = SphereCollider::Create(1.5);
			collider->SetPosition(glm::vec3(0.f));
			volume->AddCollider(collider);

			InteractableObjectBehaviour::Sptr interactions = toiletObject->Add<InteractableObjectBehaviour>();
			interactions->AddRewardMaterial(handPooMaterial);
			interactions->AddSoundEffect("toilet");

			interactions->AddFeedbackBehaviour((InteractionFeedback(toiletInteractMaterial, extraScreen)));
			InteractionTForm screenTF(InteractionTForm::tformt::pos, glm::vec3(5.87f, 5.79f, 6.9f));
			interactions->AddFeedbackBehaviour((InteractionFeedback(std::vector<InteractionTForm>{screenTF}, extraScreen)));

			interactions->AddFeedbackBehaviour((InteractionFeedback(0)));

			interactions->prompt = prompt;
			interactions->screen = extraScreen;

			toiletObject->SetScale(glm::vec3(1.2, 1.2, 1.2));
			toiletObject->Add<ObjectLinking>(megaBathroomObject);
			megaBathroomObject->Get<ObjectLinking>()->LinkObject(toiletObject);
		}



		//Objects for Kitchen
		GameObject::Sptr trashObject = MakeBasic("Trash Object", -9.15f, 150.51f, -49.76f, 90, 0, 90, 4, trashMaterial, trashMesh);
		{
			TriggerVolume::Sptr volume = trashObject->Add<TriggerVolume>();
			SphereCollider::Sptr collider = SphereCollider::Create(1.5);
			collider->SetPosition(glm::vec3(0.f));
			volume->AddCollider(collider);

			InteractableObjectBehaviour::Sptr interactions = trashObject->Add<InteractableObjectBehaviour>();
			interactions->AddRewardMaterial(handTrashMaterial);
			interactions->AddSoundEffect("trash");

			interactions->AddFeedbackBehaviour((InteractionFeedback(trashInteractMaterial, extraScreen)));
			InteractionTForm screenTF(InteractionTForm::tformt::pos, glm::vec3(5.87f, 5.79f, 6.9f));
			interactions->AddFeedbackBehaviour((InteractionFeedback(std::vector<InteractionTForm>{screenTF}, extraScreen)));

			interactions->AddFeedbackBehaviour((InteractionFeedback(3)));

			interactions->prompt = prompt;
			interactions->screen = extraScreen;

			trashObject->SetScale(glm::vec3(1.5, 1.5, 1.5));
			trashObject->Add<ObjectLinking>(megaKitchenObject);
			megaKitchenObject->Get<ObjectLinking>()->LinkObject(trashObject);
		}

		GameObject::Sptr mousetrapObject = MakeBasic("Mouse trap Object", 10.64f, 147.72f, -49.81f, 90, 0, 0, 4, mouseTrapMaterial, mouseTrapMesh);
		{
			TriggerVolume::Sptr volume = mousetrapObject->Add<TriggerVolume>();
			SphereCollider::Sptr collider = SphereCollider::Create(1.5);
			collider->SetPosition(glm::vec3(0.f));
			volume->AddCollider(collider);

			InteractableObjectBehaviour::Sptr interactions = mousetrapObject->Add<InteractableObjectBehaviour>();
			interactions->AddRewardMaterial(handMouseMaterial);
			interactions->AddSoundEffect("mouse");

			interactions->AddFeedbackBehaviour((InteractionFeedback(mouseInteractMaterial, extraScreen)));
			InteractionTForm screenTF(InteractionTForm::tformt::pos, glm::vec3(5.87f, 5.79f, 6.9f));
			interactions->AddFeedbackBehaviour((InteractionFeedback(std::vector<InteractionTForm>{screenTF}, extraScreen)));
			
			interactions->AddFeedbackBehaviour((InteractionFeedback(0)));
			
			interactions->prompt = prompt;
			interactions->screen = extraScreen;

			mousetrapObject->Add<ObjectLinking>(megaKitchenObject);
			megaKitchenObject->Get<ObjectLinking>()->LinkObject(mousetrapObject);
		}

		GameObject::Sptr cakeObject = MakeBasic("Cake Object", 4.37f, 142.73f, -44.65f, 90, 0, 0, 4, cakeMaterial,cakeMesh);
		{
			TriggerVolume::Sptr volume = cakeObject->Add<TriggerVolume>();
			SphereCollider::Sptr collider = SphereCollider::Create(1.5);
			collider->SetPosition(glm::vec3(0.f));
			volume->AddCollider(collider);

			InteractableObjectBehaviour::Sptr interactions = cakeObject->Add<InteractableObjectBehaviour>();
			interactions->AddRewardMaterial(handCakeMaterial);
			interactions->AddSoundEffect("cake");

			interactions->AddFeedbackBehaviour((InteractionFeedback(cakeInteractMaterial, extraScreen)));
			InteractionTForm screenTF(InteractionTForm::tformt::pos, glm::vec3(5.87f, 5.79f, 6.9f));
			interactions->AddFeedbackBehaviour((InteractionFeedback(std::vector<InteractionTForm>{screenTF}, extraScreen)));

			interactions->AddFeedbackBehaviour((InteractionFeedback(1)));

			interactions->prompt = prompt;
			interactions->screen = extraScreen;

			cakeObject->Add<ObjectLinking>(megaKitchenObject);
			megaKitchenObject->Get<ObjectLinking>()->LinkObject(cakeObject);
		}

		GameObject::Sptr bowlOfFruitObject = MakeBasic("Bowl Of Fruit Object", -7.01f, 144.91f, -45.19f, 90, 0, 0, 4, bowlOfFruitMaterial, bowlOfFruitMesh);
		{
			TriggerVolume::Sptr volume = bowlOfFruitObject->Add<TriggerVolume>();
			SphereCollider::Sptr collider = SphereCollider::Create(1.5);
			collider->SetPosition(glm::vec3(0.f));
			volume->AddCollider(collider);

			InteractableObjectBehaviour::Sptr interactions = bowlOfFruitObject->Add<InteractableObjectBehaviour>();
			interactions->AddRewardMaterial(handFruitMaterial);
			interactions->AddSoundEffect("fruit");

			interactions->AddFeedbackBehaviour((InteractionFeedback(fruitInteractMaterial, extraScreen)));
			InteractionTForm screenTF(InteractionTForm::tformt::pos, glm::vec3(5.87f, 5.79f, 6.9f));
			interactions->AddFeedbackBehaviour((InteractionFeedback(std::vector<InteractionTForm>{screenTF}, extraScreen)));

			interactions->AddFeedbackBehaviour((InteractionFeedback(2)));

			interactions->prompt = prompt;
			interactions->screen = extraScreen;

			bowlOfFruitObject->Add<ObjectLinking>(megaKitchenObject);
			megaKitchenObject->Get<ObjectLinking>()->LinkObject(bowlOfFruitObject);
		}


		//Objects for living room
		GameObject::Sptr glassTableObject = MakeBasic("Glass Table Object", 0.36f, 199.9f, -49.73f, 90, 0, 79, 5, glassTableMaterial, glassTableMesh);
		{
			TriggerVolume::Sptr volume = glassTableObject->Add<TriggerVolume>();
			BoxCollider::Sptr collider = BoxCollider::Create(glm::vec3(1.0));
			collider->SetPosition(glm::vec3(0.f));
			volume->AddCollider(collider);

			InteractableObjectBehaviour::Sptr interactions = glassTableObject->Add<InteractableObjectBehaviour>();
			interactions->AddRewardMaterial(handAngryMaterial);
			interactions->AddSoundEffect("table");

			interactions->AddFeedbackBehaviour((InteractionFeedback(tableInteractMaterial, extraScreen)));
			InteractionTForm screenTF(InteractionTForm::tformt::pos, glm::vec3(5.87f, 5.79f, 6.9f));
			interactions->AddFeedbackBehaviour((InteractionFeedback(std::vector<InteractionTForm>{screenTF}, extraScreen)));
			interactions->AddFeedbackBehaviour((InteractionFeedback(brokenTableMesh, glassTableObject)));

			interactions->AddFeedbackBehaviour((InteractionFeedback(3)));

			interactions->prompt = prompt;
			interactions->screen = extraScreen;

			glassTableObject->SetScale(glm::vec3(1.5, 1.5, 1.5));
			glassTableObject->Add<ObjectLinking>(megaLivingroomObject);
			megaLivingroomObject->Get<ObjectLinking>()->LinkObject(glassTableObject);
		}

		GameObject::Sptr picturesObject = MakeBasic("Pictures Object", 8.33f, 186.03f, -49.69f, 0, 0, 83, 5, picturesMaterial, picturesMesh);
		{
			TriggerVolume::Sptr volume = picturesObject->Add<TriggerVolume>();
			BoxCollider::Sptr collider = BoxCollider::Create(glm::vec3(1.0));
			collider->SetPosition(glm::vec3(0.f));
			volume->AddCollider(collider);

			InteractableObjectBehaviour::Sptr interactions = picturesObject->Add<InteractableObjectBehaviour>();
			interactions->AddRewardMaterial(handClownMaterial);
			interactions->AddSoundEffect("pictures");

			interactions->AddFeedbackBehaviour((InteractionFeedback(picturesInteractMaterial, extraScreen)));
			InteractionTForm screenTF(InteractionTForm::tformt::pos, glm::vec3(5.87f, 5.79f, 6.9f));
			interactions->AddFeedbackBehaviour((InteractionFeedback(std::vector<InteractionTForm>{screenTF}, extraScreen)));
			interactions->AddFeedbackBehaviour((InteractionFeedback(clownPicsMaterial, picturesObject)));

			interactions->AddFeedbackBehaviour((InteractionFeedback(2)));

			interactions->prompt = prompt;
			interactions->screen = extraScreen;

			picturesObject->Add<ObjectLinking>(megaLivingroomObject);
			megaLivingroomObject->Get<ObjectLinking>()->LinkObject(picturesObject);
		}

		GameObject::Sptr plantObject = MakeBasic("Plant Object", -10.35f, 210.15f, -44.63f, 90, 0, 81, 5, plantMaterial, plantFrame0);
		{
			plantObject->SetScale(glm::vec3(0.5, 0.5, 0.5));

			TriggerVolume::Sptr volume = plantObject->Add<TriggerVolume>();
			SphereCollider::Sptr collider = SphereCollider::Create(1.5);
			collider->SetPosition(glm::vec3(0.f));
			volume->AddCollider(collider);

			InteractableObjectBehaviour::Sptr interactions = plantObject->Add<InteractableObjectBehaviour>();
			interactions->AddRewardMaterial(handGreenMaterial);
			interactions->AddSoundEffect("plant");

			interactions->AddFeedbackBehaviour((InteractionFeedback(plantInteractMaterial, extraScreen)));
			InteractionTForm screenTF(InteractionTForm::tformt::pos, glm::vec3(5.87f, 5.79f, 6.9f));
			interactions->AddFeedbackBehaviour((InteractionFeedback(std::vector<InteractionTForm>{screenTF}, extraScreen)));
			interactions->AddFeedbackBehaviour((InteractionFeedback(monsterPlantMesh, plantObject)));
			interactions->AddFeedbackBehaviour((InteractionFeedback(monsterPlantMaterial, plantObject)));
			interactions->AddFeedbackBehaviour((InteractionFeedback(1, plantObject)));

			interactions->AddFeedbackBehaviour((InteractionFeedback(0)));

			interactions->prompt = prompt;
			interactions->screen = extraScreen;

			MorphRenderComponent::Sptr morphRender = plantObject->Add<MorphRenderComponent>(plantFrame0);
			MorphAnimationManager::Sptr animator = plantObject->Add<MorphAnimationManager>();
			animator->AddAnim(std::vector<MeshResource::Sptr>{plantFrame1, plantFrame2, plantFrame3, plantFrame4}, 0.5);
			animator->AddAnim(std::vector<MeshResource::Sptr>{monsterPlantFrame0, monsterPlantFrame1, monsterPlantFrame2}, 0.5);

			plantObject->Add<ObjectLinking>(megaLivingroomObject);
			megaLivingroomObject->Get<ObjectLinking>()->LinkObject(plantObject);
		}

		GameObject::Sptr remoteObject = MakeBasic("Remote Object", -4.6f, 204.75f, -48.3f, 90, 0, -43, 5, remoteMaterial, remoteMesh);
		{
			TriggerVolume::Sptr volume = remoteObject->Add<TriggerVolume>();
			BoxCollider::Sptr collider = BoxCollider::Create(glm::vec3(1.0));
			collider->SetPosition(glm::vec3(0.f));
			volume->AddCollider(collider);

			InteractableObjectBehaviour::Sptr interactions = remoteObject->Add<InteractableObjectBehaviour>();
			interactions->AddRewardMaterial(handTechMaterial);
			interactions->AddSoundEffect("remote");

			interactions->AddFeedbackBehaviour((InteractionFeedback(remoteInteractMaterial, extraScreen)));
			InteractionTForm screenTF(InteractionTForm::tformt::pos, glm::vec3(5.87f, 5.79f, 6.9f));
			interactions->AddFeedbackBehaviour((InteractionFeedback(std::vector<InteractionTForm>{screenTF}, extraScreen)));
			InteractionTForm remoteMove(InteractionTForm::tformt::pos, glm::vec3(-4.6, 4.75, -5.0));
			interactions->AddFeedbackBehaviour((InteractionFeedback(std::vector<InteractionTForm>{remoteMove}, remoteObject)));

			interactions->AddFeedbackBehaviour((InteractionFeedback(1)));

			interactions->prompt = prompt;
			interactions->screen = extraScreen;

			remoteObject->SetScale(glm::vec3(0.3, 0.3, 0.3));
			remoteObject->Add<ObjectLinking>(megaLivingroomObject);
			megaLivingroomObject->Get<ObjectLinking>()->LinkObject(remoteObject);
		}

		GameObject::Sptr TVObject = MakeBasic("TV Object", -2.57f, 187.29f, -41.16f, 90, 0, -101, 5, TVMaterial, TVMesh);
		{
			
			TVObject->Add<ObjectLinking>(megaLivingroomObject);
			megaLivingroomObject->Get<ObjectLinking>()->LinkObject(TVObject);

			remoteObject->Get<InteractableObjectBehaviour>()->AddFeedbackBehaviour(InteractionFeedback(brokenTVmaterial, TVObject));
		}



		//Objects for Basement
		GameObject::Sptr teslaCoilObject = MakeBasic("Tesla Coil Object", -8.72f, 249.6f, -47.91f, 90, 0, 0, 6, teslaCoilMaterial, teslaCoilMesh);
		{
			TriggerVolume::Sptr volume = teslaCoilObject->Add<TriggerVolume>();
			BoxCollider::Sptr collider = BoxCollider::Create(glm::vec3(1.3, 5.0, 1.3));
			collider->SetPosition(glm::vec3(0.f));
			volume->AddCollider(collider);

			RigidBody::Sptr physics = teslaCoilObject->Add<RigidBody>(RigidBodyType::Kinematic);
			physics->AddCollider(BoxCollider::Create(glm::vec3(1, 4.2, 1)));

			InteractableObjectBehaviour::Sptr interactions = teslaCoilObject->Add<InteractableObjectBehaviour>();
			interactions->AddRewardMaterial(handElectricMaterial);
			interactions->AddSoundEffect("tesla");

			interactions->AddFeedbackBehaviour((InteractionFeedback(teslaInteractMaterial, extraScreen)));
			InteractionTForm screenTF(InteractionTForm::tformt::pos, glm::vec3(5.87f, 5.79f, 6.9f));
			interactions->AddFeedbackBehaviour((InteractionFeedback(std::vector<InteractionTForm>{screenTF}, extraScreen)));
			interactions->AddFeedbackBehaviour((InteractionFeedback(teslaCoilOnMaterial, teslaCoilObject)));
			interactions->AddFeedbackBehaviour((InteractionFeedback(4)));

			interactions->prompt = prompt;
			interactions->screen = extraScreen;

			teslaCoilObject->Add<ObjectLinking>(megaBasementObject);
			megaBasementObject->Get<ObjectLinking>()->LinkObject(teslaCoilObject);
		}

		GameObject::Sptr frankenstineObject = MakeBasic("Frankenstine Object", -1.424f, 249.87f, -49.97f, 90, -1, -87, 6, frankenstineMaterial, frankenstineMesh);
		{
			TriggerVolume::Sptr volume = frankenstineObject->Add<TriggerVolume>();
			BoxCollider::Sptr collider = BoxCollider::Create(glm::vec3(1.68, 0.75, 1.56));
			collider->SetPosition(glm::vec3(-0.09, 4.74, 0.75));
			volume->AddCollider(collider);

			InteractableObjectBehaviour::Sptr interactions = frankenstineObject->Add<InteractableObjectBehaviour>();
			interactions->AddRewardMaterial(handFrankMaterial);
			interactions->AddSoundEffect("frank");

			interactions->AddFeedbackBehaviour((InteractionFeedback(frankInteractMaterial, extraScreen)));
			InteractionTForm screenTF(InteractionTForm::tformt::pos, glm::vec3(5.87f, 5.79f, 6.9f));
			interactions->AddFeedbackBehaviour((InteractionFeedback(std::vector<InteractionTForm>{screenTF}, extraScreen)));
			interactions->AddFeedbackBehaviour((InteractionFeedback(frankensteinLessMesh, frankenstineObject)));

			interactions->AddFeedbackBehaviour((InteractionFeedback(0)));

			interactions->prompt = prompt;
			interactions->screen = extraScreen;

			frankenstineObject->SetScale(glm::vec3(0.85, 0.85, 0.85));
			frankenstineObject->Add<ObjectLinking>(megaBasementObject);
			megaBasementObject->Get<ObjectLinking>()->LinkObject(frankenstineObject);
		}

		GameObject::Sptr flasksObject = MakeBasic("Flasks Object", 3.13f, 242.4f, -46.7f, 90, 0, 0, 6, flasksMaterial, flasksMesh);
		{
			TriggerVolume::Sptr volume = flasksObject->Add<TriggerVolume>();
			BoxCollider::Sptr collider = BoxCollider::Create(glm::vec3(1.0, 0.88, 1.11));
			collider->SetPosition(glm::vec3(0.33, 0.75, 0.0));
			collider->SetRotation(glm::vec3(0.0, -24.0, 0.0));
			volume->AddCollider(collider);


			InteractableObjectBehaviour::Sptr interactions = flasksObject->Add<InteractableObjectBehaviour>();
			interactions->AddRewardMaterial(handScienceMaterial);
			interactions->AddSoundEffect("lab");

			interactions->AddFeedbackBehaviour((InteractionFeedback(alchemyInteractMaterial, extraScreen)));
			InteractionTForm screenTF(InteractionTForm::tformt::pos, glm::vec3(5.87f, 5.79f, 6.9f));
			interactions->AddFeedbackBehaviour((InteractionFeedback(std::vector<InteractionTForm>{screenTF}, extraScreen)));
			interactions->AddFeedbackBehaviour((InteractionFeedback(boiledOverFlaskMesh, flasksObject)));
			interactions->AddFeedbackBehaviour((InteractionFeedback(expFlasksMaterial, flasksObject)));

			interactions->AddFeedbackBehaviour((InteractionFeedback(2)));

			interactions->prompt = prompt;
			interactions->screen = extraScreen;

			flasksObject->SetScale(glm::vec3(0.3, 0.3, 0.3));
			flasksObject->Add<ObjectLinking>(megaBasementObject);
			megaBasementObject->Get<ObjectLinking>()->LinkObject(flasksObject);
		}

		GameObject::Sptr chainsawObject = MakeBasic("Chainsaw Object", 6.65f, 248.0f, -48.66f, 90, 0, 31, 6, chainsawMaterial, chainsawMesh);
		{
			TriggerVolume::Sptr volume = chainsawObject->Add<TriggerVolume>();
			BoxCollider::Sptr collider = BoxCollider::Create(glm::vec3(0.8, 1.2, 2.5));
			collider->SetPosition(glm::vec3(0.0, 0.0, -1.66));
			volume->AddCollider(collider);


			InteractableObjectBehaviour::Sptr interactions = chainsawObject->Add<InteractableObjectBehaviour>();
			interactions->AddRewardMaterial(handKillerMaterial);
			interactions->AddSoundEffect("chainsaw");

			interactions->AddFeedbackBehaviour((InteractionFeedback(chainsawInteractMaterial, extraScreen)));
			InteractionTForm screenTF(InteractionTForm::tformt::pos, glm::vec3(5.87f, 5.79f, 6.9f));
			interactions->AddFeedbackBehaviour((InteractionFeedback(std::vector<InteractionTForm>{screenTF}, extraScreen)));
			InteractionTForm chainsawPos(InteractionTForm::tformt::pos, glm::vec3(0.21, -7.88, 5.27));
			InteractionTForm chainsawRot(InteractionTForm::tformt::rot, glm::vec3(-124, -10, 16));
			interactions->AddFeedbackBehaviour((InteractionFeedback(std::vector<InteractionTForm>{chainsawPos, chainsawRot}, chainsawObject)));

			interactions->AddFeedbackBehaviour((InteractionFeedback(3)));

			interactions->prompt = prompt;
			interactions->screen = extraScreen;

			chainsawObject->SetScale(glm::vec3(0.7, 0.7, 0.7));
			chainsawObject->Add<ObjectLinking>(megaBasementObject);
			megaBasementObject->Get<ObjectLinking>()->LinkObject(chainsawObject);
		}

		GameObject::Sptr boilerObject = MakeBasic("Boiler Object", -6.63f, 255.02f, -48.46f, 90, 0, -125, 6, boilerMaterial, boilerFrame0);
		{
			TriggerVolume::Sptr volume = boilerObject->Add<TriggerVolume>();
			BoxCollider::Sptr collider = BoxCollider::Create(glm::vec3(1.69, 1.0, 2.27));
			collider->SetPosition(glm::vec3(0.f));
			volume->AddCollider(collider);

			RigidBody::Sptr physics = boilerObject->Add<RigidBody>(RigidBodyType::Kinematic);
			physics->AddCollider(BoxCollider::Create(glm::vec3(1.24, 2.92, 1.878)));

			MorphRenderComponent::Sptr morphRend = boilerObject->Add<MorphRenderComponent>(boilerFrame0);
			MorphAnimationManager::Sptr morphManager = boilerObject->Add<MorphAnimationManager>();
			morphManager->AddAnim(std::vector<MeshResource::Sptr>{ boilerFrame0, boilerFrame0}, 0.5);
			morphManager->AddAnim(std::vector<MeshResource::Sptr>{boilerFrame1, boilerFrame2, boilerFrame3, boilerFrame4}, 0.5);
			morphManager->SetCurrentAnim(0);

			InteractableObjectBehaviour::Sptr interactions = boilerObject->Add<InteractableObjectBehaviour>();
			interactions->AddRewardMaterial(handBurnedMaterial);
			interactions->AddSoundEffect("boiler");

			interactions->AddFeedbackBehaviour((InteractionFeedback(boilerInteractMaterial, extraScreen)));
			InteractionTForm screenTF(InteractionTForm::tformt::pos, glm::vec3(5.87f, 5.79f, 6.9f));
			interactions->AddFeedbackBehaviour((InteractionFeedback(std::vector<InteractionTForm>{screenTF}, extraScreen)));
			interactions->AddFeedbackBehaviour((InteractionFeedback(1, boilerObject)));
			

			interactions->AddFeedbackBehaviour((InteractionFeedback(1)));

			interactions->prompt = prompt;
			interactions->screen = extraScreen;

			boilerObject->SetScale(glm::vec3(0.7, 0.7, 0.7));
			boilerObject->Add<ObjectLinking>(megaBasementObject);
			megaBasementObject->Get<ObjectLinking>()->LinkObject(boilerObject);
		}

		GameObject::Sptr alchemyCollider = scene->CreateGameObject("Alchemy Collider");
		{
			alchemyCollider->SetPosition(glm::vec3(4.14, 241.67f, -46.96f));
			alchemyCollider->SetRotation(glm::vec3(0, 0, -24));

			RigidBody::Sptr physics = alchemyCollider->Add<RigidBody>(RigidBodyType::Kinematic);
			physics->AddCollider(BoxCollider::Create(glm::vec3(2.07, 1.83, 0.55)));;
			
			ObjectLinking::Sptr link = alchemyCollider->Add<ObjectLinking>(flasksObject);
			flasksObject->Get<ObjectLinking>()->LinkObject(alchemyCollider);
		}

		GameObject::Sptr frankensteinCollider = scene->CreateGameObject("Frankenstein Collider");
		{
			frankensteinCollider->SetPosition(glm::vec3(-0.404, 250.25, -48.35));
			frankensteinCollider->SetRotation(glm::vec3(0, 0, -1));


			RigidBody::Sptr physics = frankensteinCollider->Add<RigidBody>(RigidBodyType::Kinematic);
			physics->AddCollider(BoxCollider::Create(glm::vec3(3.72, 1.79, 2.17)));

			ObjectLinking::Sptr link = frankensteinCollider->Add<ObjectLinking>(frankenstineObject);
			frankenstineObject->Get<ObjectLinking>()->LinkObject(frankensteinCollider);
		}

		GameObject::Sptr chainsawCollider = scene->CreateGameObject("Chainsaw Collider");
		{
			chainsawCollider->SetPosition(glm::vec3(5.89f, 249.34f, -48.66f));
			chainsawCollider->SetRotation(glm::vec3(0, 0, -59.f));

			RigidBody::Sptr physics = chainsawCollider->Add<RigidBody>(RigidBodyType::Kinematic);
			physics->AddCollider(BoxCollider::Create(glm::vec3(2.20, 0.46, 0.72)));

			InteractionTForm chainsawPos(InteractionTForm::tformt::pos, glm::vec3(0.44, -9.36, 5.73));
			InteractionTForm chainsawRot(InteractionTForm::tformt::rot, glm::vec3(-14, 32, 99));
			chainsawObject->Get<InteractableObjectBehaviour>()->AddFeedbackBehaviour((InteractionFeedback(std::vector<InteractionTForm>{chainsawPos, chainsawRot}, chainsawCollider)));

			ObjectLinking::Sptr link = chainsawCollider->Add<ObjectLinking>(chainsawObject);
			chainsawObject->Get<ObjectLinking>()->LinkObject(chainsawCollider);
		}

		

		//bedroom objects 
		GameObject::Sptr radio = MakeBasic("Radio", -7.4f, -3.1f, 0.0f, 0.0f, 0.0f, 0.0f, 1, radioMaterial, radioFrame0);
		{
			
			TriggerVolume::Sptr volume = radio->Add<TriggerVolume>();
			SphereCollider::Sptr collider = SphereCollider::Create(1.5);
			collider->SetPosition(glm::vec3(0.f));
			volume->AddCollider(collider);

			InteractableObjectBehaviour::Sptr interactions = radio->Add<InteractableObjectBehaviour>();
			interactions->AddRewardMaterial(handMusicMaterial);     
			interactions->AddSoundEffect("velcro");

			interactions->AddFeedbackBehaviour((InteractionFeedback(radioMaterial2, radio)));
			interactions->AddFeedbackBehaviour((InteractionFeedback(radioInteractMaterial, extraScreen)));
			InteractionTForm screenTF(InteractionTForm::tformt::pos, glm::vec3(5.87f, 5.79f, 6.9f));
			interactions->AddFeedbackBehaviour((InteractionFeedback(std::vector<InteractionTForm>{screenTF}, extraScreen)));
			interactions->AddFeedbackBehaviour((InteractionFeedback(1)));
			interactions->prompt = prompt;
			interactions->screen = extraScreen;

			MorphRenderComponent::Sptr morphRenderer = radio->Add<MorphRenderComponent>(radioFrame0);

			MorphAnimationManager::Sptr animator = radio->Add<MorphAnimationManager>();
			animator->AddAnim(std::vector<Gameplay::MeshResource::Sptr>{radioFrame1, radioFrame2, radioFrame3, radioFrame4, radioFrame5, radioFrame6}, 0.5);
			animator->SetContinuity(true); 

			radio->Add<ObjectLinking>(bedroomObject);
			bedroomObject->Get<ObjectLinking>()->LinkObject(radio);
		}

		GameObject::Sptr homework = MakeBasic("Homework", -5.f, 3.45f, 3.3f, 0.0f, 0.0f, 0.0f, 1, homeworkMaterial, homeworkFrame0);
		{
			

			TriggerVolume::Sptr volume = homework->Add<TriggerVolume>();
			SphereCollider::Sptr collider = SphereCollider::Create(1.5);
			collider->SetPosition(glm::vec3(0.f));
			volume->AddCollider(collider);

			InteractableObjectBehaviour::Sptr interactions = homework->Add<InteractableObjectBehaviour>();
			interactions->AddRewardMaterial(handHomeworkMaterial);
			interactions->AddSoundEffect("books");

			interactions->AddFeedbackBehaviour((InteractionFeedback(1, homework)));
			InteractionTForm tf(InteractionTForm::tformt::pos, glm::vec3(2.01f, 0.69f, 0.1f));
			interactions->AddFeedbackBehaviour((InteractionFeedback(std::vector<InteractionTForm>{tf}, homework)));
			interactions->AddFeedbackBehaviour((InteractionFeedback(booksInteractMaterial, extraScreen)));
			InteractionTForm screenTF(InteractionTForm::tformt::pos, glm::vec3(5.87f, 5.79f, 6.9f));
			interactions->AddFeedbackBehaviour((InteractionFeedback(std::vector<InteractionTForm>{screenTF}, extraScreen)));
			interactions->AddFeedbackBehaviour((InteractionFeedback(2)));
			interactions->prompt = prompt;
			interactions->screen = extraScreen;
			MorphRenderComponent::Sptr morphRenderer = homework->Add<MorphRenderComponent>(homeworkFrame0);

			MorphAnimationManager::Sptr animator = homework->Add<MorphAnimationManager>();
			animator->AddAnim(std::vector<Gameplay::MeshResource::Sptr>{homeworkFrame1, homeworkFrame2}, 2.0);
			animator->AddAnim(std::vector<Gameplay::MeshResource::Sptr>{homeworkMessFrame1, homeworkMessFrame1}, 2.0);
			
			animator->SetContinuity(true);

			homework->Add<ObjectLinking>(bedroomObject);
			bedroomObject->Get<ObjectLinking>()->LinkObject(homework);
		}
		
		GameObject::Sptr shroomba = MakeBasic("Bedroom Shroomba", 2.f, 2.f, 0.f, 0.0f, 0.0f, 0.0f, 1, shroombaMaterial, shroombaFrame0);
		{
			TriggerVolume::Sptr volume = shroomba->Add<TriggerVolume>();
			SphereCollider::Sptr collider = SphereCollider::Create(1.5);
			collider->SetPosition(glm::vec3(0.f));
			volume->AddCollider(collider);

			InteractableObjectBehaviour::Sptr interactions = shroomba->Add<InteractableObjectBehaviour>();
			interactions->AddRewardMaterial(handShroomMaterial);
			interactions->AddSoundEffect("velcro");

			InteractionTForm tf(InteractionTForm::tformt::rot, glm::vec3(180.f, 0.f, 0.f));
			interactions->AddFeedbackBehaviour((InteractionFeedback(std::vector<InteractionTForm>{tf}, shroomba)));
			InteractionTForm secretTF(InteractionTForm::tformt::pos, glm::vec3(12.11, -0.55, 1.71));
			interactions->AddFeedbackBehaviour((InteractionFeedback(std::vector<InteractionTForm>{ secretTF }, secretText)));
			interactions->AddFeedbackBehaviour((InteractionFeedback(shroombaInteractMaterial, extraScreen)));
			InteractionTForm screenTF(InteractionTForm::tformt::pos, glm::vec3(5.87f, 5.79f, 6.9f));
			interactions->AddFeedbackBehaviour((InteractionFeedback(std::vector<InteractionTForm>{screenTF}, extraScreen)));
			interactions->AddFeedbackBehaviour((InteractionFeedback(4)));
			interactions->prompt = prompt;
			interactions->screen = extraScreen;

			MorphRenderComponent::Sptr morphRenderer = shroomba->Add<MorphRenderComponent>(shroombaFrame0);

			MorphAnimationManager::Sptr animator = shroomba->Add<MorphAnimationManager>();
			animator->AddAnim(std::vector<Gameplay::MeshResource::Sptr>{shroombaFrame1, shroombaFrame2, shroombaFrame3}, 2.4);
			animator->SetContinuity(true);
			
			InterpolationBehaviour::Sptr interp = shroomba->Add<InterpolationBehaviour>();
			interp->AddBehaviourScript("interp_scripts/shroombaPath.txt");

			ObjectLinking::Sptr link = shroomba->Add<ObjectLinking>(bedroomObject);
			bedroomObject->Get<ObjectLinking>()->LinkObject(shroomba);
			link->currentlyLinked = false;

		}

		GameObject::Sptr boybandPoster = MakeBasic("Boyband Poster", -9.4f, 3.f, 9.0f, 0.0f, 0.0f, 0.0f, 1, bbPosterMaterial, bbPosterMesh);
		{
			

			TriggerVolume::Sptr volume = boybandPoster->Add<TriggerVolume>();
			SphereCollider::Sptr collider = SphereCollider::Create(1.5);
			collider->SetPosition(glm::vec3(0.f));
			volume->AddCollider(collider);

			InteractableObjectBehaviour::Sptr interactions = boybandPoster->Add<InteractableObjectBehaviour>();
			interactions->AddRewardMaterial(handBoyBandMaterial);
			interactions->AddSoundEffect("velcro");

			interactions->AddFeedbackBehaviour((InteractionFeedback(bbPosterMesh2, boybandPoster)));
			InteractionTForm tf(InteractionTForm::tformt::pos, glm::vec3(2.01f, 0.69f, 0.1f));
			interactions->AddFeedbackBehaviour((InteractionFeedback(std::vector<InteractionTForm>{tf}, homework)));
			interactions->AddFeedbackBehaviour((InteractionFeedback(posterInteractMaterial, extraScreen)));
			InteractionTForm screenTF(InteractionTForm::tformt::pos, glm::vec3(5.87f, 5.79f, 6.9f));
			interactions->AddFeedbackBehaviour((InteractionFeedback(std::vector<InteractionTForm>{screenTF}, extraScreen)));
			interactions->AddFeedbackBehaviour((InteractionFeedback(0)));

			interactions->prompt = prompt;
			interactions->screen = extraScreen;

			boybandPoster->Add<ObjectLinking>(bedroomObject);
			bedroomObject->Get<ObjectLinking>()->LinkObject(boybandPoster);
		}

		GameObject::Sptr paintCan = MakeBasic("Paint Can", 0.45f, -5.1f, 6.44f, 0.0f, 0.0f, 0.0f, 1, paintcanMaterial, paintcanMesh);
		{
			

			TriggerVolume::Sptr volume = paintCan->Add<TriggerVolume>();
			SphereCollider::Sptr collider = SphereCollider::Create(1.5);
			collider->SetPosition(glm::vec3(0.f));
			volume->AddCollider(collider);

			InteractableObjectBehaviour::Sptr interactions = paintCan->Add<InteractableObjectBehaviour>();
			interactions->AddRewardMaterial(handRainbowMaterial);
			interactions->AddSoundEffect("brush");

			interactions->AddFeedbackBehaviour((InteractionFeedback(paintedOverMaterial, bedroomObject)));
			InteractionTForm tf(InteractionTForm::tformt::pos, glm::vec3(0.f, 0.f, -10.f));
			interactions->AddFeedbackBehaviour((InteractionFeedback(std::vector<InteractionTForm>{tf}, paintCan)));
			interactions->AddFeedbackBehaviour((InteractionFeedback(paintInteractMaterial, extraScreen)));
			InteractionTForm screenTF(InteractionTForm::tformt::pos, glm::vec3(5.87f, 5.79f, 6.9f));
			interactions->AddFeedbackBehaviour((InteractionFeedback(std::vector<InteractionTForm>{screenTF}, extraScreen)));
			interactions->AddFeedbackBehaviour((InteractionFeedback(3)));

			interactions->prompt = prompt;
			interactions->screen = extraScreen;

			paintCan->Add<ObjectLinking>(bedroomObject);
			bedroomObject->Get<ObjectLinking>()->LinkObject(paintCan);
		}

		




		GameObject::Sptr boundingPosZ = scene->CreateGameObject("boundingPosZ");
		{
	
			boundingPosZ->SetPosition(glm::vec3(0, 0, 11));
			boundingPosZ->SetRotation(glm::vec3(0, 0, 0));

			RigidBody::Sptr physics = boundingPosZ->Add<RigidBody>(RigidBodyType::Kinematic);
			physics->AddCollider(BoxCollider::Create(glm::vec3(11,11,0.5)));

		}

		GameObject::Sptr boundingNegZ = scene->CreateGameObject("boundingNegZ");
		{

			boundingNegZ->SetPosition(glm::vec3(0, 0, -0.49));
			boundingNegZ->SetRotation(glm::vec3(0, 0, 0));

			RigidBody::Sptr physics = boundingNegZ->Add<RigidBody>(RigidBodyType::Kinematic);
			physics->AddCollider(BoxCollider::Create(glm::vec3(11, 11, 0.5)));
		}

		GameObject::Sptr boundingPosX = scene->CreateGameObject("boundingPosX");
		{

			boundingPosX->SetPosition(glm::vec3(5.87, 0, 6));
			boundingPosX->SetRotation(glm::vec3(0, 90, 0));

			RigidBody::Sptr physics = boundingPosX->Add<RigidBody>(RigidBodyType::Kinematic);
			physics->AddCollider(BoxCollider::Create(glm::vec3(11, 11, 0.5)));

		}

		GameObject::Sptr boundingNegX = scene->CreateGameObject("boundingNegX");
		{

			boundingNegX->SetPosition(glm::vec3(-9.98, 0, 6));
			boundingNegX->SetRotation(glm::vec3(0, 90, 0));

			RigidBody::Sptr physics = boundingNegX->Add<RigidBody>(RigidBodyType::Kinematic);
			physics->AddCollider(BoxCollider::Create(glm::vec3(11, 11, 0.5)));

		}

		GameObject::Sptr boundingPosY = scene->CreateGameObject("boundingPosY");
		{

			boundingPosY->SetPosition(glm::vec3(0, 5.91, 6));
			boundingPosY->SetRotation(glm::vec3(90, 0, 0));

			RigidBody::Sptr physics = boundingPosY->Add<RigidBody>(RigidBodyType::Kinematic);
			physics->AddCollider(BoxCollider::Create(glm::vec3(11, 11, 0.5)));

		}

		GameObject::Sptr boundingNegY = scene->CreateGameObject("boundingNegY");
		{

			boundingNegY->SetPosition(glm::vec3(0, -7, 6));
			boundingNegY->SetRotation(glm::vec3(90, 0, 0));

			RigidBody::Sptr physics = boundingNegY->Add<RigidBody>(RigidBodyType::Kinematic);
			physics->AddCollider(BoxCollider::Create(glm::vec3(11, 11, 0.5)));
		}



		//the    H A N D
		GameObject::Sptr hand = MakeBasic("Idle Hand", 0.f, 0.f, 2.f, 0.0f, 0.0f, 0.0f, 0, handDefaultMaterial, theHandMesh);
		{
			//make hand dynamic so that we can move it and it can interact with triggers
			RigidBody::Sptr physics = hand->Add<RigidBody>(RigidBodyType::Dynamic);
			physics->AddCollider(BoxCollider::Create(glm::vec3(1.7, 0.9, 0.8)));

			CharacterController::Sptr controller = hand->Add<CharacterController>();

			//add a skin manager to the hand so that we can change its appearance at will
			SkinManager::Sptr skinSwapper = hand->Add<SkinManager>();

			MorphRenderComponent::Sptr morph = hand->Add<MorphRenderComponent>(handIdle0);
			MorphAnimationManager::Sptr anims = hand->Add<MorphAnimationManager>();
			anims->AddAnim(std::vector<MeshResource::Sptr>{handIdle3, handIdle4, handIdle5}, 0.5);
		}

		// Call scene awake to start up all of our components
		scene->Window = window;
		scene->Awake();

		// Save the asset manifest for all the resources we just loaded
		ResourceManager::SaveManifest("manifest.json");
		// Save the scene to a JSON file
		scene->Save("scene.json");
	}
	scene->IsPlaying = true;

	// We'll use this to allow editing the save/load path
	// via ImGui, note the reserve to allocate extra space
	// for input!
	std::string scenePath = "scene.json"; 
	scenePath.reserve(256); 

	bool isRotating = true;
	float rotateSpeed = 90.0f;

	// Our high-precision timer
	double lastFrame = glfwGetTime();


	BulletDebugMode physicsDebugMode = BulletDebugMode::None;
	float playbackSpeed = 1.0f;

	nlohmann::json editorSceneState;

	///// Game loop /////
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		ImGuiHelper::StartFrame();

		// Calculate the time since our last frame (dt)
		double thisFrame = glfwGetTime();
		float dt = static_cast<float>(thisFrame - lastFrame);

		// Showcasing how to use the imGui library!
		bool isDebugWindowOpen = ImGui::Begin("Debugging");//false;
		if (isDebugWindowOpen) {
			// Draws a button to control whether or not the game is currently playing
			static char buttonLabel[64];
			sprintf_s(buttonLabel, "%s###playmode", scene->IsPlaying ? "Exit Play Mode" : "Enter Play Mode");
			if (ImGui::Button(buttonLabel)) {
				// Save scene so it can be restored when exiting play mode
				if (!scene->IsPlaying) {
					editorSceneState = scene->ToJson();
				}

				// Toggle state
				scene->IsPlaying = !scene->IsPlaying;

				// If we've gone from playing to not playing, restore the state from before we started playing
				if (!scene->IsPlaying) {
					scene = nullptr;
					// We reload to scene from our cached state
					scene = Scene::FromJson(editorSceneState);
					// Don't forget to reset the scene's window and wake all the objects!
					scene->Window = window;
					scene->Awake();
				}
			}

			// Make a new area for the scene saving/loading
			ImGui::Separator();
			if (DrawSaveLoadImGui(scene, scenePath)) {
				// C++ strings keep internal lengths which can get annoying
				// when we edit it's underlying datastore, so recalcualte size
				scenePath.resize(strlen(scenePath.c_str()));

				// We have loaded a new scene, call awake to set
				// up all our components
				scene->Window = window;
				scene->Awake();
			}
			ImGui::Separator();
			// Draw a dropdown to select our physics debug draw mode
			if (BulletDebugDraw::DrawModeGui("Physics Debug Mode:", physicsDebugMode)) {
				scene->SetPhysicsDebugDrawMode(physicsDebugMode);
			}
			LABEL_LEFT(ImGui::SliderFloat, "Playback Speed:    ", &playbackSpeed, 0.0f, 10.0f);
			ImGui::Separator();
		}

		// Clear the color and depth buffers
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Update our application level uniforms every frame

		// Draw some ImGui stuff for the lights
		if (isDebugWindowOpen) {
			for (int ix = 0; ix < scene->Lights.size(); ix++) {
				char buff[256];
				sprintf_s(buff, "Light %d##%d", ix, ix);
				// DrawLightImGui will return true if the light was deleted
				if (DrawLightImGui(scene, buff, ix)) {
					// Remove light from scene, restore all lighting data
					scene->Lights.erase(scene->Lights.begin() + ix);
					scene->SetupShaderAndLights();
					// Move back one element so we don't skip anything!
					ix--;
				}
			}
			// As long as we don't have max lights, draw a button
			// to add another one
			if (scene->Lights.size() < scene->MAX_LIGHTS) {
				if (ImGui::Button("Add Light")) {
					scene->Lights.push_back(Light());
					scene->SetupShaderAndLights();
				}
			}
			// Split lights from the objects in ImGui
			ImGui::Separator();
		}
		
		dt *= playbackSpeed;
		
		// Perform updates for all components
		scene->Update(dt);

		// Grab shorthands to the camera and shader from the scene
		Camera::Sptr camera = scene->MainCamera;

		// Cache the camera's viewprojection
		glm::mat4 viewProj = camera->GetViewProjection();
		DebugDrawer::Get().SetViewProjection(viewProj);

		// Update our worlds physics!
		scene->DoPhysics(dt);

		// Draw object GUIs
		if (isDebugWindowOpen) {
			scene->DrawAllGameObjectGUIs();
		}
		
		// The current material that is bound for rendering
		Material::Sptr currentMat = nullptr;
		Shader::Sptr shader = nullptr;

		TextureCube::Sptr environment = scene->GetSkyboxTexture();
		if (environment) environment->Bind(0); 

		// Render all our objects
		ComponentManager::Each<RenderComponent>([&](const RenderComponent::Sptr& renderable) {
			// Early bail if mesh not set
			if (renderable->GetMesh() == nullptr) { 
				return;
			}

			// If we don't have a material, try getting the scene's fallback material
			// If none exists, do not draw anything
			if (renderable->GetMaterial() == nullptr) {
				if (scene->DefaultMaterial != nullptr) {
					renderable->SetMaterial(scene->DefaultMaterial);
				} else {
					return;
				}
			}

			

			// If the material has changed, we need to bind the new shader and set up our material and frame data
			// Note: This is a good reason why we should be sorting the render components in ComponentManager
			if (renderable->GetMaterial() != currentMat) {
				currentMat = renderable->GetMaterial();
				shader = currentMat->MatShader;

				shader->Bind();
				shader->SetUniform("u_CamPos", scene->MainCamera->GetGameObject()->GetPosition());
				currentMat->Apply();
			}

			// Grab the game object so we can do some stuff with it
			GameObject* object = renderable->GetGameObject();

			// Set vertex shader parameters
			shader->SetUniformMatrix("u_ModelViewProjection", viewProj * object->GetTransform());
			shader->SetUniformMatrix("u_Model", object->GetTransform());
			shader->SetUniformMatrix("u_NormalMatrix", glm::mat3(glm::transpose(glm::inverse(object->GetTransform()))));

			if (object->Has<MorphRenderComponent>()) { shader->SetUniform("u_t", object->Get<MorphRenderComponent>()->_t); } 


			if (object->Get<RenderComponent>()->floorTag != scene->FindObjectByName("Floor Manager")->Get<WarpBehaviour>()->currentFloor && object->Get<RenderComponent>()->floorTag != 0)
			{
				return;
			}
			// Draw the object
			renderable->GetMesh()->Draw();
		});

		// Use our cubemap to draw our skybox
		scene->DrawSkybox();


		
		if (isDebugWindowOpen) {// End our ImGui window
			ImGui::End();
		}
		VertexArrayObject::Unbind();

		lastFrame = thisFrame;
		
		ImGuiHelper::EndFrame();
		glfwSwapBuffers(window);
	}

	// Clean up the ImGui library
	ImGuiHelper::Cleanup();

	// Clean up the resource manager
	ResourceManager::Cleanup();

	// Clean up the toolkit logger so we don't leak memory
	Logger::Uninitialize();
	return 0;
}

