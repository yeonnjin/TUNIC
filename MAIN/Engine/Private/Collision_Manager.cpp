#include "Collision_Manager.h"

#include "GameInstance.h"
#include "GameObject.h"

CCollision_Manager::CCollision_Manager()
{
}

HRESULT CCollision_Manager::Add_Group(GROUP eCollisionGroup, CGameObject* pGameObject)
{
    if (GROUP_END <= eCollisionGroup)
        return E_FAIL;

    m_arrCollisionGroups[eCollisionGroup].push_back(pGameObject);
    Safe_AddRef(pGameObject);

    return S_OK;
}

HRESULT CCollision_Manager::Add_RigidGroup(CGameObject* pGameObject)
{
    m_RigidGroups.push_back(pGameObject);
    Safe_AddRef(pGameObject);

    return S_OK;
}

HRESULT CCollision_Manager::Clear_Group()
{
    for (auto& vecGameObject : m_arrCollisionGroups)
    {
        for (auto& Object : vecGameObject)
            Safe_Release(Object);

        vecGameObject.clear();
    }

    for (auto& vecGameObject : m_RigidGroups)
        Safe_Release(vecGameObject);       

    m_RigidGroups.clear();

    return S_OK;
}

void CCollision_Manager::Check_Collision_Groups(GROUP eCollisionGroupA, GROUP eCollisionGroupB)
{
    for (auto& GameObjectA : m_arrCollisionGroups[eCollisionGroupA])
    {
        for (auto& GameObjectB : m_arrCollisionGroups[eCollisionGroupB])
        {
            if (GameObjectA == GameObjectB)
                continue;

            CCollider* pColliderA = dynamic_cast<CCollider*>(GameObjectA->Get_Component(TEXT("Com_Collider")));
            CCollider* pColliderB = dynamic_cast<CCollider*>(GameObjectB->Get_Component(TEXT("Com_Collider")));

            if (true == pColliderA->Check_Collision(pColliderB))
            {
                GameObjectA->Collision_Event(GameObjectB);
                GameObjectB->Collision_Event(GameObjectA);
            }
        }
    }
}

void CCollision_Manager::Check_Rigid_Groups()
{
    for (size_t i = 0; i < m_RigidGroups.size(); i++)
    {
        for (size_t j = i + 1; j < m_RigidGroups.size(); j++)
        {
            if (m_RigidGroups[i] == m_RigidGroups[j])
                continue;

            CCollider* pColliderA = dynamic_cast<CCollider*>(m_RigidGroups[i]->Get_Component(TEXT("Com_RigidCollider")));
            CCollider* pColliderB = dynamic_cast<CCollider*>(m_RigidGroups[j]->Get_Component(TEXT("Com_RigidCollider")));

            if (true == pColliderA->Check_Collision(pColliderB))
            {
                m_RigidGroups[i]->Rigid_Event(m_RigidGroups[j]);
                m_RigidGroups[j]->Rigid_Event(m_RigidGroups[i]);
            }
        }
    }
}

HRESULT CCollision_Manager::Initialize()
{
    return S_OK;
}

CCollision_Manager* CCollision_Manager::Create()
{
    CCollision_Manager* pInstance = new CCollision_Manager();

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed To Create : CCollision_Manager"));

        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCollision_Manager::Free()
{
    __super::Free();

    for (auto& vecGameObject : m_arrCollisionGroups)
    {
        for (auto& Object : vecGameObject)
            Safe_Release(Object);

        vecGameObject.clear();
    }

    for (auto& vecGameObject : m_RigidGroups)
        Safe_Release(vecGameObject);

    m_RigidGroups.clear();
}
