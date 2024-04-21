#include "stdafx.h"

#include "Player.h"
#include "NPC_Merchant.h"
#include "Merchant_State_Thankyou.h"

CMerchant_State_Thankyou::CMerchant_State_Thankyou(CNPC_Merchant* pNPC, CPlayer* pPlayer)
{
    m_pNPC = pNPC;
    m_pPlayer = pPlayer;

    m_pNPCTransform = dynamic_cast<CTransform*>(m_pNPC->Get_Component(g_strTransformTag));
    m_pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag));
}

void CMerchant_State_Thankyou::OnStateEnter()
{
    m_pNPC->Set_Blending(true, CNPC_Merchant::ANIM_THANKYOU);
}

void CMerchant_State_Thankyou::OnStateUpdate(_float fTimeDelta)
{
    if (true == m_pNPC->Get_isFinished(CNPC_Merchant::ANIM_THANKYOU))
        m_pNPC->Change_State(CNPC_Merchant::STATE_IDLE);
}

void CMerchant_State_Thankyou::OnStateExit()
{
}

CMerchant_State_Thankyou* CMerchant_State_Thankyou::Create(CNPC_Merchant* pNPC, CPlayer* pPlayer)
{
    CMerchant_State_Thankyou* pInstance = new CMerchant_State_Thankyou(pNPC, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CMerchant_State_Thankyou"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CMerchant_State_Thankyou::Free()
{
    __super::Free();
}