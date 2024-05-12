#include "stdafx.h"

#include "Player.h"
#include "NPC_Merchant.h"
#include "Merchant_State_Hidden.h"

CMerchant_State_Hidden::CMerchant_State_Hidden(CNPC_Merchant* pNPC, CPlayer* pPlayer)
{
    m_pNPC = pNPC;
    m_pPlayer = pPlayer;

    //Safe_AddRef(m_pNPC);
    //Safe_AddRef(m_pPlayer);

    m_pNPCTransform = dynamic_cast<CTransform*>(m_pNPC->Get_Component(g_strTransformTag));
    m_pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag));

    //Safe_AddRef(m_pNPCTransform);
    //Safe_AddRef(m_pPlayerTransform);
}   

void CMerchant_State_Hidden::OnStateEnter()
{
    m_pNPC->Set_Blending(true, CNPC_Merchant::ANIM_HIDDEN);
}

void CMerchant_State_Hidden::OnStateUpdate(_float fTimeDelta)
{
    _vector vPlayerPosition = m_pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vNPCPosition = m_pNPCTransform->Get_State_Vector(CTransform::STATE_POSITION);

    _float fDistance = vNPCPosition.m128_f32[2] - vPlayerPosition.m128_f32[2];

    if (true == m_isHidden && m_fWakeUpDistance > fDistance)
    {
        m_pNPC->Set_Blending(true, CNPC_Merchant::ANIM_WAKEUP);
        m_isHidden = false;
        m_pGameInstance->Play_Once(TEXT("NPC_Arrive.wav"), CSound_Manager::MONSTER1);
    }

    if (false == m_isHidden && true == m_pNPC->Get_isFinished(CNPC_Merchant::ANIM_WAKEUP))
        m_pNPC->Change_State(CNPC_Merchant::STATE_IDLE);
}

void CMerchant_State_Hidden::OnStateExit()
{
    m_isHidden = true;
}

CMerchant_State_Hidden* CMerchant_State_Hidden::Create(CNPC_Merchant* pNPC, CPlayer* pPlayer)
{
    CMerchant_State_Hidden* pInstance = new CMerchant_State_Hidden(pNPC, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CMerchant_State_Hidden"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMerchant_State_Hidden::Free()
{
    __super::Free();

    //Safe_Release(m_pNPC);
    //Safe_Release(m_pPlayer);
    //Safe_Release(m_pNPCTransform);
    //Safe_Release(m_pPlayerTransform);
}