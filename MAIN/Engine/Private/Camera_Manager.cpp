#include "Camera_Manager.h"

#include "GameInstance.h"
#include "Camera.h"

CCamera_Manager::CCamera_Manager()
{
}

HRESULT CCamera_Manager::Add_Camera(const wstring& strCameraTag, CCamera* pCamera)
{
    if (nullptr == pCamera)
        return E_FAIL;

    m_Cameras.emplace(strCameraTag, pCamera);

    return S_OK;
}

HRESULT CCamera_Manager::Change_Camera(const wstring& strCameraTag, void* pArg)
{
    CCamera* pNextCamera = Find_Camera(strCameraTag);
    if (nullptr == pNextCamera)
        return E_FAIL;

    if (nullptr != m_pCurCamera)
    {
        pNextCamera->Set_Matrix(m_pCurCamera->Get_Matrix());
        m_pCurCamera->OnExit();
    }

    m_pCurCamera = pNextCamera;

    m_pCurCamera->OnEnter(pArg);

    return S_OK;
}

_float CCamera_Manager::Get_Camera_Far()
{
    if (nullptr == m_pCurCamera)
        return 1000.f;

    return m_pCurCamera->Get_CamFar();
}

CCamera* CCamera_Manager::Get_Camera(const wstring& strCameraTag)
{
    return Find_Camera(strCameraTag);
}

HRESULT CCamera_Manager::Set_Exit(const wstring& strCameraTag, _bool isExit)
{
    CCamera* pExitCamera = Find_Camera(strCameraTag);
    if (nullptr == pExitCamera)
        return E_FAIL;

    pExitCamera->Set_Exit(isExit);

    return S_OK;
}


void CCamera_Manager::Set_Camera_Level(_uint iLevel)
{
    for (auto& pCamera : m_Cameras)
        pCamera.second->Set_Level(iLevel);
}

HRESULT CCamera_Manager::Initialize()
{
    return S_OK;
}

void CCamera_Manager::Tick(_float fTimeDelta)
{
    if(nullptr != m_pCurCamera)
        m_pCurCamera->Tick(fTimeDelta);
}

void CCamera_Manager::Late_Tick(_float fTimeDelta)
{
    if (nullptr != m_pCurCamera)
        m_pCurCamera->Late_Tick(fTimeDelta);
}

CCamera* CCamera_Manager::Find_Camera(const wstring& strCameraTag)
{
    auto iter = m_Cameras.find(strCameraTag);
    if (m_Cameras.end() == iter)
        return nullptr;

    return iter->second;
}

CCamera_Manager* CCamera_Manager::Create()
{
    CCamera_Manager* pInstance = new CCamera_Manager();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed To Create : CCamera_Manager"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCamera_Manager::Free()
{
    __super::Free();

    for (auto& pCamera : m_Cameras)
            Safe_Release(pCamera.second);

    m_Cameras.clear();
}