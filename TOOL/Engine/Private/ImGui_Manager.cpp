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
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(pDevice, pContext);

    m_pDevice = pDevice;
    m_pContext = pContext;

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

void CImGui_Manager::EditTransform(CTransform* pTransformCom)
{
#ifndef _DEBUG
    return;
#endif
    ImGuizmo::BeginFrame();

    _float4x4 matrix = pTransformCom->Get_WorldFloat4x4();

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
    ImGui::Checkbox("##UseSnap", &useSnap);
    ImGui::SameLine();

    _float snap[3] = { 1.f, 1.f, 1.f };
    switch (mCurrentGizmoOperation)
    {
    case ImGuizmo::TRANSLATE:
        ImGui::InputFloat3("Snap", &snap[0]);
        break;
    case ImGuizmo::ROTATE:
        ImGui::InputFloat("Angle Snap", &snap[0]);
        break;
    case ImGuizmo::SCALE:
        ImGui::InputFloat("Scale Snap", &snap[0]);
        break;
    }

    ImGuiIO& io = ImGui::GetIO();
    ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

    _float4x4 ViewMatrix, ProjMatrix;
    ViewMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
    ProjMatrix = m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);
    ImGuizmo::Manipulate(ViewMatrix.m[0], ProjMatrix.m[0], mCurrentGizmoOperation, mCurrentGizmoMode, matrix.m[0], NULL, useSnap ? &snap[0] : NULL);
    pTransformCom->Set_WorldMatrix(matrix);

    // Grid
    _float4x4 identityMatrix;
    XMStoreFloat4x4(&identityMatrix, XMMatrixIdentity());
    ImGuizmo::DrawGrid(ViewMatrix.m[0], ProjMatrix.m[0], identityMatrix.m[0], 100.f);
}

HRESULT CImGui_Manager::Create_Prototype_Model(CModel::TYPE eType, const wstring& strFolderPath)
{
    WIN32_FIND_DATA findFileData;
    _tchar      szFolderPath[MAX_PATH] = TEXT("");
    wsprintf(szFolderPath, strFolderPath.c_str());
    lstrcat(szFolderPath, L"*.fbx");
    HANDLE hFile = FindFirstFile(szFolderPath, &findFileData);
    if (hFile == INVALID_HANDLE_VALUE)
        return E_FAIL;

    while (true)
    {
        _tchar   szFullPath[MAX_PATH] = TEXT("");
        lstrcat(szFullPath, strFolderPath.c_str());
        lstrcat(szFullPath, findFileData.cFileName);

        // Your wchar_t*
        wstring ws(szFullPath);
        // your new String
        string str(ws.begin(), ws.end());

        HRESULT      hr = {};

        if (CModel::TYPE_NONANIM == eType)
        {
            if (FAILED(m_pGameInstance->Add_Prototype(4, szFullPath,
                CModel::Create(m_pDevice, m_pContext, CModel::TYPE_NONANIM, str))))
                return E_FAIL;
        }
        else
        {
            if (FAILED(m_pGameInstance->Add_Prototype(4, szFullPath,
                CModel::Create(m_pDevice, m_pContext, CModel::TYPE_ANIM, str))))
                return E_FAIL;
        }

        if (!FindNextFileW(hFile, &findFileData))
            break;
    }
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
