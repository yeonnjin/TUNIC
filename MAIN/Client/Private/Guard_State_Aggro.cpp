#include "stdafx.h"

#include "Player.h"
#include "Monster_Guard.h"
#include "Guard_State_Aggro.h"

#include "Easing.h"

CGuard_State_Aggro::CGuard_State_Aggro(CMonster_Guard* pMonster, CPlayer* pPlayer)
{
	m_pMonster = pMonster;
	m_pPlayer = pPlayer;

    m_pMonsterTransform = dynamic_cast<CTransform*>(m_pMonster->Get_Component(g_strTransformTag));
    m_pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag));

    m_pEasing = CEasing::Get_Instance();
}

void CGuard_State_Aggro::OnStateEnter()
{
    m_pGameInstance->Play_Once(TEXT("MONSTER_Guard_Aggro.wav"), CSound_Manager::MONSTER4);
    m_pMonster->Set_isAggro(true);
	m_pMonster->Set_Blending(true, CMonster_Guard::ANIM_AGGRO);

    /* 목표 방향 설정 */
    m_vEnterPosition = m_pMonsterTransform->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vPlayerPosition = m_pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION);

    m_vTargetDir = XMVector3Normalize(m_vEnterPosition - vPlayerPosition);
    _vector vLookDir = XMVector3Normalize(m_pMonsterTransform->Get_State_Vector(CTransform::STATE_LOOK));
    if (false == XMVector3Equal(m_vTargetDir, vLookDir))
        m_isLook = false;
}

void CGuard_State_Aggro::OnStateUpdate(_float fTimeDelta)
{
    m_fAccChangeTime += fTimeDelta;

    // 플레이어 바라보기
    _vector vMonsterPosition = m_pMonsterTransform->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vPlayerPosition = m_pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION);
    if (false == m_isLook)
    {
        m_vTargetDir = XMVector3Normalize(vMonsterPosition - vPlayerPosition);
        _vector vLookDir = XMVector3Normalize(m_pMonsterTransform->Get_State_Vector(CTransform::STATE_LOOK));
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

        if (m_fIdleDistance < fDistance)
        {
            m_pMonster->Change_State(CMonster_Guard::STATE_IDLE);
            m_fAccChangeTime = 0.f;
        }
        else if (m_fAttackDistance > fDistance)
        {
            m_pMonster->Change_State(CMonster_Guard::STATE_BLOCK);
            m_fAccChangeTime = 0.f;
        }
    }
}

void CGuard_State_Aggro::OnStateExit()
{
    m_isLook = true;

    m_fAccChangeTime = 0.f;
    m_fAccLookTime = 0.f;

    m_pMonster->Set_isAggro(false);
}

CGuard_State_Aggro* CGuard_State_Aggro::Create(CMonster_Guard* pMonster, CPlayer* pPlayer)
{
    CGuard_State_Aggro* pInstance = new CGuard_State_Aggro(pMonster, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CGuard_State_Aggro"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGuard_State_Aggro::Free()
{
    __super::Free();
}
