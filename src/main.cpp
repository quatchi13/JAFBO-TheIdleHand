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
glm::ivec2 windowSize = glm::ivec2(800, 800);
// The title of our GLFW window
std::string windowTitle = "INFR-1350U";

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
		interactable->SetPostion(interactablePos);
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
	

	// GL states, we'll enable depth testing and backface fulling
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
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
		// This time we'll have 2 different shaders, and share data between both of them using the UBO
		// This shader will handle reflective materials
		Shader::Sptr reflectiveShader = ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shader.glsl" },  
			{ ShaderPartType::Fragment, "shaders/frag_environment_reflective.glsl" }  
		}); 

		// This shader handles our basic materials without reflections (cause they expensive)
		Shader::Sptr basicShader = ResourceManager::CreateAsset<Shader>(std::unordered_map<ShaderPartType, std::string>{
			{ ShaderPartType::Vertex, "shaders/vertex_shader.glsl" },
			{ ShaderPartType::Fragment, "shaders/frag_blinn_phong_textured.glsl" }
		});

		//Meshes
		MeshResource::Sptr BedMesh = ResourceManager::CreateAsset<MeshResource>("meshes/Bed.obj");
		MeshResource::Sptr darkLampMesh = ResourceManager::CreateAsset<MeshResource>("meshes/DarkLamp.obj");
		MeshResource::Sptr lightLampMesh = ResourceManager::CreateAsset<MeshResource>("meshes/LightLamp.obj");
		MeshResource::Sptr monkeyMesh = ResourceManager::CreateAsset<MeshResource>("meshes/Monkey.obj");
		MeshResource::Sptr nightstandMesh = ResourceManager::CreateAsset<MeshResource>("meshes/Nightstand.obj");
		MeshResource::Sptr theHandMesh = ResourceManager::CreateAsset<MeshResource>("meshes/hand.obj");
		

		//Textures
		Texture2D::Sptr    bleedingPosterTex = ResourceManager::CreateAsset<Texture2D>("textures/BleedingPoster.png");
		Texture2D::Sptr    boxTexture = ResourceManager::CreateAsset<Texture2D>("textures/box-diffuse.png");
		Texture2D::Sptr    darkLampTex = ResourceManager::CreateAsset<Texture2D>("textures/DarkLamp.png");
		Texture2D::Sptr    drumstickPosterTex = ResourceManager::CreateAsset<Texture2D>("textures/DrumstickPoster.png");
		Texture2D::Sptr    floorTex = ResourceManager::CreateAsset<Texture2D>("textures/Floor.png");
		Texture2D::Sptr    leftWallTex = ResourceManager::CreateAsset<Texture2D>("textures/LeftWall.png");
		Texture2D::Sptr    lightLampTex = ResourceManager::CreateAsset<Texture2D>("textures/LightLamp.png");
		Texture2D::Sptr    missingTex = ResourceManager::CreateAsset<Texture2D>("textures/MissingTexture.png");
		Texture2D::Sptr    monkeyTex = ResourceManager::CreateAsset<Texture2D>("textures/monkey-uvMap.png");
		Texture2D::Sptr    pentagramPosterTex = ResourceManager::CreateAsset<Texture2D>("textures/PentagramPoster.png");
		Texture2D::Sptr    rightWallTex = ResourceManager::CreateAsset<Texture2D>("textures/RightWall.png");
		Texture2D::Sptr    rewardSkin = ResourceManager::CreateAsset<Texture2D>("textures/forggyBonds.png");

		// Here we'll load in the cubemap, as well as a special shader to handle drawing the skybox
		TextureCube::Sptr testCubemap = ResourceManager::CreateAsset<TextureCube>("cubemaps/ocean/ocean.jpg");
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
		Material::Sptr bleedingPosterMaterial = MakeMaterial("Bleeding Poster", basicShader,  bleedingPosterTex, 0.1f);
		Material::Sptr boxMaterial = MakeMaterial("Box", basicShader, boxTexture, 0.1f);
		Material::Sptr darkLampMaterial = MakeMaterial("Dark Lamp", basicShader, darkLampTex, 0.1f);
		Material::Sptr drumstickPosterMaterial = MakeMaterial("Drumstick Poster", basicShader, drumstickPosterTex, 0.1f);
		Material::Sptr floorMaterial = MakeMaterial("Floor", basicShader, floorTex, 0.1f);
		Material::Sptr leftWallMaterial = MakeMaterial("Left Wall", basicShader, leftWallTex, 0.1f);
		Material::Sptr lightLampMaterial = MakeMaterial("Light Lamp", basicShader, lightLampTex, 0.1f);
		Material::Sptr missingMaterial = MakeMaterial("Missing Texture", basicShader, missingTex, 0.1f);
		Material::Sptr rewardMaterial = MakeMaterial("Reward Material", reflectiveShader, rewardSkin, 0.5);
		Material::Sptr pentagramPosterMaterial = MakeMaterial("Pentagram Poster", basicShader, pentagramPosterTex, 0.1f);
		Material::Sptr rightWallMaterial = MakeMaterial("Right Wall", basicShader, rightWallTex, 0.1f);


		// Create some lights for our scene
		scene->Lights.resize(3);
		scene->Lights[0].Position = glm::vec3(0.0f, 1.0f, 3.0f);
		scene->Lights[0].Color = glm::vec3(1.0f, 1.0f, 1.0f);
		scene->Lights[0].Range = 100.0f;

		scene->Lights[1].Position = glm::vec3(1.0f, 0.0f, 3.0f);
		scene->Lights[1].Color = glm::vec3(0.2f, 0.8f, 0.1f);

		scene->Lights[2].Position = glm::vec3(0.0f, 1.0f, 3.0f);
		scene->Lights[2].Color = glm::vec3(1.0f, 0.2f, 0.1f);

		// We'll create a mesh that is a simple plane that we can resize later
		MeshResource::Sptr planeMesh = ResourceManager::CreateAsset<MeshResource>();
		planeMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(1.0f)));
		planeMesh->GenerateMesh();

		// Set up the scene's camera
		GameObject::Sptr camera = scene->CreateGameObject("Main Camera");
		{
			camera->SetPostion(glm::vec3(8.0f, 8.0f, 15.0f));
			camera->LookAt(glm::vec3(-7.5f, -7.5f, 4.0f));		

			Camera::Sptr cam = camera->Add<Camera>();
			// Make sure that the camera is set as the scene's main camera!
			scene->MainCamera = cam;
		}

		// Set up all our sample objects
		GameObject::Sptr floor = scene->CreateGameObject("Floor");
		{
			// Make a big tiled mesh
			MeshResource::Sptr tiledMesh = ResourceManager::CreateAsset<MeshResource>();
			tiledMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(15.0f)));
			tiledMesh->GenerateMesh();

			// Create and attach a RenderComponent to the object to draw our mesh
			RenderComponent::Sptr renderer = floor->Add<RenderComponent>();
			renderer->SetMesh(tiledMesh);
			renderer->SetMaterial(floorMaterial);

			// Attach a plane collider that extends infinitely along the X/Y axis
			RigidBody::Sptr physics = floor->Add<RigidBody>(/*static by default*/);
			physics->AddCollider(BoxCollider::Create(glm::vec3(50.0f, 50.0f, 1.0f)))->SetPosition({0,0,-1});
			floor->SetPostion(glm::vec3(-2.0f, -2.0f, 0.0f));

		}

		GameObject::Sptr lWall = scene->CreateGameObject("LeftWall");
		{
			// Make a big tiled mesh
			MeshResource::Sptr tiledMesh = ResourceManager::CreateAsset<MeshResource>();
			tiledMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(15.0f, 15.0f)));
			tiledMesh->GenerateMesh();

			// Create and attach a RenderComponent to the object to draw our mesh
			RenderComponent::Sptr renderer = lWall->Add<RenderComponent>();
			renderer->SetMesh(tiledMesh);
			renderer->SetMaterial(leftWallMaterial);

			// Attach a plane collider that extends infinitely along the X/Y axis
			RigidBody::Sptr physics = lWall->Add<RigidBody>(/*static by default*/);
			physics->AddCollider(BoxCollider::Create(glm::vec3(50.0f, 50.0f, 1.0f)))->SetPosition({ 0,0,-1 });
			lWall->SetPostion(glm::vec3(-2.0f, -9.5f, 7.5f));
			lWall->SetRotation(glm::vec3(-90.0f, 0.0f, 0.0f));

		}

		GameObject::Sptr rWall = scene->CreateGameObject("RightWall");
		{
			// Make a big tiled mesh
			MeshResource::Sptr tiledMesh = ResourceManager::CreateAsset<MeshResource>();
			tiledMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(15.0f, 15.0f)));
			tiledMesh->GenerateMesh();

			// Create and attach a RenderComponent to the object to draw our mesh
			RenderComponent::Sptr renderer = rWall->Add<RenderComponent>();
			renderer->SetMesh(tiledMesh);
			renderer->SetMaterial(rightWallMaterial);

			// Attach a plane collider that extends infinitely along the X/Y axis
			RigidBody::Sptr physics = rWall->Add<RigidBody>(/*static by default*/);
			physics->AddCollider(BoxCollider::Create(glm::vec3(50.0f, 50.0f, 1.0f)))->SetPosition({ 0,0,-1 });
			rWall->SetPostion(glm::vec3(-9.5f, -2.0f, 7.5f));
			rWall->SetRotation(glm::vec3(-90.0f, 0.0f, -90.0f));

		}

		GameObject::Sptr screen = scene->CreateGameObject("Screen");
		{
			// Make a big tiled mesh
			MeshResource::Sptr tiledMesh = ResourceManager::CreateAsset<MeshResource>();
			tiledMesh->AddParam(MeshBuilderParam::CreatePlane(ZERO, UNIT_Z, UNIT_X, glm::vec2(10.0f, 20.0f)));
			tiledMesh->GenerateMesh();

			// Create and attach a RenderComponent to the object to draw our mesh
			RenderComponent::Sptr renderer = screen->Add<RenderComponent>();
			renderer->SetMesh(tiledMesh);
			renderer->SetMaterial(rightWallMaterial);

			screen->SetPostion(glm::vec3(4.7f, 5.1f, 12.8f));
			screen->SetRotation(glm::vec3(0.0f, 63.0f, 45.0f));

			MainMenu::Sptr menu = screen->Add<MainMenu>();


		}

		//set up interactable GameObjects
		GameObject::Sptr nightstand = MakeInteractableObject("Nightstand", nightstandMesh, missingMaterial, rewardMaterial, glm::vec3(-8.5f, -2.0f, 0.0f));


		//set up the hand
		GameObject::Sptr hand = scene->CreateGameObject("Idle Hand");
		{
			hand->SetPostion(glm::vec3(0.f, 0.f, 2.f));
			
			//allows the hand to be rendered
			RenderComponent::Sptr renderer = hand->Add<RenderComponent>();
			renderer->SetMesh(theHandMesh);
			renderer->SetMaterial(missingMaterial);

			//make hand dynamic so that we can move it and it can interact with triggers
			RigidBody::Sptr physics = hand->Add<RigidBody>(RigidBodyType::Dynamic);
			physics->AddCollider(ConvexMeshCollider::Create());

			CharacterController::Sptr controller = hand->Add<CharacterController>();

			//add a skin manager to the hand so that we can change its appearance at will
			SkinManager::Sptr skinSwapper = hand->Add<SkinManager>();
		}

		// Call scene awake to start up all of our components
		scene->Window = window;
		scene->Awake();

		// Save the asset manifest for all the resources we just loaded
		ResourceManager::SaveManifest("manifest.json");
		// Save the scene to a JSON file
		scene->Save("scene.json");
	}
	

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
		bool isDebugWindowOpen = ImGui::Begin("Debugging");
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

			// Draw the object
			renderable->GetMesh()->Draw();
		});

		// Use our cubemap to draw our skybox
		scene->DrawSkybox();


		// End our ImGui window
		ImGui::End();

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