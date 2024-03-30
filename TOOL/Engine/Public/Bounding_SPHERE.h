#pragma once

#include "Bounding.h"

BEGIN(Engine)

class CBounding_SPHERE final : public CBounding
{
public:
	typedef struct Bounding_SPHERE_Desc : public CBounding::BOUNDING_DESC
	{
		_float	fRadius;
	}BOUNDING_SPHERE_DESC;

private:
	CBounding_SPHERE(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CBounding_SPHERE() = default;

public:
	virtual void*	Get_BoundingDesc() { return m_pBoundingDesc; }

public:
	virtual HRESULT Initialize(CBounding::BOUNDING_DESC* pBoundingDesc) override;
	virtual void	Tick(_fmatrix WorldMatrix) override;
	virtual _bool	Check_Collision(CCollider::TYPE eType, CBounding* pBounding) override;

#ifdef _DEBUG
public:
	virtual HRESULT Render(PrimitiveBatch<VertexPositionColor>* pBatch);
#endif // _DEBUG

private:
	BoundingSphere* m_pBoundingDesc_Origin = { nullptr };
	BoundingSphere*	m_pBoundingDesc = { nullptr };

public:
	static CBounding_SPHERE* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CBounding::BOUNDING_DESC* pBoundingDesc);
	virtual void Free() override;
};

END