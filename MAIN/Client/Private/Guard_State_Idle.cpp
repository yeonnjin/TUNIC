#include "stdafx.h"

#include "Player.h"
#include "Monster_Guard.h"
#include "Guard_State_Idle.h"

#include "Easing.h"

CGuard_State_Idle::CGuard_State_Idle(CMonster_Guard* pMonster, CPlayer* pPlayer)
{
    m_pMonster = pMonster;
    m_pPlayer = pPlayer;

    m_pMonsterTransform = dynamic_cast<CTransform*>(m_pMonster->Get_Component(g_strTransformTag));
    m_pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag));

    m_pEasing = CEasing::Get_Instance();
}

void CGuard_State_Idle::OnStateEnter()
{
    m_pMonster->Set_Blending(true, CMonster_Guard::ANIM_IDLE);

    /* 목표 방향 설정 */
    m_vEnterPosition = m_pMonsterTransform->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vPlayerPosition = m_pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION);

    m_vTargetDir = XMVector3Normalize(m_vEnterPosition - vPlayerPosition);
    _vector vLookDir = XMVector3Normalize(m_pMonsterTransform->Get_State_Vector(CTransform::STATE_LOOK));
    if (false == XMVector3Equal(m_vTargetDir, vLookDir))
        m_isLook = false;
}

void CGuard_State_Idle::OnStateUpdate(_float fTimeDelta)
{
    m_fAccChangeTime += fTimeDelta;

    // 플레이어 바라보기
    _vector vMonsterPosition = m_pMonsterTransform->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vPlayerPosition = m_pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION);
    if (false == m_isLook)
    {
        m_vTargetDir = vMonsterPosition - vPlayerPosition;
        m_vTargetDir.m128_f32[1] = 0.f;
        m_vTargetDir.m128_f32[3] = 0.f;
        m_vTargetDir = XMVector3Normalize(m_vTargetDir);
        _vector vLookDir = m_pMonsterTransform->Get_State_Vector(CTransform::STATE_LOOK);
        vLookDir.m128_f32[1] = 0.f;
        vLookDir.m128_f32[3] = 0.f;
        vLookDir = XMVector3Normalize(vLookDir);
        if (false == XMVector3Equal(m_vTargetDir, vLookDir))
        {
            m_fAccLookTime += fTimeDelta;
            _float fRatio = m_fAccLookTime / m_fLookTime;
            if (fRatio >= 1)
            {
                fRatio = 1.f;
                m_isLook = true;
            }

            _vector vDir;
            vDir.m128_f32[0] = m_pEasing->Get_Ease(CEasing::Ease_OutQuad, vLookDir.m128_f32[0], m_vTargetDir.m128_f32[0], fRatio);
            vDir.m128_f32[1] = m_pEasing->Get_Ease(CEasing::Ease_OutQuad, vLookDir.m128_f32[1], m_vTargetDir.m128_f32[1], fRatio);
            vDir.m128_f32[2] = m_pEasing->Get_Ease(CEasing::Ease_OutQuad, vLookDir.m128_f32[2], m_vTargetDir.m128_f32[2], fRatio);
            vDir.m128_f32[3] = 0.f;

            m_pMonsterTransform->Look_At_Dir(vDir);
        }
    }

    if (m_fAccChangeTime > m_fChangeTime)
    {
        _float fDistance = XMVector3Length(vPlayerPosition - vMonsterPosition).m128_f32[0];

        if (FIND_DISTANCE > fDistance)
        {
            m_pMonster->Change_State(CMonster_Guard::STATE_AGGRO);
            m_fAccChangeTime = 0.f;
        }
    }
}

void CGuard_State_Idle::OnStateExit()
{
    m_isLook = true;

    m_fAccChangeTime = 0.f;
    m_fAccLookTime = 0.f;
}

CGuard_State_Idle* CGuard_State_Idle::Create(CMonster_Guard* pMonster, CPlayer* pPlayer)
{
    CGuard_State_Idle* pInstance = new CGuard_State_Idle(pMonster, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CGuard_State_Idle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGuard_State_Idle::Free()
{
    __super::Free();
}
