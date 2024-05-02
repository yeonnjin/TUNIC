#include "stdafx.h"
#include "Editor.h"

#include "Test_Object.h"
#include "Map_Object.h"
#include "ImGuizmo.h"

#include "../../EngineSDK/ImGui/ImGuizmo.h"

#include <locale>
#include <codecvt>

#include <fstream>
//#include "Texture.h"

#include "Model.h"
#include "Player.h"
#include "Monster_Spinner.h"
#include "Camera_Free.h"
#include "Camera_Telescope.h"

#define DATAPATH "../Bin/Resources/Data/Map/Map_12_1.dat"
#define MODELPATH "../Bin/Resources/Data/Model/Player.dat"
#pragma region Initial

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

	m_pPlayer = (CPlayer*)m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player"), 0);
	//m_pTargetObject = (CMonster_Spinner*)m_pGameInstance->Get_GameObject(
	// , TEXT("Layer_Monster"), 0);

	return S_OK;
}

#pragma endregion

HRESULT CEditor::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);

	m_fTime += fTimeDelta;
	++m_iCount;

	if (1.f <= m_fTime)
	{
		m_iFPS = m_iCount;
		m_iCount = 0;
		m_fTime = 0.f;
	}

	return S_OK;
}

void CEditor::Late_Tick(_float fTimeDelta)
{
	m_pGameInstance->New_Frame();

	m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_UI, this);

	// To Do
	Frame_Tab();

	ImGui::EndFrame();
}

HRESULT CEditor::Render()
{	
	m_pGameInstance->Render();

	return S_OK;
}

void CEditor::Frame_Tab()
{
	ImGui::Begin("FRAME");

	if (ImGui::BeginTabBar("MyTabBar")) 
	{
		if (ImGui::BeginTabItem("[PICKING]")) 
		{
			Tool_Picking();
			ImGui::EndTabItem();
		}

		if (ImGui::BeginTabItem("[PLAYER]"))
		{
			Tool_PlayerInfo();
			Tool_CameraInfo();
			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();	
}

void CEditor::Gizmo(CTransform* pTransform)
{
	ImGui::Text("FPS : %d", m_iFPS);

	ImGui::Separator();

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

	m_pGameInstance->EditTransform(pTransform);

	ImGui::Separator();

	

	/*CPlayer::DIR eDir = m_pTargetObject->Get_Dir();
	ImGui::Text("Dir : %d", (_uint)eDir);*/
}
#pragma endregion

void CEditor::Tool_Picking()
{
	m_pGizmoTransform = (CTransform*)m_pGameInstance->Get_Component(LEVEL_STATIC, TEXT("Layer_Player"), g_strTransformTag);
	//Safe_AddRef(m_pGizmoTransform);
	Gizmo(m_pGizmoTransform);
	
}

void CEditor::Tool_PlayerInfo()
{
	// STATE
	ImGui::Text("STATE : %d", m_pPlayer->Get_State());

	// DAMAGE
	ImGui::Text("DAMAGE :");
	ImGui::SameLine();
	ImGui::Text(m_pPlayer->Get_isDamage() ? "DAMAGE" : "NONE");

}

void CEditor::Tool_CameraInfo()
{
	ImGui::Separator();

	_float4 vCamPosition = m_pGameInstance->Get_CamPosition_Float4();

	ImGui::Separator();
	CCamera_Free* pCamera = dynamic_cast<CCamera_Free*>(m_pGameInstance->Get_Camera(TEXT("Camera_Free")));
	CTransform* pTransform = dynamic_cast<CTransform*>(pCamera->Get_Component(g_strTransformTag));
	_float4 vLook = pTransform->Get_State_Float4(CTransform::STATE_LOOK);

	ImGui::Text("POSITION");
	ImGui::Text("X : %f", vCamPosition.x);
	ImGui::Text("Y : %f", vCamPosition.y);
	ImGui::Text("Z : %f", vCamPosition.z);

	ImGui::Separator();

	CCamera_Telescope* pTelescopeCamera = dynamic_cast<CCamera_Telescope*>(m_pGameInstance->Get_Camera(TEXT("Camera_Telescope")));
	CTransform* pTelescopeTransform = dynamic_cast<CTransform*>(pTelescopeCamera->Get_Component(g_strTransformTag));
	_float4 vTelescopeLook = pTelescopeTransform->Get_State_Float4(CTransform::STATE_LOOK);

	ImGui::Text("FREE LOOK");
	ImGui::Text("LOOK_X : %f", vLook.x);
	ImGui::Text("LOOK_Y : %f", vLook.y);
	ImGui::Text("LOOK_Z : %f", vLook.z);

	ImGui::Separator();

	ImGui::Text("TELESCOPE LOOK");
	ImGui::Text("Telescope - LOOK_X : %f", vTelescopeLook.x);
	ImGui::Text("Telescope - LOOK_Y : %f", vTelescopeLook.y);
	ImGui::Text("Telescope - LOOK_Z : %f", vTelescopeLook.z);
}

// ============================================================================================
// ETC ========================================================================================

#pragma region ETC
string CEditor::WStringToString(wstring& wstr)
{
	wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
	return myconv.to_bytes(wstr);
}
#pragma endregion

// ============================================================================================
// CREATE =====================================================================================

#pragma region CREATE
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

	/*if(nullptr != m_pGizmoTransform)
		Safe_Release(m_pGizmoTransform);*/

}
#pragma endregion

// ============================================================================================