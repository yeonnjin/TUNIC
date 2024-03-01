#include "ImGui_Manager.h"

#include "GameInstance.h"
#include "Camera.h"

#include "ImGuizmo.h"

CImGui_Manager::CImGui_Manager()
{
}

HRESULT CImGui_Manager::Initialize(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(pDevice, pContext);

	return S_OK;
}

void CImGui_Manager::New_Frame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

HRESULT CImGui_Manager::Render()
{	
	// ImGui를 사용한 렌더링 코드를 여기에 작성하세요.

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return S_OK;
}

void CImGui_Manager::EditTransform(/*const CCamera& camera,*/ _float4x4& matrix)
{
#ifndef _DEBUG
    return;
#endif

    ImGui::Separator();
    ImGui::NewLine();

    static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
    static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);

    if (CGameInstance::Get_Instance()->Get_DIKeyState(DIK_Q, KEY_DOWN))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
    if (CGameInstance::Get_Instance()->Get_DIKeyState(DIK_E, KEY_DOWN))
        mCurrentGizmoOperation = ImGuizmo::ROTATE;
    if (CGameInstance::Get_Instance()->Get_DIKeyState(DIK_R, KEY_DOWN))
        mCurrentGizmoOperation = ImGuizmo::SCALE;
    if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
        mCurrentGizmoOperation = ImGuizmo::TRANSLATE;

    ImGui::SameLine();

    if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
        mCurrentGizmoOperation = ImGuizmo::ROTATE;

    ImGui::SameLine();

    if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
        mCurrentGizmoOperation = ImGuizmo::SCALE;

    float matrixTranslation[3], matrixRotation[3], matrixScale[3];
    ImGuizmo::DecomposeMatrixToComponents(matrix.m[0], matrixTranslation, matrixRotation, matrixScale);
    ImGui::InputFloat3("Tr", matrixTranslation);
    ImGui::InputFloat3("Rt", matrixRotation);
    ImGui::InputFloat3("Sc", matrixScale);
    ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix.m[0]);

    if (mCurrentGizmoOperation != ImGuizmo::SCALE)
    {
        if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
            mCurrentGizmoMode = ImGuizmo::LOCAL;

        ImGui::SameLine();

        if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
            mCurrentGizmoMode = ImGuizmo::WORLD;
    }

    static bool useSnap(false);
    ImGui::Checkbox("useSnap", &useSnap);

    ImGui::SameLine();

    _float3 snap;
    switch (mCurrentGizmoOperation)
    {
    case ImGuizmo::TRANSLATE:
        ImGui::InputFloat3("Snap", &snap.x);
        break;
    case ImGuizmo::ROTATE:
        ImGui::InputFloat("Angle Snap", &snap.x);
        break;
    case ImGuizmo::SCALE:
        ImGui::InputFloat("Scale Snap", &snap.x);
        break;
    }

    ImGuiIO& io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

    _float4x4 ViewMatrix, ProjMatrix;
    ViewMatrix = CGameInstance::Get_Instance()->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
    ProjMatrix = CGameInstance::Get_Instance()->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);
    ImGuizmo::Manipulate(ViewMatrix.m[0], ProjMatrix.m[0], mCurrentGizmoOperation, mCurrentGizmoMode, matrix.m[0], NULL, useSnap ? &snap.x : NULL);
}

CImGui_Manager* CImGui_Manager::Create(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CImGui_Manager* pInstance = new CImGui_Manager();

    if (FAILED(pInstance->Initialize(hWnd, pDevice, pContext)))
    {
        MSG_BOX(TEXT("Failed To Create : CObject_Manager"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CImGui_Manager::Free()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
