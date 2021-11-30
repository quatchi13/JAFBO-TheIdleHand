#include "Gameplay/Components/MorphAnimationManager.h"
#include "Gameplay/GameObject.h"
#include "Utils/ImGuiHelper.h"
#include "Utils/JsonGlmHelpers.h"
#include "Gameplay/Scene.h"
#include "GLFW/glfw3.h"

MorphAnimationManager::MorphAnim::MorphAnim(const std::vector<Gameplay::MeshResource::Sptr>& meshes, float pace) {
	index = 0;
	secondsBetween = pace;
	done = false;

	for (int i = 0; i < meshes.size(); i++) {
		morphFrames.push_back(meshes[i]);
	}
}

MorphAnimationManager::MorphAnimationManager():IComponent() {}
MorphAnimationManager::~MorphAnimationManager() = default;

void MorphAnimationManager::AddAnim(const std::vector<Gameplay::MeshResource::Sptr>& meshes, float pace) {
	MorphAnim anim(meshes, pace);
	animations.push_back(anim);
}

void MorphAnimationManager::SetCurrentAnim(int newCur) {
	if (newCur < animations.size()) {
		if (animations[currentAnimIndex].index) { animations[currentAnimIndex].index = 0; }

		currentAnimIndex = newCur;
	}
}

void MorphAnimationManager::SetAnOrder(std::vector<int> anOrder) {
	if (order.size()) { order.clear(); }

	for (int i = 0; i < anOrder.size(); i++) {
		order.push_back(anOrder[i]);
	}

	hasDeclaredOrder = true;
	orderIndex = -1;

}

void MorphAnimationManager::SetContinuity(bool c) {
	isContinuous = c;
	futureContinuity = c;
}




void MorphAnimationManager::Update(float deltaTime) {
	//ManageInput();

	if (currentlyAnimating) {

		if (animations[currentAnimIndex].done) {

			if (toBePaused) {
				toBePaused = false;
				currentlyAnimating = false;
			}
			else {
				if (hasDeclaredOrder) {
					orderIndex = (orderIndex == -1) ? 0 : orderIndex + 1;
					if (orderIndex == order.size()) {
						if (isContinuous) {
							orderIndex = 0;

							animations[currentAnimIndex].done = false;
							animations[currentAnimIndex].index = 0;

							SetCurrentAnim(order[orderIndex]);
							InterpolateMeshes(deltaTime);
						}
						else {
							orderIndex = 0;
							hasDeclaredOrder = false;
							animations[currentAnimIndex].done = false;
							animations[currentAnimIndex].index = 0;
							currentAnimIndex = curIdleIndex;
							InterpolateMeshes(deltaTime);
						}
					}
					else {
						animations[currentAnimIndex].done = false;
						animations[currentAnimIndex].index = 0;
						SetCurrentAnim(order[orderIndex]);
						InterpolateMeshes(deltaTime);
					}
				}
				else {
					animations[currentAnimIndex].done = false;
					animations[currentAnimIndex].index = 0;
					InterpolateMeshes(deltaTime);
				}
			}

		}
		else {
			InterpolateMeshes(deltaTime);
		}

	}

}

void MorphAnimationManager::InterpolateMeshes(float deltaTime) {
	float t;
	int index1;

	m_timer += deltaTime;

	if (m_timer > animations[currentAnimIndex].secondsBetween) {
		m_timer = 0.f;

		animations[currentAnimIndex].index++;

		if (animations[currentAnimIndex].index >= (animations[currentAnimIndex].morphFrames.size() - 1)) {

			if (hasDeclaredOrder && !isContinuous) {
				animations[currentAnimIndex].done = true;
			}
			else if (animations[currentAnimIndex].index > (animations[currentAnimIndex].morphFrames.size() - 1)) {
				animations[currentAnimIndex].index = 0;
			}


		}


	}

	if (!animations[currentAnimIndex].done) {
		t = m_timer / animations[currentAnimIndex].secondsBetween;
		index1 = ((animations[currentAnimIndex].index) >= (animations[currentAnimIndex].morphFrames.size()-1)) ? 0 : (animations[currentAnimIndex].index + 1);

		GetGameObject()->Get<MorphRenderComponent>()->UpdateVBOs(animations[currentAnimIndex].morphFrames[animations[currentAnimIndex].index], animations[currentAnimIndex].morphFrames[index1], t);
	}
}





void MorphAnimationManager::RenderImGui() {
	
}

nlohmann::json MorphAnimationManager::ToJson() const {
	nlohmann::json result;
	return result;
}

MorphAnimationManager::Sptr MorphAnimationManager::FromJson(const nlohmann::json& data) {
	MorphAnimationManager::Sptr result = std::make_shared<MorphAnimationManager>();
	
	return result;
}