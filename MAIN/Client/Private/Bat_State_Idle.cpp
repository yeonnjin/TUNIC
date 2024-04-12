#include "stdafx.h"

#include "Player.h"
#include "Monster_Bat.h"
#include "Bat_State_Idle.h"

#include "Easing.h"

CBat_State_Idle::CBat_State_Idle(CMonster_Bat* pMonster, CPlayer* pPlayer)
{
	m_pMonster = pMonster;
	m_pPlayer = pPlayer;

    m_pMonsterTransform = dynamic_cast<CTransform*>(m_pMonster->Get_Component(g_strTransformTag));
    m_pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag));

    m_pEasing = CEasing::Get_Instance();
}

void CBat_State_Idle::OnStateEnter()
{
	m_pMonster->Set_Blending(true, CMonster_Bat::ANIM_IDLE);

    /* 목표 방향 설정 */
    m_vEnterPosition = m_pMonsterTransform->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vPlayerPosition = m_pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION);

    m_vTargetDir = XMVector3Normalize(m_vEnterPosition - vPlayerPosition);
    _vector vLookDir = XMVector3Normalize(m_pMonsterTransform->Get_State_Vector(CTransform::STATE_LOOK));
    if (false == XMVector3Equal(m_vTargetDir, vLookDir))
        m_isLook = false;
}

void CBat_State_Idle::OnStateUpdate(_float fTimeDelta)
{
    m_fAccIdleTime += fTimeDelta;

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
    else
    {
        dynamic_cast<CTransform*>(m_pMonster->Get_Component(g_strTransformTag))->Look_At_For_LandOject(vPlayerPosition, true);
    }

    if (m_fAccIdleTime > m_fIdleTime)
    {
        _float fDistance = XMVector3Length(vPlayerPosition - vMonsterPosition).m128_f32[0];

        if (m_fAttackDistance > fDistance)
        {
            m_pMonster->Change_State(CMonster_Bat::STATE_ATTACK);
        }
        else
        {
            dynamic_cast<CTransform*>(m_pMonster->Get_Component(g_strTransformTag))->Go_Backward(fTimeDelta);
        }
    }  
}

void CBat_State_Idle::OnStateExit()
{
    m_isLook = true;

    m_fAccIdleTime = 0.f;
    m_fAccLookTime = 0.f;
}

CBat_State_Idle* CBat_State_Idle::Create(CMonster_Bat* pMonster, CPlayer* pPlayer)
{
    CBat_State_Idle* pInstance = new CBat_State_Idle(pMonster, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CBat_State_Idle"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CBat_State_Idle::Free()
{
    __super::Free();
}