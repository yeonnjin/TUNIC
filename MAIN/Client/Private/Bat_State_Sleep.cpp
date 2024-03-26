#include "stdafx.h"

#include "Player.h"
#include "Monster_Bat.h"
#include "Bat_State_Sleep.h"

CBat_State_Sleep::CBat_State_Sleep(CMonster_Bat* pMonster, CPlayer* pPlayer)
{
    m_pMonster = pMonster;
    m_pPlayer = pPlayer;
}

void CBat_State_Sleep::OnStateEnter()
{
    m_pMonster->Set_Blending(true, CMonster_Bat::ANIM_SLEEPING);
}

void CBat_State_Sleep::OnStateUpdate(_float fTimeDelta)
{
    _vector vPlayerPosition = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vMonsterPosition = dynamic_cast<CTransform*>(m_pMonster->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION);

    if (false == m_isDetect && 5 > XMVector3Length(vPlayerPosition - vMonsterPosition).m128_f32[0])
    {
        m_pMonster->Set_Blending(true, CMonster_Bat::ANIM_WAKEUP);
        m_isDetect = true;
    }

    if (true == m_isDetect && true == m_pMonster->Get_isFinished(CMonster_Bat::ANIM_WAKEUP))
        m_pMonster->Change_State(CMonster_Bat::STATE_IDLE);
}

void CBat_State_Sleep::OnStateExit()
{
    m_isDetect = false;
}

CBat_State_Sleep* CBat_State_Sleep::Create(CMonster_Bat* pMonster, CPlayer* pPlayer)
{
    CBat_State_Sleep* pInstance = new CBat_State_Sleep(pMonster, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CBat_State_Sleep"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBat_State_Sleep::Free()
{
    __super::Free();
}