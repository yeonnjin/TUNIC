#include "stdafx.h"
#include "InteractiveObject.h"
#include "UI_Interactive.h"
#include "Player.h"

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

    GameObjectDesc.fSpeedPerSec = (INTERACTIVE_GEM == m_eInteractiveType) ? 7.f : 0.f;
    GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(__super::Initialize(&GameObjectDesc)))
        return E_FAIL;

    if (FAILED(Add_Components()))
        return E_FAIL;

    m_eType = OBJ_INTERACTIVE;

    CUI_Interactive::UI_INTERACTIVE_DESC tDesc{};
    tDesc.pTargetTransform = m_pTransformCom;
    m_pUIInteractive = dynamic_cast<CUI_Interactive*>(m_pGameInstance->Get_GameObject_Clone(TEXT("Prototype_GameObject_UI_Interactive"), &tDesc));
    if (nullptr == m_pUIInteractive)
        return E_FAIL;

    return S_OK;
}

HRESULT CInteractiveObject::Tick(_float fTimeDelta)
{
    if (E_FAIL == __super::Tick(fTimeDelta))
        return E_FAIL;
   
    m_pUIInteractive->Set_Using(false);
    m_pUIInteractive->Tick(fTimeDelta);

    return S_OK;
}

void CInteractiveObject::Late_Tick(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
    m_pUIInteractive->Late_Tick(fTimeDelta);

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

HRESULT CInteractiveObject::Render_LightDepth()
{
    if (nullptr == m_pShaderCom)
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_WorldMatrix", m_pTransformCom->Get_WorldFloat4x4_Ptr())))
        return E_FAIL;

    _float4x4		ViewMatrix, ProjMatrix;

    CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player")));

    _vector vShadowEye = pPlayer->Get_ShadowEye();
    _vector vShadowLookAt = pPlayer->Get_ShadowLookAt();

    XMStoreFloat4x4(&ViewMatrix, XMMatrixLookAtLH(vShadowEye, vShadowLookAt, XMVectorSet(0.f, 1.f, 0.f, 0.f)));
    XMStoreFloat4x4(&ProjMatrix, XMMatrixPerspectiveFovLH(XMConvertToRadians(120.0f), (_float)g_iWinSizeX / g_iWinSizeY, 0.1f, 2000.f));

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &ProjMatrix)))
        return E_FAIL;

    _float fCamFar = m_pGameInstance->Get_Camera_Far();
    if (FAILED(m_pShaderCom->Bind_RawValue("g_fCamFar", &fCamFar, sizeof(_float))))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, TEX_DIFFUSE)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        if (FAILED(m_pShaderCom->Begin(1)))
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

void CInteractiveObject::Compute_ColliderMatrix()
{
    _vector vPosition = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vLook = XMVector3Normalize(m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK));
    vPosition -= vLook;

    _matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
    memcpy(&WorldMatrix.r[3], &vPosition, sizeof(_vector));
    m_ColliderMatrix = WorldMatrix;
}

void CInteractiveObject::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pUIInteractive);
}

void CInteractiveObject::Collision_Event(Engine::CGameObject* pGameObject)
{
    if (OBJ_PLAYER == pGameObject->Get_ObjectType())
    {
        m_pUIInteractive->Set_Using(true, m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION));
    }
}
