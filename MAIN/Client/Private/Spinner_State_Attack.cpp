#include "stdafx.h"

#include "Player.h"
#include "Monster_Spinner.h"
#include "Spinner_State_Attack.h"

CSpinner_State_Attack::CSpinner_State_Attack(CMonster_Spinner* pMonster, CPlayer* pPlayer)
{
    m_pMonster = pMonster;
    m_pPlayer = pPlayer;
}

void CSpinner_State_Attack::OnStateEnter()
{
    m_pMonster->Set_Blending(true, CMonster_Spinner::ANIM_START_SPIN);
}

void CSpinner_State_Attack::OnStateUpdate(_float fTimeDelta)
{
    // 준비 모션이 끝나면
    if (false == m_isAttack && true == m_pMonster->Get_isFinished(CMonster_Spinner::ANIM_START_SPIN))
    {
        // 플레이어 위치로 돌진
        _vector vPlayerPosition = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION);
        dynamic_cast<CTransform*>(m_pMonster->Get_Component(g_strTransformTag))->Look_At_For_LandOject(vPlayerPosition, true);
        m_pMonster->Set_Blending(true, CMonster_Spinner::ANIM_FORWARD);
        m_isAttack = true;
    }

    // 돌진 끝나면 IDLE
    if (true == m_isAttack && true == m_pMonster->Get_isFinished(CMonster_Spinner::ANIM_FORWARD))
    {
        m_pMonster->Change_State(CMonster_Spinner::STATE_IDLE);
        m_isAttack = false;
    }
}

void CSpinner_State_Attack::OnStateExit()
{
    m_isAttack = false;
}

CSpinner_State_Attack* CSpinner_State_Attack::Create(CMonster_Spinner* pMonster, CPlayer* pPlayer)
{
    CSpinner_State_Attack* pInstance = new CSpinner_State_Attack(pMonster, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CSpinner_State_Attack"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSpinner_State_Attack::Free()
{
    __super::Free();
}