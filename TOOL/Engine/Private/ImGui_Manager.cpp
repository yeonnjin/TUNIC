#include "ImGui_Manager.h"

#include "GameInstance.h"
#include "Camera.h"

#include "ImGuizmo.h"

CImGui_Manager::CImGui_Manager()
    : m_pGameInstance(CGameInstance::Get_Instance())
{
    Safe_AddRef(m_pGameInstance);
}

HRESULT CImGui_Manager::Initialize(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    m_io = io;
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

void CImGui_Manager::Set_Perspective(_bool isPerspective, _float fFov, _float fViewWidth)
{
    if (isPerspective)
    {
        Perspective(fFov, m_io.DisplaySize.x / m_io.DisplaySize.y, 0.1f, 100.f, m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ).m[0]);
    }
    else
    {
        float viewHeight = fViewWidth * m_io.DisplaySize.y / m_io.DisplaySize.x;
        OrthoGraphic(-fViewWidth, fViewWidth, -viewHeight, viewHeight, 1000.f, -1000.f, m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ).m[0]);
}
    ImGuizmo::SetOrthographic(!isPerspective);
}

void CImGui_Manager::EditTransform(CTransform* pTransformCom)
{
#ifndef _DEBUG
    return;
#endif
    ImGuizmo::BeginFrame();

    _float4x4 matrix = pTransformCom->Get_WorldFloat4x4();

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
    ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
    ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);
    ImGuizmo::Manipulate(ViewMatrix.m[0], ProjMatrix.m[0], mCurrentGizmoOperation, mCurrentGizmoMode, matrix.m[0], NULL, useSnap ? &snap.x : NULL);
    pTransformCom->Set_WorldMatrix(matrix);

    // Grid
    _float4x4 identityMatrix;
    XMStoreFloat4x4(&identityMatrix, XMMatrixIdentity());
    ImGuizmo::DrawGrid(ViewMatrix.m[0], ProjMatrix.m[0], identityMatrix.m[0], 100.f);
}

void CImGui_Manager::Perspective(float fovyInDegrees, float aspectRatio, float znear, float zfar, float* m16)
{
    _float ymax, xmax;
    ymax = znear * tanf(fovyInDegrees * 3.141592f / 180.0f);
    xmax = ymax * aspectRatio;
    Frustum(-xmax, xmax, -ymax, ymax, znear, zfar, m16);
}

void CImGui_Manager::Frustum(float left, float right, float bottom, float top, float znear, float zfar, float* m16)
{
    _float temp, temp2, temp3, temp4;
    temp = 2.0f * znear;
    temp2 = right - left;
    temp3 = top - bottom;
    temp4 = zfar - znear;
    m16[0] = temp / temp2;
    m16[1] = 0.0;
    m16[2] = 0.0;
    m16[3] = 0.0;
    m16[4] = 0.0;
    m16[5] = temp / temp3;
    m16[6] = 0.0;
    m16[7] = 0.0;
    m16[8] = (right + left) / temp2;
    m16[9] = (top + bottom) / temp3;
    m16[10] = (-zfar - znear) / temp4;
    m16[11] = -1.0f;
    m16[12] = 0.0;
    m16[13] = 0.0;
    m16[14] = (-temp * zfar) / temp4;
    m16[15] = 0.0;
}

void CImGui_Manager::OrthoGraphic(const float l, float r, float b, const float t, float zn, const float zf, float* m16)
{
    m16[0] = 2 / (r - l);
    m16[1] = 0.0f;
    m16[2] = 0.0f;
    m16[3] = 0.0f;
    m16[4] = 0.0f;
    m16[5] = 2 / (t - b);
    m16[6] = 0.0f;
    m16[7] = 0.0f;
    m16[8] = 0.0f;
    m16[9] = 0.0f;
    m16[10] = 1.0f / (zf - zn);
    m16[11] = 0.0f;
    m16[12] = (l + r) / (l - r);
    m16[13] = (t + b) / (b - t);
    m16[14] = zn / (zn - zf);
    m16[15] = 1.0f;
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

    Safe_Release(m_pGameInstance);
}
