#include "stdafx.h"

#include "Player.h"
#include "Monster_Spinner.h"
#include "Spinner_State_Hidden.h"

CSpinner_State_Hidden::CSpinner_State_Hidden(CMonster_Spinner* pMonster, CPlayer* pPlayer)
{
    m_pMonster = pMonster;
    m_pPlayer = pPlayer;
}

void CSpinner_State_Hidden::OnStateEnter()
{
    m_pMonster->Set_Blending(true, CMonster_Spinner::ANIM_HIDDEN);
}

void CSpinner_State_Hidden::OnStateUpdate(_float fTimeDelta)
{
    _vector vPlayerPosition = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vMonsterPosition = dynamic_cast<CTransform*>(m_pMonster->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION);

    if (false == m_isEmerge && 5 > XMVector3Length(vPlayerPosition - vMonsterPosition).m128_f32[0])
    {
        m_pMonster->Set_Blending(true, CMonster_Spinner::ANIM_EMERGE);
        m_isEmerge = true;
        m_pMonster->Set_isAggro(true);
    }

    if (true == m_isEmerge && true == m_pMonster->Get_isFinished(CMonster_Spinner::ANIM_EMERGE))
        m_pMonster->Change_State(CMonster_Spinner::STATE_IDLE);
}

void CSpinner_State_Hidden::OnStateExit()
{
    m_isEmerge = false;
}

CSpinner_State_Hidden* CSpinner_State_Hidden::Create(CMonster_Spinner* pMonster, CPlayer* pPlayer)
{
    CSpinner_State_Hidden* pInstance = new CSpinner_State_Hidden(pMonster, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CSpinner_State_Hidden"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSpinner_State_Hidden::Free()
{
    __super::Free();
}