#include "stdafx.h"

#include "Player.h"
#include "Monster_Librarian.h"
#include "Librarian_State_Pattern_Summon_Bat.h"

#include "Monster_Bat.h"

CLibrarian_State_Pattern_Summon_Bat::CLibrarian_State_Pattern_Summon_Bat(CMonster_Librarian* pMonster, CPlayer* pPlayer)
{
    m_pMonster = pMonster;
    m_pPlayer = pPlayer;

    m_vPositions[0] = { 9.224f, 0.084f, -0.056f, 1.f };
    m_vPositions[1] = { -9.565f, 0.084f, -0.229f, 1.f };
    m_vPositions[2] = { 0.129f, 0.084f, 9.505f, 1.f };
    m_vPositions[3] = { 0.129f, 0.084f, -9.097f, 1.f };

    m_vIndices[0] = 31;
    m_vIndices[1] = 42;
    m_vIndices[2] = 26;
    m_vIndices[3] = 36;
}

void CLibrarian_State_Pattern_Summon_Bat::OnStateEnter()
{
    // 패턴
    // 0. ANIM_SUMMON_ORB_START
    // 1. ANIM_SUMMON_ORB_LOOP  : 박쥐 4마리 소환
    // 2. ANIM_SUMMON_ORB_END   : 패턴 종료

    m_pMonster->Set_Blending(true, CMonster_Librarian::ANIM_SUMMON_ORB_START);

    m_pGameInstance->Play_Once(TEXT("BOSS_Pattern_Summon_Bat.wav"), CSound_Manager::BOSS);
}

void CLibrarian_State_Pattern_Summon_Bat::OnStateUpdate(_float fTimeDelta)
{
    // 0 -> 1
    if (0 == m_iPattern && true == m_pMonster->Get_isFinished(CMonster_Librarian::ANIM_SUMMON_ORB_START))
    {
        ++m_iPattern;

        // 1. ANIM_SUMMON_ORB_LOOP
        m_pMonster->Set_Blending(true, CMonster_Librarian::ANIM_SUMMON_ORB_LOOP);
    }

    // 1 -> 2 : 박쥐 소환
    if (1 == m_iPattern)
    {
        m_fAccLoopTime += fTimeDelta;
        if (m_fAccLoopTime > m_fLoopTime)
        {
            ++m_iPattern;

            CMonster_Bat::MONSTER_BAT_DESC tDesc = {};
            _char szModelTag[MAX_PATH] = "Prototype_Component_Model_Monster_Bat";
            wstring wstr(&szModelTag[0], &szModelTag[MAX_PATH]);
            tDesc.strModelComTag = wstr;
            tDesc.eLevel = LEVEL_BOSS;

            for (size_t i = 0; i < 4; ++i)
            {
                tDesc.vPosition = m_vPositions[i];
                tDesc.iNavigationIndex = m_vIndices[i];
                if (FAILED(m_pGameInstance->Add_Clone(LEVEL_BOSS, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_Monster_Bat"), &tDesc)))
                    return;
            }

            // 2. ANIM_SUMMON_ORB_END
            m_pMonster->Set_Blending(true, CMonster_Librarian::ANIM_SUMMON_ORB_END);
        }
    }

    // EXIT
    if (2 == m_iPattern && true == m_pMonster->Get_isFinished(CMonster_Librarian::ANIM_SUMMON_ORB_END))
    {
        m_pMonster->Change_State(CMonster_Librarian::STATE_IDLE);
    }   
}

void CLibrarian_State_Pattern_Summon_Bat::OnStateExit()
{
    m_iPattern = 0;
    m_fAccLoopTime = 0.f;
}

CLibrarian_State_Pattern_Summon_Bat* CLibrarian_State_Pattern_Summon_Bat::Create(CMonster_Librarian* pMonster, CPlayer* pPlayer)
{
    CLibrarian_State_Pattern_Summon_Bat* pInstance = new CLibrarian_State_Pattern_Summon_Bat(pMonster, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CLibrarian_State_Pattern_Summon_Bat"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLibrarian_State_Pattern_Summon_Bat::Free()
{
    __super::Free();
}
