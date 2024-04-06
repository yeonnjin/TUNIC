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
    m_pMonster->Set_isMove(true);
    m_vPrePosition = dynamic_cast<CTransform*>(m_pMonster->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION);
}

void CSpinner_State_Attack::OnStateUpdate(_float fTimeDelta)
{
    CTransform* pMonsterTransform = dynamic_cast<CTransform*>(m_pMonster->Get_Component(g_strTransformTag));
    _vector vPlayerPosition = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vMonsterPosition = pMonsterTransform->Get_State_Vector(CTransform::STATE_POSITION);

    // 준비 모션이 끝나면 : SPIN 애니메이션 재생
    if (false == m_isAttack && true == m_pMonster->Get_isFinished(CMonster_Spinner::ANIM_START_SPIN))
    {
        m_pMonster->Set_Blending(true, CMonster_Spinner::ANIM_FORWARD);     
        m_isAttack = true;
    }

    // 플레이어를 쫓아오면서 틱 뎀
    if (true == m_isAttack && false == m_pMonster->isCollision())
    {
        pMonsterTransform->Look_At_For_LandOject(vPlayerPosition, true);
    }

    //// 몬스터끼리 충돌 상태일 떄 : 지정된 룩 방향으로 이동
    //if (true == m_pMonster->isCollision())
    //{

    //}

    // 플레이어와의 거리가 일정 이상이면 IDLE
    
    _vector vDistance = vPlayerPosition - vMonsterPosition;

    if (FIND_DISTANCE < XMVector3Length(vDistance).m128_f32[0])
    {
        m_pMonster->Change_State(CMonster_Spinner::STATE_IDLE);
    }

    // 플레이어와 거리가 가까우면 제자리에서 재생
    if (2.f > XMVector3Length(vDistance).m128_f32[0])
    {
        pMonsterTransform->Set_State(CTransform::STATE_POSITION, m_vPrePosition);
    }   

    m_vPrePosition = pMonsterTransform->Get_State_Vector(CTransform::STATE_POSITION);
}

void CSpinner_State_Attack::OnStateExit()
{
    m_isAttack = false;
    m_pMonster->Set_isMove(false);
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