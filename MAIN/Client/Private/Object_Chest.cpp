#include "stdafx.h"
#include "Object_Chest.h"

#include "UI_Interactive.h"

CObject_Chest::CObject_Chest(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CInteractiveObject{ pDevice, pContext }
{
}

CObject_Chest::CObject_Chest(const CObject_Chest& rhs)
    : CInteractiveObject{ rhs }
{
}

HRESULT CObject_Chest::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CObject_Chest::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    if (nullptr != pArg)
    {
        CHEST_DESC* pDesc = (CHEST_DESC*)pArg;
        //m_pTransformCom->Set_WorldMatrix(pDesc->TransformMatrix);
        m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPosition);
        m_pItem = dynamic_cast<CItem*>(m_pGameInstance->Get_GameObject_Clone(TEXT("Prototype_GameObject_Object_Item")));
        if (nullptr == m_pItem)
            return E_FAIL;

        m_pItem->Set_ItemType(pDesc->eType);
        m_pItem->Set_Item(pDesc->eItem);
        m_isRotation = pDesc->isRotation;
        m_fAngle = pDesc->fAngle;
    }
    
    //_float4 vPosition = _float4(0.f, 0.2f, 0.f, 1.f);
    //m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

    m_pModelCom->Set_Animation_Index(ANIM_OPEN);
    m_pModelCom->Set_Animation_Transform(m_pTransformCom);

    m_eInteractiveType = INTERACTIVE_CHEST;
    m_eRigid = RIGID_BLOCK;

    if (true == m_isRotation)
        m_pTransformCom->Rotation(_vector{ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(m_fAngle));

    Compute_ColliderMatrix();

    // TODO: Test용
    /*CItem* pItem = dynamic_cast<CItem*>(m_pGameInstance->Get_GameObject_Clone(TEXT("Prototype_GameObject_Object_Item")));
    pItem->Set_ItemType((CItem::ITEM_TYPE)2);
    pItem->Set_Item((CItem::ITEM)2);

    m_pItem = pItem;*/

    return S_OK;
}

HRESULT CObject_Chest::Tick(_float fTimeDelta)
{
    if (E_FAIL == __super::Tick(fTimeDelta))
        return E_FAIL;

    if (true == m_isFirstFrame)
    {
        m_pModelCom->Play_Animation(fTimeDelta);
        m_isFirstFrame = false;
    }

    if(false == m_isClose && false == m_isFinished)
    {
        m_pModelCom->Play_Animation(fTimeDelta);

        if (true == m_pModelCom->isFinished(ANIM_OPEN))
        {
            m_isFinished = true;
        }
    }

    // Collider
    m_pColliderCom->Tick(m_ColliderMatrix);

    m_pRigidColliderCom->Tick(m_pTransformCom->Get_WorldMatrix());

    m_pGameInstance->Add_Group(CCollision_Manager::GROUP_INTERACTIVE, this);

    m_pGameInstance->Add_RigidGroup(this);

    return S_OK;
}

void CObject_Chest::Late_Tick(_float fTimeDelta)
{
    __super::Late_Tick(fTimeDelta);

#ifdef _DEBUG
    //m_pGameInstance->Add_DebugComponent(m_pRigidColliderCom);
#endif
}

HRESULT CObject_Chest::Render()
{
    if (FAILED(__super::Render()))
        return E_FAIL;

    return S_OK;
}

HRESULT CObject_Chest::Add_Components()
{
    /* For.Com_Shader */
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
        TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
        return E_FAIL;

    /* For.Com_Model */
    _char szModelTag[MAX_PATH] = "Prototype_Component_Model_Object_Chest";
    wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
    if (FAILED(__super::Add_Component(LEVEL_BEACH, wstr,
        TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
        return E_FAIL;

    /* Com_Collider */ // TODO: 콜라이더를 트랜스폼 위치 앞에다가 둬서 트리거 발동~
    CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

    // 로컬상의 정보를 셋팅한다.
    ColliderDesc.vSize = _float3(2.f, 2.f, 2.f);
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);

    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
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

HRESULT CObject_Chest::Bind_ShaderResources()
{
    if (FAILED(__super::Bind_ShaderResources()))
        return E_FAIL;

    return S_OK;
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

    Safe_Release(m_pItem);
    Safe_Release(m_pRigidColliderCom);
}

void CObject_Chest::Collision_Event(Engine::CGameObject* pGameObject)
{
    __super::Collision_Event(pGameObject);

    if (false == m_isClose)
        m_pUIInteractive->Set_Using(false);
}
