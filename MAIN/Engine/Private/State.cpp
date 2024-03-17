#include "State.h"
#include "GameInstance.h"

CState::CState()
	: m_pGameInstance(CGameInstance::Get_Instance())
{
	Safe_AddRef(m_pGameInstance);
}

void CState::Free()
{
	__super::Free();

	Safe_Release(m_pGameInstance);
}
