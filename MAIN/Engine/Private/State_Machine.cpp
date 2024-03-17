#include "State_Machine.h"
#include "State.h"

CState_Machine::CState_Machine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
}

CState_Machine::CState_Machine(const CState_Machine& rhs)
{
}

void CState_Machine::Add_State(_uint iState, CState* pAddState)
{
	if (nullptr == pAddState)
		return;

	m_States.emplace(iState, pAddState);
}

void CState_Machine::Update_State(_float fTimeDelta)
{
	if (pCurrent_State == nullptr)
		return;

	pCurrent_State->OnStateUpdate(fTimeDelta);
}

void CState_Machine::Change_State(_uint iState)
{
	CState* pNextState = Find_State(iState);
	if (nullptr == pNextState)
		return;

	if (pCurrent_State == pNextState)
		return;

	// 이전 상태 종료
	if (pCurrent_State != nullptr)
		pCurrent_State->OnStateExit();

	// 현재 상태 변경
	Safe_Release(pCurrent_State);
	pCurrent_State = pNextState;
	Safe_AddRef(pCurrent_State);

	// 새로운 상태 진입
	pCurrent_State->OnStateEnter();
}

HRESULT CState_Machine::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CState_Machine::Initialize(void* pArg)
{
	return S_OK;
}

CState* CState_Machine::Find_State(_uint iState)
{
	auto iter = m_States.find(iState);
	if (iter == m_States.end()) 
		return nullptr;

	return iter->second;
}

CState_Machine* CState_Machine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CState_Machine* pInstance = new CState_Machine(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed To Create : CState_Machine"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

CState_Machine* CState_Machine::Clone(void* pArg)
{
	CState_Machine* pInstance = new CState_Machine(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed To Clone : CState_Machine"));

		Safe_Release(pInstance);
	}

	return pInstance;
}

void CState_Machine::Free()
{
	__super::Free();

	Safe_Release(pCurrent_State);
	for (auto& pstate : m_States)
		Safe_Release(pstate.second);

	m_States.clear();
}