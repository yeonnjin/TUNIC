#include "stdafx.h"

#include "Player.h"
#include "Monster_Blob.h"
#include "Blob_State_Attack.h"

CBlob_State_Attack::CBlob_State_Attack(CMonster_Blob* pMonster, CPlayer* pPlayer)
{
    m_pMonster = pMonster;
    m_pPlayer = pPlayer;
}

void CBlob_State_Attack::OnStateEnter()
{
    m_pMonster->Set_Blending(true, CMonster_Blob::ANIM_ATTACK);
}

void CBlob_State_Attack::OnStateUpdate(_float fTimeDelta)
{
    _uint iFrame = m_pMonster->Get_Current_Frame(CMonster_Blob::ANIM_ATTACK);
    if (34 <= iFrame && 40 >= iFrame)
        m_pMonster->Set_isAttackFrame(true);
    else
        m_pMonster->Set_isAttackFrame(false);

    if (true == m_pMonster->Get_isFinished(CMonster_Blob::ANIM_ATTACK))
    {
        m_pMonster->Change_State(CMonster_Blob::STATE_IDLE);
    }
}

void CBlob_State_Attack::OnStateExit()
{
}

CBlob_State_Attack* CBlob_State_Attack::Create(CMonster_Blob* pMonster, CPlayer* pPlayer)
{
    CBlob_State_Attack* pInstance = new CBlob_State_Attack(pMonster, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CBlob_State_Attack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBlob_State_Attack::Free()
{
    __super::Free();
}
