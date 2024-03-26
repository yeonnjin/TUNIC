#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CEasing final : public CBase
{
	DECLARE_SINGLETON(CEasing)

private:
	CEasing();
	virtual ~CEasing() = default;

public:
    enum EASE {
        Ease_InQuad = 0, Ease_OutQuad, Ease_InOutQuad, Ease_InCubic, Ease_OutCubic,
        Ease_InOutCubic = 5, Ease_InQuart, Ease_OutQuart, Ease_InOutQuart, Ease_InQuint,
        Ease_OutQuint = 10, Ease_InOutQuint, Ease_InSine, Ease_OutSine, Ease_InOutSine,
        Ease_InExpo = 15, Ease_OutExpo, Ease_InOutExpo, Ease_InCirc, Ease_OutCirc,
        Ease_InOutCirc = 20, Ease_Linear, Ease_InBounce, Ease_OutBounce, Ease_InOutBounce, 
        Ease_InBack = 25, Ease_OutBack, Ease_InOutBack, Ease_InElastic,
        Ease_OutElastic = 30, Ease_InOutElastic, Ease_END
    };

public:
    _float Get_Ease(EASE eEase, _float fStart, _float fEnd, _float fValue);

private:
    _float Find_Ease(EASE eEase, _float fStart, _float fEnd, _float fValue);

private:
    _float EaseInQuad(_float start, _float end, _float value);
    _float EaseOutQuad(_float start, _float end, _float value);
    _float EaseInOutQuad(_float start, _float end, _float value);
    _float EaseInCubic(_float start, _float end, _float value);
    _float EaseOutCubic(_float start, _float end, _float value);

    _float EaseInOutCubic(_float start, _float end, _float value);
    _float EaseInQuart(_float start, _float end, _float value);
    _float EaseOutQuart(_float start, _float end, _float value);
    _float EaseInOutQuart(_float start, _float end, _float value);
    _float EaseInQuint(_float start, _float end, _float value);

    _float EaseOutQuint(_float start, _float end, _float value);
    _float EaseInOutQuint(_float start, _float end, _float value);
    _float EaseInSine(_float start, _float end, _float value);
    _float EaseOutSine(_float start, _float end, _float value);
    _float EaseInOutSine(_float start, _float end, _float value);

    _float EaseInExpo(_float start, _float end, _float value);
    _float EaseOutExpo(_float start, _float end, _float value);
    _float EaseInOutExpo(_float start, _float end, _float value);
    _float EaseInCirc(_float start, _float end, _float value);
    _float EaseOutCirc(_float start, _float end, _float value);

    _float EaseInOutCirc(_float start, _float end, _float value);
    _float Linear(_float start, _float end, _float value);
    _float EaseInBounce(_float start, _float end, _float value);
    _float EaseOutBounce(_float start, _float end, _float value);
    _float EaseInOutBounce(_float start, _float end, _float value);

    _float EaseInBack(_float start, _float end, _float value);
    _float EaseOutBack(_float start, _float end, _float value);
    _float EaseInOutBack(_float start, _float end, _float value);
    _float EaseInElastic(_float start, _float end, _float value);
    _float EaseOutElastic(_float start, _float end, _float value);

    _float EaseInOutElastic(_float start, _float end, _float value);

public:
	virtual void Free() override;
};

END