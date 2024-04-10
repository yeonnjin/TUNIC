#include "stdafx.h"

#include "Player.h"
#include "Monster_Librarian.h"
#include "Librarian_State_Melee.h"

#include "Easing.h"

CLibrarian_State_Melee::CLibrarian_State_Melee(CMonster_Librarian* pMonster, CPlayer* pPlayer)
{
    m_pMonster = pMonster;
    m_pPlayer = pPlayer;
    m_pMonsterTransform = dynamic_cast<CTransform*>(m_pMonster->Get_Component(g_strTransformTag));
    m_pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag));

    m_pEasing = CEasing::Get_Instance();

    m_eAnimation[0] = CMonster_Librarian::ANIM_MELEE_WIDE;
    m_eAnimation[1] = CMonster_Librarian::ANIM_MELEE_THRUST;
}

void CLibrarian_State_Melee::OnStateEnter()
{
    // 패턴
    // 0. ANIM_MELEE_WIDE, ANIM_MELEE_AOE, ANIM_MELEE_THRUST 중 1개 재생
    // 1. IDLE 상태이고, 회전 궤도에 진입했을 때 플레이어가 근처에 있으면 재생

    m_pMonster->Set_Blending(true, m_eAnimation[m_iPattern]);

    m_vOriginPosition = m_pMonsterTransform->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vPlayerPostion = m_pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vDir = XMVector3Normalize(m_vOriginPosition - vPlayerPostion);
    m_vRetreatPosition = m_vOriginPosition + vDir * 4.f;
    m_vDashPosition = m_vOriginPosition - vDir * 15.f;

    m_pMonsterTransform->Look_At_Dir(vDir);
}

void CLibrarian_State_Melee::OnStateUpdate(_float fTimeDelta)
{
    // ANIM_MELEE_THRUST 애니메이션 제어
    if (1 == m_iPattern)
        Play_Thrust();

    if (true == m_pMonster->Get_isFinished(m_eAnimation[m_iPattern]))
    {
        m_pMonster->Change_State(CMonster_Librarian::STATE_IDLE);
    }
}

void CLibrarian_State_Melee::OnStateExit()
{
    ++m_iPattern;
    m_iPattern = (m_iPattern == 2) ? 0 : m_iPattern;
}

void CLibrarian_State_Melee::Play_Thrust()
{
    // 1 ~ 15 : 뒤로 밀리기
    // 21 ~ 35 : 전진, 멈추기

    _uint iFrameIndex = m_pMonster->Get_Current_Frame(m_eAnimation[m_iPattern]);
    CTransform* pMonsterTransform = dynamic_cast<CTransform*>(m_pMonster->Get_Component(g_strTransformTag));
    _vector vMonsterPosition = pMonsterTransform->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vTargetPosition = vMonsterPosition;

    if (1 <= iFrameIndex && 15 >= iFrameIndex)
    {
        _float fRatio = (_float)(iFrameIndex - 0) / 15.f;
        vTargetPosition.m128_f32[0] = m_pEasing->Get_Ease(CEasing::Ease_OutQuart, m_vOriginPosition.m128_f32[0], m_vRetreatPosition.m128_f32[0], fRatio);
        vTargetPosition.m128_f32[1] = m_pEasing->Get_Ease(CEasing::Ease_OutQuart, m_vOriginPosition.m128_f32[1], m_vRetreatPosition.m128_f32[1], fRatio);
        vTargetPosition.m128_f32[2] = m_pEasing->Get_Ease(CEasing::Ease_OutQuart, m_vOriginPosition.m128_f32[2], m_vRetreatPosition.m128_f32[2], fRatio);
        vTargetPosition.m128_f32[3] = 1.f;
    }
    else if (21 <= iFrameIndex && 35 >= iFrameIndex)
    {
        _float fRatio = (_float)(iFrameIndex - 20) / 15.f;
        vTargetPosition.m128_f32[0] = m_pEasing->Get_Ease(CEasing::Ease_OutQuart, m_vRetreatPosition.m128_f32[0], m_vDashPosition.m128_f32[0], fRatio);
        vTargetPosition.m128_f32[1] = m_pEasing->Get_Ease(CEasing::Ease_OutQuart, m_vRetreatPosition.m128_f32[1], m_vDashPosition.m128_f32[1], fRatio);
        vTargetPosition.m128_f32[2] = m_pEasing->Get_Ease(CEasing::Ease_OutQuart, m_vRetreatPosition.m128_f32[2], m_vDashPosition.m128_f32[2], fRatio);
        vTargetPosition.m128_f32[3] = 1.f;
    }

    _float fRatio = iFrameIndex / 52.f;

    _vector vFinPosition = XMVectorLerp(vMonsterPosition, vTargetPosition, fRatio);
    m_pMonsterTransform->Set_State(CTransform::STATE_POSITION, vFinPosition);
}

CLibrarian_State_Melee* CLibrarian_State_Melee::Create(CMonster_Librarian* pMonster, CPlayer* pPlayer)
{
    CLibrarian_State_Melee* pInstance = new CLibrarian_State_Melee(pMonster, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CLibrarian_State_Melee"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLibrarian_State_Melee::Free()
{
    __super::Free();

    Safe_Release(m_pEasing);
}
