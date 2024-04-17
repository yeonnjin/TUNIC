#include "stdafx.h"
#include "Object_Chest.h"

CObject_Chest::CObject_Chest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject{ pDevice, pContext }
{
}

CObject_Chest::CObject_Chest(const CObject_Chest& rhs)
    : CGameObject{ rhs }
{
}

void CObject_Chest::Change_State(STATE eState)
{
    m_pModelCom->Change_State(eState);
    m_eState = eState;
}

HRESULT CObject_Chest::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CObject_Chest::Initialize(void* pArg)
{
    GAMEOBJECT_DESC		GameObjectDesc{};

    GameObjectDesc.fSpeedPerSec = 8.f;
    GameObjectDesc.fRotationPerSec = XMConvertToRadians(90.0f);

    if (FAILED(__super::Initialize(&GameObjectDesc)))
        return E_FAIL;

    if (nullptr != pArg)
    {
        CHEST_DESC* pDesc = (CHEST_DESC*)pArg;
        //m_strModelComTag = pDesc->strModelComTag;
    }

    if (FAILED(Add_Components()))
        return E_FAIL;

    if (FAILED(Add_States()))
        return E_FAIL;

    //m_eType = OBJ_OBJECT;

    _float4 vPosition = _float4(0.f, 0.2f, 0.f, 1.f);
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

    m_pModelCom->Set_Animation_Index(ANIM_OPEN);
    m_pModelCom->Set_Animation_Transform(m_pTransformCom);
    Set_Animation();

    return S_OK;
}

HRESULT CObject_Chest::Tick(_float fTimeDelta)
{
    if (E_FAIL == __super::Tick(fTimeDelta))
        return E_FAIL;

    static _uint iIndex = 0;
    if (m_pGameInstance->Get_DIKeyState(DIK_I, KEY_DOWN))
    {
        iIndex++;
        if (iIndex > 1)
            iIndex = 0;
        m_pModelCom->Set_Animation_Index(iIndex);
    }

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

    // Collider
    m_pColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

    m_pGameInstance->Add_Group(CCollision_Manager::GROUP_PLAYER, this);

    return S_OK;
}

void CObject_Chest::Late_Tick(_float fTimeDelta)
{
    m_pGameInstance->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);

#ifdef _DEBUG
    m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CObject_Chest::Render()
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

void CObject_Chest::Update_State()
{
}

HRESULT CObject_Chest::Add_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Model */
    _char szModelTag[MAX_PATH] = "Prototype_Component_Model_Object_Chest";
    wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, wstr,
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;

    /* For. Com_Collider */
    CBounding_SPHERE::BOUNDING_SPHERE_DESC ColliderDesc{};

    ColliderDesc.fRadius = 0.8f;
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.fRadius + 0.6f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Collider_SPHERE"),
        TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
        return E_FAIL;
}

HRESULT CObject_Chest::Add_States()
{
    //m_pModelCom->Add_State(STATE_IDLE, CPlayer_State_Idle::Create(this));
    //m_pModelCom->Add_State(STATE_SLEEP, CPlayer_State_Sleep::Create(this));

    //m_pModelCom->Change_State(STATE_IDLE);
    //m_eState = STATE_IDLE;

    return S_OK;
}

HRESULT CObject_Chest::Bind_ShaderResources()
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

void CObject_Chest::Set_Animation()
{
}

CObject_Chest* CObject_Chest::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CObject_Chest* pInstance = new CObject_Chest(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CObject_Chest"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CObject_Chest::Clone(void* pArg)
{
    CObject_Chest* pInstance = new CObject_Chest(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CObject_Chest"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CObject_Chest::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pModelCom);
    Safe_Release(m_pColliderCom);
}

void CObject_Chest::Collision_Event(Engine::CGameObject* pGameObject)
{
}
