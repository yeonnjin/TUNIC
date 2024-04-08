#include "stdafx.h"

#include "Player.h"
#include "Monster_Librarian.h"
#include "Librarian_State_Pattern_Energy_Wave.h"

#include "Librarian_Effect_Slash.h"

CLibrarian_State_Pattern_Energy_Wave::CLibrarian_State_Pattern_Energy_Wave(CMonster_Librarian* pMonster, CPlayer* pPlayer)
{
    m_pMonster = pMonster;
    m_pPlayer = pPlayer;
}

void CLibrarian_State_Pattern_Energy_Wave::OnStateEnter()
{
    // 패턴
    // 0. 칼 뒤로 슥 한다음에
    // 1. 에너지 웨이브 발사 (플레이어 기준?) , 오른쪽 아래부터 날리면 수평, 아님 수직
    // 2. 칼 아래에서 위로 올리기

    m_iMotionIndex = (m_iMotionIndex == CMonster_Librarian::ANIM_FLYING_SWING_OVERHEAD) ? CMonster_Librarian::ANIM_MELEE_WIDE : CMonster_Librarian::ANIM_FLYING_SWING_OVERHEAD;
    m_iSlashIndex = (m_iMotionIndex == CMonster_Librarian::ANIM_FLYING_SWING_OVERHEAD) ? 32 : 35;
    m_pMonster->Set_Blending(true, (CMonster_Librarian::ANIMATION)m_iMotionIndex);
}

void CLibrarian_State_Pattern_Energy_Wave::OnStateUpdate(_float fTimeDelta)
{
    _vector vPlayerPosition = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION);
    CTransform* pMonsterTransform = dynamic_cast<CTransform*>(m_pMonster->Get_Component(g_strTransformTag));
    pMonsterTransform->Look_At_For_LandOject(vPlayerPosition, true);
    //35
    if (false == m_isSlash && m_iSlashIndex == m_pMonster->Get_Current_Frame((CMonster_Librarian::ANIMATION)m_iMotionIndex))
    {
        _vector vMonsterPosition = pMonsterTransform->Get_State_Vector(CTransform::STATE_POSITION);
        CLibrarian_Effect_Slash::LIBRARIAN_EFFECT_SLASH_DESC tDesc{};
        tDesc.vStartPosition = pMonsterTransform->Get_State_Vector(CTransform::STATE_POSITION);
        tDesc.vStartPosition.m128_f32[1] += 2.f;
        tDesc.vLookDir = vMonsterPosition - vPlayerPosition;
        tDesc.isVertical = (m_iMotionIndex == CMonster_Librarian::ANIM_FLYING_SWING_OVERHEAD) ? true : false;

        if (FAILED(m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Librarian_Effect"), TEXT("Prototype_GameObject_Librarian_Attack_Slash"), &tDesc)))
            return;

        m_isSlash = true;
    }

    if(true == m_isSlash && true == m_pMonster->Get_isFinished((CMonster_Librarian::ANIMATION)m_iMotionIndex))
    {
        //m_pMonster->Change_State(CMonster_Librarian::STATE_PATTERN_LUNGE_SWIPE);
        m_pMonster->Change_State(CMonster_Librarian::STATE_IDLE);
    }
}

void CLibrarian_State_Pattern_Energy_Wave::OnStateExit()
{
    m_isSlash = false;
}

CLibrarian_State_Pattern_Energy_Wave* CLibrarian_State_Pattern_Energy_Wave::Create(CMonster_Librarian* pMonster, CPlayer* pPlayer)
{
    CLibrarian_State_Pattern_Energy_Wave* pInstance = new CLibrarian_State_Pattern_Energy_Wave(pMonster, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CLibrarian_State_Pattern_Energy_Wave"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLibrarian_State_Pattern_Energy_Wave::Free()
{
    __super::Free();
}

