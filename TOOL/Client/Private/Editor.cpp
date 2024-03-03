#include "stdafx.h"
#include "Editor.h"

#include "Test_Object.h"
#include "ImGuizmo.h"

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

	return S_OK;
}

void CEditor::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CEditor::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->New_Frame();

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);

	// To Do
	//Test();
	//Gizmo();

	if (!m_isUsingGizmo && m_pGameInstance->Get_DIMouseState(DIMKS_LBUTTON, KEY_DOWN))
		Test_Picking();

	if (!m_isUsingGizmo && m_pGameInstance->Get_DIMouseState(DIMKS_RBUTTON, KEY_DOWN))
		Test_Mesh_Picking();

	if (m_pGameInstance->Get_DIKeyState(DIK_G, KEY_DOWN))
		m_isUsingGizmo = !m_isUsingGizmo;

	if (m_isUsingGizmo)
		Gizmo(m_pGizmoTransform);
}

HRESULT CEditor::Render()
{	
	m_pGameInstance->Render();

	return S_OK;
}

HRESULT CEditor::Test_Picking()
{
	const CVIBuffer_Terrain* pTerrainBuffer = dynamic_cast<const CVIBuffer_Terrain*>(m_pGameInstance->Get_Component(LEVEL_TOOL_MAP, TEXT("Layer_Terrain"), TEXT("Com_VIBuffer")));
	if (nullptr == pTerrainBuffer)
		return E_FAIL;

	const CTransform* pTerrainTransform = dynamic_cast<const CTransform*>(m_pGameInstance->Get_Component(LEVEL_TOOL_MAP, TEXT("Layer_Terrain"), TEXT("Com_Transform")));
	if (nullptr == pTerrainTransform)
		return E_FAIL;

	_float3 vPickingPos =  pTerrainBuffer->Compute_Picking(pTerrainTransform);

	if (!(0.f == vPickingPos.x && 0.f == vPickingPos.y && 0.f == vPickingPos.z))
	{
		CTest_Object::TEST_DESC tDesc = {};
		tDesc.vPosition = vPickingPos;

		if (FAILED(m_pGameInstance->Add_Clone(LEVEL_TOOL_MAP, TEXT("Layer_Object"), TEXT("Prototype_GameObject_Test_Object"), &tDesc)))
			return E_FAIL;
	}
}

HRESULT CEditor::Test_Mesh_Picking()
{
	_uint iNumObjects = m_pGameInstance->Get_Object_Count(LEVEL_TOOL_MAP, TEXT("Layer_Object"));
	if (0 == iNumObjects)
		return E_FAIL;

	for (size_t i = 0; i < iNumObjects; ++i)
	{
		const CModel* pObjectModel = dynamic_cast<const CModel*>(m_pGameInstance->Get_Component(LEVEL_TOOL_MAP, TEXT("Layer_Object"), TEXT("Com_Model"), i));
		if (nullptr == pObjectModel)
			return E_FAIL;

		const CTransform* pObjectTransform = dynamic_cast<const CTransform*>(m_pGameInstance->Get_Component(LEVEL_TOOL_MAP, TEXT("Layer_Object"), TEXT("Com_Transform"), i));
		if (nullptr == pObjectTransform)
			return E_FAIL;

		if (true == pObjectModel->Check_Picking(pObjectTransform))
		{
			m_isUsingGizmo = true;
			m_pGizmoTransform = (CTransform*)pObjectTransform;
			return S_OK;
		}			
	}

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

void CEditor::Gizmo(CTransform* pTransform)
{
	ImGui::SetNextWindowPos(ImVec2(1024, 100), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(256, 256), ImGuiCond_Appearing);

	// create a window and insert the inspector
	ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Appearing);
	ImGui::SetNextWindowSize(ImVec2(320, 340), ImGuiCond_Appearing);

	ImGui::Begin("Editor");

	if (ImGuizmo::IsUsing())
	{
		ImGui::Text("Using gizmo");
	}
	else
	{
		ImGui::Text(ImGuizmo::IsOver() ? "Over gizmo" : "");
		ImGui::SameLine();
		ImGui::Text(ImGuizmo::IsOver(ImGuizmo::TRANSLATE) ? "Over translate gizmo" : "");
		ImGui::SameLine();
		ImGui::Text(ImGuizmo::IsOver(ImGuizmo::ROTATE) ? "Over rotate gizmo" : "");
		ImGui::SameLine();
		ImGui::Text(ImGuizmo::IsOver(ImGuizmo::SCALE) ? "Over scale gizmo" : "");
	}
	ImGui::Separator();
	//for (int matId = 0; matId < gizmoCount; matId++)
	//{
	//	ImGuizmo::SetID(matId);

	//CTransform* pTransform = (CTransform*)(m_pGameInstance->Get_Component(LEVEL_TOOL_MAP, TEXT("Layer_Object"), TEXT("Com_Transform")));
	m_pGameInstance->EditTransform(pTransform);

	//	if (ImGuizmo::IsUsing())
	//	{
	//		lastUsing = matId;
	//	}
	//}

	ImGui::End();
	ImGui::EndFrame();
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
		MSG_BOX(TEXT("Failed To Clone : CEditor"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEditor::Free()
{
	__super::Free();
}
