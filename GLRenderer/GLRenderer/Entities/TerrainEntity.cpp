#include <GLRendererStdafx.h>

#include <GLRenderer/Entities/TerrainEntity.h>

#include <GLRenderer/Components/TerrainMergedMesh.h>

#include <imgui.h>

namespace GLEngine {
namespace GLRenderer {

//=================================================================================
C_TerrainEntity::C_TerrainEntity()
	: m_ID(NextGUID())
	, m_SqPerLine(5)
	, m_Freq(13)
	, Controls(false)
	, Visualise(false)
{

}

//=================================================================================
GLEngine::Entity::I_Entity::EntityID C_TerrainEntity::GetID() const
{
	return m_ID;
}

//=================================================================================
GLEngine::T_ComponentPtr C_TerrainEntity::GetComponent(Entity::E_ComponentType type) const
{
	if (type == Entity::E_ComponentType::Graphical) {
		// should be frustum culled
		auto merged = std::make_shared<Components::C_TerrainMergedMesh>(m_Patches);
		return merged;
	}

	return nullptr;
}

//=================================================================================
std::string C_TerrainEntity::GetName() const
{
	return "Terrain";
}

//=================================================================================
void C_TerrainEntity::OnEvent(Core::I_Event& event)
{
	Controls = !Controls;
}

//=================================================================================
void C_TerrainEntity::Update()
{
	WholeTerrain([](T_TerrainPtr patch) {
		patch->UpdateStats();
	});
}

//=================================================================================
void C_TerrainEntity::AddPatch(T_TerrainPtr patch)
{
	m_Patches.push_back(patch);
	patch->SetSqPerLine(m_SqPerLine);
	patch->SetFrequncy(m_Freq);
}

//=================================================================================
void C_TerrainEntity::AddPatch(glm::ivec2 coord)
{
	auto comp = std::make_shared<Components::C_TerrainMesh>();
	comp->SetCoord(coord);
	AddPatch(comp);
}

//=================================================================================
void C_TerrainEntity::DrawControls()
{
	if (Controls) {
		::ImGui::Begin("Terrain controls", &Controls, ImGuiWindowFlags_MenuBar);
			if (::ImGui::BeginMenuBar())
			{
				if (::ImGui::BeginMenu("File"))
				{
					if (::ImGui::MenuItem("Open..", "Ctrl+O")) { /* Do stuff */ }
					if (::ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
					::ImGui::EndMenu();
				}
				::ImGui::EndMenuBar();
			}
			if (::ImGui::Button("Rain"))
			{
				WholeTerrain([&](T_TerrainPtr terrain) {
					terrain->Simulate();
				});
			}
			bool value = m_Patches[0]->UsingPerlinNoise();
			::ImGui::Checkbox("PerlinNoise", &value);
			WholeTerrain([&](T_TerrainPtr terrain) {
				terrain->UsePerlinNoise(value);
			});
			::ImGui::Checkbox("Visualization", &Visualise);
			::ImGui::SliderInt("Vertexes", &m_SqPerLine, 4, 16);
			::ImGui::SliderInt("Noise frequency", &m_Freq, 4, 40);
			WholeTerrain([&](T_TerrainPtr patch) {
				patch->SetFrequncy(m_Freq);
				patch->SetSqPerLine(m_SqPerLine);
			});
			WholeTerrain([&](T_TerrainPtr patch) {
				bool selected = false;
				auto coord = patch->GetCoord();
				std::stringstream ss;
				ss << "[" << coord.x << ":" << coord.y << "]";
				::ImGui::Selectable(ss.str().c_str(), &selected);
				if (selected) {
					patch->OnEvent(Core::C_UserEvent("selected"));
				}
			});
		::ImGui::End();
	}


	if (Visualise) {
		::ImGui::Begin("Terrain visualizations", &Visualise);
			WholeTerrain([](T_TerrainPtr terrain) {
				::ImGui::Image((void*)terrain->GetTexture().GetTexture(),
				{
					256,
					256
				},
				{ 0,1 }, { 1,0 });
			});
		::ImGui::End();
	}
}


//=================================================================================
void C_TerrainEntity::WholeTerrain(std::function<void(T_TerrainPtr)> lambda)
{
	std::for_each(m_Patches.begin(), m_Patches.end(), lambda);
}

}}