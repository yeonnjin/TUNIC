#include "stdafx.h"
#include "Monster.h"

#include "Object_Gem.h"
#include "UI_Aggro.h"

#include "Player.h"

CMonster::CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CMonster::CMonster(const CMonster& rhs)
    : CGameObject{ rhs }
{
}

HRESULT CMonster::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CMonster::Initialize(void* pArg)
{
    GAMEOBJECT_DESC		GameObjectDesc{};

    GameObjectDesc.fSpeedPerSec = 3.f;
    GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(__super::Initialize(&GameObjectDesc)))
        return E_FAIL;

    if (nullptr != pArg)
    {
        MONSTER_DESC* pDesc = (MONSTER_DESC*)pArg;
        m_strModelComTag = pDesc->strModelComTag;
        m_eLevel = pDesc->eLevel;
    }

    if (FAILED(Add_Components()))
        return E_FAIL;

    m_eType = OBJ_MONSTER;
    m_eRigid = RIGID_BLOCK;

    CUI_Aggro::UI_AGGRO_DESC tDesc{};
    tDesc.pTargetTransform = m_pTransformCom;
    tDesc.fHeight = m_fUIHeight;
    m_pUIAggro = dynamic_cast<CUI_Aggro*>(m_pGameInstance->Get_GameObject_Clone(TEXT("Prototype_GameObject_UI_Aggro"), &tDesc));
    if (nullptr == m_pUIAggro)
        return E_FAIL;

    return S_OK;
}

HRESULT CMonster::Tick(_float fTimeDelta)
{
    if(FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    // State_Machine
    m_pModelCom->Update_State(fTimeDelta);
    Update_State();

    // Blending
    if (true == m_isBlend)
    {
        if (S_OK == m_pModelCom->Blending_Animation(m_eBlendAnimIndex, fTimeDelta))
        {
            m_isBlend = false;
            m_pModelCom->Set_Animation_Index(m_eBlendAnimIndex);
            m_eAnimationIndex = m_eBlendAnimIndex;

        }
    }
    else
        m_pModelCom->Play_Animation(fTimeDelta);

    // Navigation
    if (false == m_pNavigationCom->isMove(m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION)))
    {
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPrePosition);
    }
    m_vPrePosition = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION);

    // Height
    _float fHeight = m_pNavigationCom->Compute_Height(m_vPrePosition);
    m_vPrePosition.m128_f32[1] = fHeight;
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, m_vPrePosition);

    // Collider
    m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
    m_pRigidColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());
    m_pGameInstance->Add_Group(CCollision_Manager::GROUP_MONSTER, this);
    m_pGameInstance->Add_RigidGroup(this);

    // UI_Aggro
    m_pUIAggro->Tick(fTimeDelta);

    return S_OK;
}

void CMonster::Late_Tick(_float fTimeDelta)
{
    Compute_Damage_CoolTime(fTimeDelta);

    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);

    m_pUIAggro->Late_Tick(fTimeDelta);

#ifdef _DEBUG
        m_pGameInstance->Add_DebugComponent(m_pColliderCom);
        m_pGameInstance->Add_DebugComponent(m_pRigidColliderCom); 
#endif
}

HRESULT CMonster::Render()
{
    if (FAILED(Bind_ShaderResources()))
        return E_FAIL;

    _uint iNumMeshes = m_pModelCom->Get_NumMeshes();

    for (size_t i = 0; i < iNumMeshes; i++)
    {
        if (FAILED(m_pModelCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture", i, TEX_DIFFUSE)))
            return E_FAIL;

        if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
            return E_FAIL;

        /* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이터를 다 던져야한다. */
        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }

    return S_OK;
}

HRESULT CMonster::Render_LightDepth()
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

HRESULT CMonster::Add_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(m_eLevel, m_strModelComTag,
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;

    /* For. Com_RigidCollider */
    CBounding_OBB::BOUNDING_OBB_DESC		RigidDesc{};

    // 로컬상의 정보를 셋팅한다.
    RigidDesc.vSize = _float3(2.f, 2.f, 2.f);
    RigidDesc.vCenter = _float3(0.f, RigidDesc.vSize.y * 0.5f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_RigidCollider"), (CComponent**)&m_pRigidColliderCom, &RigidDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CMonster::Bind_ShaderResources()
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

void CMonster::Set_isAggro(_bool isAggro)
{
    m_pUIAggro->Set_Using(isAggro);
}

void CMonster::Clone_Gem()
{
    for (size_t i = 0; i < m_iGemCount; i++)
    {
        CObject_Gem::OBJECT_GEM_DESC tDesc{};
        tDesc.vPosition = m_vPrePosition;
        tDesc.iGemCount = m_iGemCount;
        tDesc.iIndex = i;
        tDesc.iNavigationIndex = m_pNavigationCom->Get_CurrentIndex();

        if(FAILED(m_pGameInstance->Add_Clone(LEVEL_BEACH, TEXT("Layer_Object_Gem"), TEXT("Prototype_GameObject_Object_Gem"), &tDesc)))
            return;
    }
}

void CMonster::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pColliderCom);
    Safe_Release(m_pRigidColliderCom);
    Safe_Release(m_pNavigationCom);
    Safe_Release(m_pUIAggro);
}

void CMonster::Collision_Event(Engine::CGameObject* pGameObject)
{

}

