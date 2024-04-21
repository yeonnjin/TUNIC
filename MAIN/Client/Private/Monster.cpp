#include "stdafx.h"
#include "Monster.h"

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
    }

    if (FAILED(Add_Components()))
        return E_FAIL;

    m_eType = OBJ_MONSTER;

    return S_OK;
}

HRESULT CMonster::Tick(_float fTimeDelta)
{
    if(FAILED(__super::Tick(fTimeDelta)))
        return E_FAIL;

    /*static _uint iIndex = 0;
    if (m_pGameInstance->Get_DIKeyState(DIK_I, KEY_DOWN))
    {
        iIndex++;
        if (iIndex > 6)
            iIndex = 0;
        m_pModelCom->Set_Animation_Index(iIndex);
    }*/

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

    m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

    m_pGameInstance->Add_Group(CCollision_Manager::GROUP_MONSTER, this);

    return S_OK;
}

void CMonster::Late_Tick(_float fTimeDelta)
{
    Compute_Damage_CoolTime(fTimeDelta);
    //Compute_Collision_CoolTime(fTimeDelta);
  

    if (true == m_pGameInstance->isInFrustum_WorldSpace(m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION), 2.f))
    {
        m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG
        m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
    }
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

        /* 이 함수 내부에서 호출되는 Apply함수 호출 이전에 쉐이더 전역에 던져야할 모든 데이ㅏ터를 다 던져야한다. */
        if (FAILED(m_pShaderCom->Begin(0)))
            return E_FAIL;

        m_pModelCom->Render(i);
    }

//#ifdef _DEBUG
//    m_pColliderCom->Render();
//#endif // _DEBUG

    return S_OK;
}

HRESULT CMonster::Add_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Model */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, m_strModelComTag,
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
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

void CMonster::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pColliderCom);
}

void CMonster::Collision_Event(Engine::CGameObject* pGameObject)
{
    if (OBJ_MONSTER == pGameObject->Get_ObjectType() && true == m_isMove )
    {
        // 몬스터끼리 충돌 시 LOOK 방향 변경
        CMonster* pMonster = dynamic_cast<CMonster*>(pGameObject);
        if (false == pMonster->isCollision())
        {
            _vector vOpposite = m_pTransformCom->Get_State_Vector(CTransform::STATE_POSITION) - dynamic_cast<CTransform*>(pMonster->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION);
            vOpposite = XMVector3Normalize(vOpposite);
            vOpposite.m128_f32[1] = 0.f;
            _vector vOriginLook = XMVector3Normalize(m_pTransformCom->Get_State_Vector(CTransform::STATE_LOOK));
            vOriginLook.m128_f32[1] = 0.f;
            _vector vTargetLook = vOpposite * -0.8f - vOriginLook;
            vTargetLook = XMVector3Normalize(vTargetLook);
            m_pTransformCom->Look_At_Dir(XMVector3Normalize(vTargetLook));
            m_isCollision = true;
        }       
    }
}

