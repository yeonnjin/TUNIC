#include "stdafx.h"

#include "Easing.h"
#include "Player.h"
#include "Monster_Librarian.h"
#include "Librarian_State_Entry.h"

CLibrarian_State_Entry::CLibrarian_State_Entry(CMonster_Librarian* pMonster, CPlayer* pPlayer)
{
    m_pMonster = pMonster;
    m_pPlayer = pPlayer;
    m_pEasing = CEasing::Get_Instance();
    m_pMonsterTransform = dynamic_cast<CTransform*>(m_pMonster->Get_Component(g_strTransformTag));
}

void CLibrarian_State_Entry::OnStateEnter()
{
    m_pMonster->Set_Blending(true, CMonster_Librarian::ANIM_ENTRY);

    m_fOriginRotation = m_pMonsterTransform->Get_RotationPerSec();
    m_pMonsterTransform->Set_RotationPerSec(1.f);
}

void CLibrarian_State_Entry::OnStateUpdate(_float fTimeDelta)
{
    _float4 vPosition = _float4(-7.f, 0.5f, -7.f, 1.f);
    CTransform* pMonsterTransform = dynamic_cast<CTransform*>(m_pMonster->Get_Component(g_strTransformTag));
    pMonsterTransform->Set_State(CTransform::STATE_POSITION, vPosition);

    if (true == m_pGameInstance->Get_DIKeyState(DIK_0, KEY_DOWN))
        m_isTrigger = true;

    if (true == m_isTrigger)
    {
        m_pMonster->Set_Blending(true, CMonster_Librarian::ANIM_TURN);
        m_isActive = true;
        m_isTrigger = false;
    }

    // 22 ~ 56 : ео
    _uint iFrameIndex = m_pMonster->Get_Current_Frame(CMonster_Librarian::ANIM_TURN);
    _float fTargetAngle = 0.f;
    if (true == m_isActive && 22 <= iFrameIndex && 56 >= iFrameIndex)
    {
        _float fRatio = (_float)(iFrameIndex - 21) / 35.f;
        fTargetAngle = m_pEasing->Get_Ease(CEasing::Ease_OutQuart, 0.f, m_fTargetAngle, fRatio);
        m_pMonsterTransform->Rotation(_vector{ 0.f, 1.f, 0.f, 0.f }, 0.f);
    }

    _float fRatio = iFrameIndex / 189.f;
    _float fFinAngle = m_pEasing->Get_Ease(CEasing::Ease_OutQuart, 0, fTargetAngle, fRatio);
    m_pMonsterTransform->Turn(_vector{ 0.f, 1.f, 0.f, 0.f }, XMConvertToRadians(fFinAngle));


    if (true == m_isActive && m_pMonster->Get_isFinished(CMonster_Librarian::ANIM_TURN))
        m_pMonster->Change_State(CMonster_Librarian::STATE_IDLE);
}

void CLibrarian_State_Entry::OnStateExit()
{
    m_isTrigger = false;
    m_isActive = false;
}

CLibrarian_State_Entry* CLibrarian_State_Entry::Create(CMonster_Librarian* pMonster, CPlayer* pPlayer)
{
    CLibrarian_State_Entry* pInstance = new CLibrarian_State_Entry(pMonster, pPlayer);

    if (nullptr == pInstance)
    {
        MSG_BOX(TEXT("Failed To Create : CLibrarian_State_Entry"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLibrarian_State_Entry::Free()
{
    __super::Free();

    Safe_Release(m_pEasing);
}
