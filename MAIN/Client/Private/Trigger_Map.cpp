#include "stdafx.h"
#include "Trigger_Map.h"

#include "Level_Beach.h"
#include "Level_Shop.h"
#include "Level_Puzzle.h"
#include "Level_Boss.h"

CTrigger_Map::CTrigger_Map(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CInteractiveObject{ pDevice, pContext }
{
}

CTrigger_Map::CTrigger_Map(const CTrigger_Map& rhs)
    : CInteractiveObject{ rhs }
{
}

HRESULT CTrigger_Map::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CTrigger_Map::Initialize(void* pArg)
{
    TRIGGER_MAP_DESC* pDesc = (TRIGGER_MAP_DESC*)pArg;
    m_vSize = pDesc->vSize;

    if (FAILED(__super::Initialize(pArg)))
        return E_FAIL;

    m_pTransformCom->Set_State(CTransform::STATE_POSITION, pDesc->vPosition);
    if (true == pDesc->isRotation)
        m_pTransformCom->Rotation(_vector{ 0.f, 1.f, 0.f }, XMConvertToRadians(pDesc->fAngle));

    m_eCurLevel = pDesc->eCurLevel;
    m_eNextLevel = pDesc->eNextLevel;

    m_eRigid = RIGID_NONBLOCK;
    m_eInteractiveType = INTERACTIVE_TRIGGER_MAP;

    Compute_ColliderMatrix();

    return S_OK;
}

HRESULT CTrigger_Map::Tick(_float fTimeDelta)
{
    if (E_FAIL == __super::Tick(fTimeDelta))
        return E_FAIL;

    m_pColliderCom->Tick(m_ColliderMatrix);

    m_pGameInstance->Add_Group(CCollision_Manager::GROUP_INTERACTIVE, this);

    return S_OK;
}

void CTrigger_Map::Late_Tick(_float fTimeDelta)
{
#ifdef _DEBUG
    m_pGameInstance->Add_DebugComponent(m_pColliderCom);
#endif
}

HRESULT CTrigger_Map::Render()
{
    return S_OK;
}

HRESULT CTrigger_Map::Add_Components()
{
    /* Com_Collider */
    CBounding_OBB::BOUNDING_OBB_DESC		ColliderDesc{};

    // 로컬상의 정보를 셋팅한다.
    ColliderDesc.vSize = m_vSize;
    ColliderDesc.vCenter = _float3(0.f, ColliderDesc.vSize.y * 0.5f, 0.f);
    
    if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Collider_OBB"),
        TEXT("Com_Collider"), (CComponent**)&m_pColliderCom, &ColliderDesc)))
        return E_FAIL;

    return S_OK;
}

HRESULT CTrigger_Map::Bind_ShaderResources()
{
    return S_OK;
}

void CTrigger_Map::Compute_ColliderMatrix()
{
    _matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
    m_ColliderMatrix = WorldMatrix;
}

CTrigger_Map* CTrigger_Map::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CTrigger_Map* pInstance = new CTrigger_Map(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed To Create : CTrigger_Map"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CTrigger_Map::Clone(void* pArg)
{
    CTrigger_Map* pInstance = new CTrigger_Map(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX(TEXT("Failed To Clone : CTrigger_Map"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CTrigger_Map::Free()
{
    __super::Free();
}

void CTrigger_Map::Collision_Event(Engine::CGameObject* pGameObject)
{
    if(false == m_isNext)
    {
        CLevel* pLevel = m_pGameInstance->Get_Current_Level();

        if (LEVEL_BEACH == m_eCurLevel)
        {
            CLevel_Beach* pCurLevel = dynamic_cast<CLevel_Beach*>(pLevel);
            pCurLevel->Set_NextLevel(m_eNextLevel);
        }
        else if (LEVEL_SHOP == m_eCurLevel)
        {
            CLevel_Shop* pCurLevel = dynamic_cast<CLevel_Shop*>(pLevel);
            pCurLevel->Set_NextLevel(m_eNextLevel);
        }
        else if (LEVEL_PUZZLE == m_eCurLevel)
        {
            CLevel_Puzzle* pCurLevel = dynamic_cast<CLevel_Puzzle*>(pLevel);
            pCurLevel->Set_NextLevel(m_eNextLevel);
        }
        else if (LEVEL_BOSS == m_eCurLevel)
        {
            CLevel_Boss* pCurLevel = dynamic_cast<CLevel_Boss*>(pLevel);
            pCurLevel->Set_NextLevel(m_eNextLevel);
        }

        m_isNext = true;
    }
}
