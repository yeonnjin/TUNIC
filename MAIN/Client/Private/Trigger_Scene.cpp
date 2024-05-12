#include "stdafx.h"
#include "Trigger_Scene.h"

#include "Player.h"

CTrigger_Scene::CTrigger_Scene(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CInteractiveObject{ pDevice, pContext }
{
}

CTrigger_Scene::CTrigger_Scene(const CTrigger_Scene& rhs)
    : CInteractiveObject{ rhs }
{
}

HRESULT CTrigger_Scene::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTrigger_Scene::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    _vector vPosition = { 0.f, 0.f, 7.6f, 1.f };
    m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

    m_eRigid = RIGID_NONBLOCK;
    m_eInteractiveType = INTERACTIVE_TRIGGER_SCENE;

    Compute_ColliderMatrix();

    return S_OK;
}

HRESULT CTrigger_Scene::Tick(_float fTimeDelta)
{
    if (E_FAIL == __super::Tick(fTimeDelta))
        return E_FAIL;

    m_pColliderCom->Tick(m_ColliderMatrix);

    m_pGameInstance->Add_Group(CCollision_Manager::GROUP_INTERACTIVE, this);

    return S_OK;
}

void CTrigger_Scene::Late_Tick(_float fTimeDelta)
{
#ifdef _DEBUG
    m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CTrigger_Scene::Render()
{
    return S_OK;
}

HRESULT CTrigger_Scene::Add_Components()
{
    /* Com_Collider */
    CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

    // 로컬상의 정보를 셋팅한다.
    ColliderDesc.vSize = { 2.f, 2.f, 2.f };
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CTrigger_Scene::Bind_ShaderResources()
{
    return S_OK;
}

void CTrigger_Scene::Compute_ColliderMatrix()
{
    _matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
    m_ColliderMatrix = WorldMatrix;
}

CTrigger_Scene* CTrigger_Scene::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTrigger_Scene* pInstance = new CTrigger_Scene(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CTrigger_Scene"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTrigger_Scene::Clone(void* pArg)
{
    CTrigger_Scene* pInstance = new CTrigger_Scene(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CTrigger_Scene"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTrigger_Scene::Free()
{
    __super::Free();
}

void CTrigger_Scene::Collision_Event(Engine::CGameObject* pGameObject)
{
    if (false == m_isTrigger)
    {
        m_pGameInstance->Change_Camera(TEXT("Camera_Scene"));

        m_isTrigger = true;

        CPlayer* pPlayer = dynamic_cast<CPlayer*>(m_pGameInstance->Get_GameObject(LEVEL_STATIC, TEXT("Layer_Player")));
        pPlayer->Set_Scene(true);
    }
}
