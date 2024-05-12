#include "stdafx.h"

#include "Player.h"
#include "Monster_CowBot.h"
#include "CowBot_State_Run.h"

#include "Easing.h"

CCowBot_State_Run::CCowBot_State_Run(CMonster_CowBot* pMonster, CPlayer* pPlayer)
{
    m_pMonster = pMonster;
    m_pPlayer = pPlayer;

    m_pMonsterTransform = dynamic_cast<CTransform*>(m_pMonster->Get_Component(g_strTransformTag));
    m_pPlayerTransform = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag));

    m_pEasing = CEasing::Get_Instance();
}

void CCowBot_State_Run::OnStateEnter()
{
    m_pGameInstance->Play_Once(TEXT("MONSTER_CowBot_Aggro.wav"), CSound_Manager::MONSTER3);
    m_pMonster->Set_isAggro(true);
    m_pMonster->Set_Blending(true, CMonster_CowBot::ANIM_RUN);

    /* 목표 방향 설정 */
    m_vEnterPosition = m_pMonsterTransform->Get_State_Vector(CTransform::STATE_POSITION);
    _vector vPlayerPosition = m_pPlayerTransform->Get_State_Vector(CTransform::STATE_POSITION);

    m_vTargetDir = XMVector3Normalize(m_vEnterPosition - vPlayerPosition);
    _vector vLookDir = XMVector3Normalize(m_pMonsterTransform->Get_State_Vector(CTransform::STATE_LOOK));
    if (false == XMVector3Equal(m_vTargetDir, vLookDir))
        m_isLook = false;
}

void CCowBot_State_Run::OnStateUpdate(_float fTimeDelta)
{
    m_fAccChangeTime += fTimeDelta;

    m_fAccSoundTime += fTimeDelta;
    if (m_fAccSoundTime >= m_fSoundTime)
    {
        m_pGameInstance->Play_Once(TEXT("MONSTER_CowBot_Run.wav"), CSound_Manager::MONSTER3);
        m_fAccSoundTime = 0.f;
    }

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

    m_pMonsterTransform->Go_Backward(fTimeDelta);

    if (m_fAccChangeTime > m_fChangeTime)
    {
        _float fDistance = XMVector3Length(vPlayerPosition - vMonsterPosition).m128_f32[0];

        if (4 > fDistance)
            m_pMonster->Change_State(CMonster_CowBot::STATE_ATTACK);

        else if (FIND_DISTANCE < fDistance)
        {
            m_pMonster->Change_State(CMonster_CowBot::STATE_IDLE);
        }
    }  
}

void CCowBot_State_Run::OnStateExit()
{
    m_isLook = true;

    m_fAccLookTime = 0.f;
    m_fAccChangeTime = 0.f;
    m_fAccSoundTime = 0.f;

    m_pMonster->Set_isAggro(false);
}

CCowBot_State_Run* CCowBot_State_Run::Create(CMonster_CowBot* pMonster, CPlayer* pPlayer)
{
    CCowBot_State_Run* pInstance = new CCowBot_State_Run(pMonster, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CCowBot_State_Run"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CCowBot_State_Run::Free()
{
    __super::Free();
}

