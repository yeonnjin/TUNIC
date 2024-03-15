#pragma once

#include "Component.h"

/* 
애니메이션 클립, 애니메이터 컨트롤러, 아바타는 모두 애니메이터(Animator) 컴포넌트를 통해 게임 오브젝트에서 결합
애니메이터 컨트롤러와 (필요한 경우) 해당 모델의 아바타에 대한 레퍼런스가 있습니다. 
애니메이터 컨트롤러에도 컨트롤러가 사용하는 애니메이션 클립에 대한 레퍼런스가 포함
애니메이션을 제어하는 애니메이터 컨트롤러 에셋에 대한 참조를 포함
*/

BEGIN(Engine)

class ENGINE_DLL CAnimator final : public CComponent
{
private:
	CAnimator(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CAnimator(const CAnimator& rhs);
	virtual ~CAnimator() = default;

public:
	// void	Set_Animation(_uint iAnimIndex, _bool isLoop); 

public:
	// HRESULT Play_Animation(_float fTimeDelta); 
	// 
	// 1. 모델 로드


private: /* 프로퍼티 */
	// 1. Controller : 캐릭터에 부여된 애니메이터 컨트롤러
	// 2. Apply Root Motion : 캐릭터의 포지션과 회전을 애니메이션 자체에서 제어할지 또는 스크립트에서 제어할지 여부
	// 3. Update Mode : 애니메이터 업데이트를 언제 할지, 어떤 타임스케일을 사용할지 선택할 수 있게 해줍니다.
	//		1) Normal : 애니메이터는 업데이트 호출과 싱크되어 업데이트되며, 애니메이터의 속도는 현재의 타임스케일과 일치합니다. 타임스케일이 느려지면 애니메이션도 따라서 느려집니다.
	//		2) Unscaled Time : 애니메이터는 업데이트 호출에 따라 업데이트되지만 애니메이터의 속도는 현재의 타임스케일을 무시하고 항상 100%의 속도로 애니메이션화합니다. 
	//							게임플레이를 일시정지하거나 특수한 효과를 위해 수정된 타임스케일을 사용하면서 GUI 시스템을 노멀 속도로 애니메이션화하는 경우에 유용합니다.
	
private: /* 애니메이션 커브 정보 : Animator 컴포넌트의 하단에 있는 인포메이션 박스에는 애니메이터 컨트롤러가 사용하는 모든 클립의 상세한 데이터가 표시됩니다. 
									애니메이션 클립은 시간 흐름에 따라 값이 어떻게 변하는지 나타내는 “커브” 형식의 데이터를 포함합니다.*/
	// 1. Clip Count : 해당 애니메이터에 할당된 애니메이터 컨트롤러가 사용하는 애니메이션 클립의 총 개수.
	// 2. Curves (Pos, Rot & Scale) : 오브젝트의 포지션, 회전, 스케일을 애니메이션화하는 데 사용하는 커브의 총 개수. 
	//									일반적인 휴머노이드 리그에 포함되지 않는 애니메이션화한 오브젝트의 커브를 의미합니다. 
	//									휴머노이드 아바타를 애니메이션화했을 때 꼬리라든지 흩날리는 천 또는 달랑거리는 펜던트와 같은 추가적인 비근육 뼈대의 개수를 표시합니다. 
	// 3. Curves Count : 애니메이션 커브의 전체 개수 합계			




public:
	static CAnimator* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END