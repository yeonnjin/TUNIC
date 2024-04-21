#include "stdafx.h"
#include "InteractiveObject.h"

CInteractiveObject::CInteractiveObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CInteractiveObject::CInteractiveObject(const CInteractiveObject& rhs)
    : CGameObject{ rhs }
{
}

HRESULT CInteractiveObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CInteractiveObject::Initialize(void* pArg)
{
    GAMEOBJECT_DESC		GameObjectDesc{};

    GameObjectDesc.fSpeedPerSec = 8.f;
    GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(__super::Initialize(&GameObjectDesc)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    m_eType = OBJ_INTERACTIVE;

    return S_OK;
}

HRESULT CInteractiveObject::Tick(_float fTimeDelta)
{
    if (E_FAIL == __super::Tick(fTimeDelta))
        return E_FAIL;


    return S_OK;
}

void CInteractiveObject::Late_Tick(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG
    m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CInteractiveObject::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; ++i)
    {
        if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, TEX_DIFFUSE)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CInteractiveObject::Add_Components()
{
    return S_OK;
}

HRESULT CInteractiveObject::Bind_ShaderResources()
{
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
        return E_FAIL;
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
        return E_FAIL;

    _float fCamFar = m_pGameInstance->Get_Camera_Far();
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float))))
        return E_FAIL;

    return S_OK;
}

void CInteractiveObject::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pColliderCom);
}

void CInteractiveObject::Collision_Event(Engine::CGameObject* pGameObject)
{
}
