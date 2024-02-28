#include "stdafx.h"
#include "Editor.h"

#include "ImGui_Manager.h"

CEditor::CEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject{ pDevice, pContext }
{
}

CEditor::CEditor(const CEditor& rhs)
	: CGameObject{ rhs }
{
}

HRESULT CEditor::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEditor::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pImGui_Manager = CImGui_Manager::Get_Instance();

	return S_OK;
}

void CEditor::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CEditor::Late_Tick(_float fTimeDelta)
{
	m_pImGui_Manager->New_Frame();
	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);

	// To Do
	Test();	
}

HRESULT CEditor::Render()
{	
	m_pImGui_Manager->Render();

	return S_OK;
}

void CEditor::Test()
{
	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::End();
	}

	// 3. Show another simple window.
	if (show_another_window)
	{
		static float fCharacterX = 0.0f;
		static float fCharacterY = 0.0f;
		static float fCharacterZ = 0.0f;

		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");

		ImGui::SliderFloat("float", &fCharacterX, 0.0f, 300.0f);
		ImGui::SliderFloat("float", &fCharacterY, 0.0f, 300.0f);
		ImGui::SliderFloat("float", &fCharacterZ, 0.0f, 1.0f);

		if (ImGui::Button("Save Me"))
			// File Save
			if (ImGui::Button("Close Me"))
				show_another_window = false;
		ImGui::End();
	}
}

CEditor* CEditor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEditor* pInstance = new CEditor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Create : CEditor"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CEditor::Clone(void* pArg)
{
	CEditor* pInstance = new CEditor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Create : CEditor"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEditor::Free()
{
	__super::Free();

	Safe_Release(m_pImGui_Manager);
}
