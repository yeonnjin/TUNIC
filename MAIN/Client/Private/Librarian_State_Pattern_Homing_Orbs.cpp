#include "stdafx.h"

#include "Player.h"
#include "Monster_Librarian.h"
#include "Librarian_State_Pattern_Homing_Orbs.h"

#include "Librarian_Effect_Orb.h"

CLibrarian_State_Pattern_Homing_Orbs::CLibrarian_State_Pattern_Homing_Orbs(CMonster_Librarian* pMonster, CPlayer* pPlayer)
{
    m_pMonster = pMonster;
    m_pPlayer = pPlayer;

    for (size_t i = 0; i < 6; i++)
    {
        CLibrarian_Effect_Orb* pOrb = dynamic_cast<CLibrarian_Effect_Orb*>(m_pGameInstance->Get_GameObject_Clone(TEXT("Prototype_GameObject_Librarian_Attack_Orb")));
        m_Orbs.push_back(pOrb);
        Safe_AddRef(pOrb);
        pOrb->Set_Index(i);
        m_pGameInstance->Add_Clone(LEVEL_GAMEPLAY, TEXT("Librarian_Effect"), pOrb);
    }
}

void CLibrarian_State_Pattern_Homing_Orbs::OnStateEnter()
{
    // 패턴
    // 0. ANIM_SUMMON_ORB_START
    // 1. ANIM_SUMMON_ORB_LOOP  : 오브 6개 생성 (일자, 육각형)
    // 2. ANIM_SUMMON_ORB_END   : 오브 차례로 발사 (플레이어 방향)

    // 0. ANIM_SUMMON_ORB_START
    m_pMonster->Set_Blending(true, CMonster_Librarian::ANIM_SUMMON_ORB_START);

    m_iOrbPattern = (m_iOrbPattern == 0) ? 1 : 0;
    (m_iOrbPattern == 0) ? Set_Orbs_Line() : Set_Orbs_Hexagon();
}

void CLibrarian_State_Pattern_Homing_Orbs::OnStateUpdate(_float fTimeDelta)
{
    // 0 -> 1
    if (0 == m_iPattern && true == m_pMonster->Get_isFinished(CMonster_Librarian::ANIM_SUMMON_ORB_START))
    {
        ++m_iPattern;

        // 1. ANIM_SUMMON_ORB_LOOP
        m_pMonster->Set_Blending(true, CMonster_Librarian::ANIM_SUMMON_ORB_LOOP);
    }

    // 1 -> 2
    if (1 == m_iPattern)
    {
        m_fAccLoopTime += fTimeDelta;

        if (6 > m_iActiveOrb && (m_iActiveOrb + 1) / 7 <= m_fAccLoopTime / m_fLoopTime)
        {
            m_Orbs[m_iActiveOrb]->Set_Pooling_Active();
            ++m_iActiveOrb;
        }

        if (m_fAccLoopTime > m_fLoopTime)
        {
            ++m_iPattern;

            // 2. ANIM_SUMMON_ORB_END
            m_pMonster->Set_Blending(true, CMonster_Librarian::ANIM_SUMMON_ORB_END);
        }
    }

    // EXIT
    if (2 == m_iPattern && true == m_pMonster->Get_isFinished(CMonster_Librarian::ANIM_SUMMON_ORB_END))
    {
        _vector vPlayerPostion = dynamic_cast<CTransform*>(m_pPlayer->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION);
        for (size_t i = 0; i < m_Orbs.size(); i++)
        {
            _vector vOrbPosition = dynamic_cast<CTransform*>(m_Orbs[i]->Get_Component(g_strTransformTag))->Get_State_Vector(CTransform::STATE_POSITION);        
            _vector vDir = XMVector3Normalize(vOrbPosition - vPlayerPostion);
            m_Orbs[i]->Set_Target_Dir(vDir);
            m_Orbs[i]->Set_Move(true);
        }

        //m_pMonster->Change_State(CMonster_Librarian::STATE_PATTERN_LIGHTNING_WARP);
        m_pMonster->Change_State(CMonster_Librarian::STATE_IDLE);
    }
}

void CLibrarian_State_Pattern_Homing_Orbs::OnStateExit()
{
    m_iPattern = 0;
    m_fAccLoopTime = 0.f;
    m_iActiveOrb = 0;

    for (size_t i = 0; i < m_Orbs.size(); i++)
    {
        m_Orbs[i]->Set_Pooling_Inactive();
    }
}

void CLibrarian_State_Pattern_Homing_Orbs::Set_Orbs_Line()
{
    for (size_t i = 0; i < m_Orbs.size(); i++)
    {
        _vector StartPosition = { -9.f + i * 3.f, 2.f, 9.f , 1.f};
        m_Orbs[i]->Set_Start_Position(StartPosition);
    }
}

void CLibrarian_State_Pattern_Homing_Orbs::Set_Orbs_Hexagon()
{
    /*m_Orbs[0]->Set_Start_Position(_vector{0.f, 2.f, 9.f});
    m_Orbs[1]->Set_Start_Position(_vector{0.f, 2.f, 9.f});
    m_Orbs[2]->Set_Start_Position(_vector{0.f, 2.f, 9.f});
    m_Orbs[3]->Set_Start_Position(_vector{0.f, 2.f, 9.f});
    m_Orbs[4]->Set_Start_Position(_vector{0.f, 2.f, 9.f});
    m_Orbs[5]->Set_Start_Position(_vector{0.f, 2.f, 9.f});*/

    _vector StartPosition = { 9.f, 2.f, 0.f };
    
    for (size_t i = 0; i < m_Orbs.size(); i++)
    {      
        m_Orbs[i]->Set_Start_Position(XMVector3TransformCoord(StartPosition, XMMatrixRotationY(XMConvertToRadians(60.f * i))));
    }
}

CLibrarian_State_Pattern_Homing_Orbs* CLibrarian_State_Pattern_Homing_Orbs::Create(CMonster_Librarian* pMonster, CPlayer* pPlayer)
{
    CLibrarian_State_Pattern_Homing_Orbs* pInstance = new CLibrarian_State_Pattern_Homing_Orbs(pMonster, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CLibrarian_State_Pattern_Homing_Orbs"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLibrarian_State_Pattern_Homing_Orbs::Free()
{
    __super::Free();

    for (auto& pOrb : m_Orbs)
        Safe_Release(pOrb);
}
