#include "Easing.h"

IMPLEMENT_SINGLETON(CEasing)

CEasing::CEasing()
{
}

_float CEasing::Get_Ease(EASE eEase, _float fStart, _float fEnd, _float fValue)
{
	return Find_Ease(eEase, fStart, fEnd, fValue);
}

_float CEasing::EaseInQuad(_float start, _float end, _float value)
{
	end -= start;
	return end * value * value + start;
}

_float CEasing::EaseOutQuad(_float start, _float end, _float value)
{
	end -= start;
	return -end * value * (value - 2) + start;
}

_float CEasing::EaseInOutQuad(_float start, _float end, _float value)
{
	value /= .5f;
	end -= start;
	if (value < 1) return end * 0.5f * value * value + start;
	value--;
	return -end * 0.5f * (value * (value - 2) - 1) + start;
}

_float CEasing::EaseInCubic(_float start, _float end, _float value)
{
	end -= start;
	return end * value * value * value + start;
}

_float CEasing::EaseOutCubic(_float start, _float end, _float value)
{
	value--;
	end -= start;
	return end * (value * value * value + 1) + start;
}

_float CEasing::EaseInOutCubic(_float start, _float end, _float value)
{
	value /= .5f;
	end -= start;
	if (value < 1) return end * 0.5f * value * value * value + start;
	value -= 2;
	return end * 0.5f * (value * value * value + 2) + start;
}

_float CEasing::EaseInQuart(_float start, _float end, _float value)
{
	end -= start;
	return end * value * value * value * value + start;
}

_float CEasing::EaseOutQuart(_float start, _float end, _float value)
{
	value--;
	end -= start;
	return -end * (value * value * value * value - 1) + start;
}

_float CEasing::EaseInOutQuart(_float start, _float end, _float value)
{
	value /= .5f;
	end -= start;
	if (value < 1) return end * 0.5f * value * value * value * value + start;
	value -= 2;
	return -end * 0.5f * (value * value * value * value - 2) + start;
}

_float CEasing::EaseInQuint(_float start, _float end, _float value)
{
	end -= start;
	return end * value * value * value * value * value + start;
}

_float CEasing::EaseOutQuint(_float start, _float end, _float value)
{
	value--;
	end -= start;
	return end * (value * value * value * value * value + 1) + start;
}

_float CEasing::EaseInOutQuint(_float start, _float end, _float value)
{
	value /= .5f;
	end -= start;
	if (value < 1) return end * 0.5f * value * value * value * value * value + start;
	value -= 2;
	return end * 0.5f * (value * value * value * value * value + 2) + start;
}

_float CEasing::EaseInSine(_float start, _float end, _float value)
{
	end -= start;
	return -end * cosf(value * (XM_PI * 0.5f)) + end + start;
}

_float CEasing::EaseOutSine(_float start, _float end, _float value)
{
	end -= start;
	return end * sinf(value * (XM_PI * 0.5f)) + start;
}

_float CEasing::EaseInOutSine(_float start, _float end, _float value)
{
	end -= start;
	return -end * 0.5f * (cosf(XM_PI* value) - 1) + start;
}

_float CEasing::EaseInExpo(_float start, _float end, _float value)
{
	end -= start;
	return end * pow(2, 10 * (value - 1)) + start;
}

_float CEasing::EaseOutExpo(_float start, _float end, _float value)
{
	end -= start;
	return end * (-pow(2, -10 * value) + 1) + start;
}

_float CEasing::EaseInOutExpo(_float start, _float end, _float value)
{
	value /= .5f;
	end -= start;
	if (value < 1) return end * 0.5f * pow(2, 10 * (value - 1)) + start;
	value--;
	return end * 0.5f * (-pow(2, -10 * value) + 2) + start;
}

_float CEasing::EaseInCirc(_float start, _float end, _float value)
{
	end -= start;
	return -end * (sqrt(1 - value * value) - 1) + start;
}

_float CEasing::EaseOutCirc(_float start, _float end, _float value)
{
	value--;
	end -= start;
	return end * sqrt(1 - value * value) + start;
}

_float CEasing::EaseInOutCirc(_float start, _float end, _float value)
{
	value /= .5f;
	end -= start;
	if (value < 1) return -end * 0.5f * (sqrt(1 - value * value) - 1) + start;
	value -= 2;
	return end * 0.5f * (sqrt(1 - value * value) + 1) + start;
}

_float CEasing::Linear(_float start, _float end, _float value)
{
	_vector vStart = { start, start, start };
	_vector vEnd = { end, end, end };
	return XMVectorLerp(vStart, vEnd, value).m128_f32[0];
}

_float CEasing::EaseInBounce(_float start, _float end, _float value)
{
	end -= start;
	float d = 1.f;
	return end - EaseOutBounce(0, end, d - value) + start;
}

_float CEasing::EaseOutBounce(_float start, _float end, _float value)
{
	value /= 1.f;
	end -= start;
	if (value < (1 / 2.75f))
	{
		return end * (7.5625f * value * value) + start;
	}
	else if (value < (2 / 2.75f))
	{
		value -= (1.5f / 2.75f);
		return end * (7.5625f * (value)*value + .75f) + start;
	}
	else if (value < (2.5 / 2.75))
	{
		value -= (2.25f / 2.75f);
		return end * (7.5625f * (value)*value + .9375f) + start;
	}
	else
	{
		value -= (2.625f / 2.75f);
		return end * (7.5625f * (value)*value + .984375f) + start;
	}
}

_float CEasing::EaseInOutBounce(_float start, _float end, _float value)
{
	end -= start;
	float d = 1.f;
	if (value < d * 0.5f) return EaseInBounce(0, end, value * 2) * 0.5f + start;
	else return EaseOutBounce(0, end, value * 2 - d) * 0.5f + end * 0.5f + start;
}

_float CEasing::EaseInBack(_float start, _float end, _float value)
{
	end -= start;
	value /= 1;
	float s = 1.70158f;
	return end * (value)*value * ((s + 1) * value - s) + start;
}

_float CEasing::EaseOutBack(_float start, _float end, _float value)
{
	float s = 1.70158f;
	end -= start;
	value = (value)-1;
	return end * ((value)*value * ((s + 1) * value + s) + 1) + start;
}

_float CEasing::EaseInOutBack(_float start, _float end, _float value)
{
	float s = 1.70158f;
	end -= start;
	value /= .5f;
	if ((value) < 1)
	{
		s *= (1.525f);
		return end * 0.5f * (value * value * (((s)+1) * value - s)) + start;
	}
	value -= 2;
	s *= (1.525f);
	return end * 0.5f * ((value)*value * (((s)+1) * value + s) + 2) + start;
}

_float CEasing::EaseInElastic(_float start, _float end, _float value)
{
	end -= start;

	float d = 1.f;
	float p = d * .3f;
	float s;
	float a = 0;

	if (value == 0) return start;

	if ((value /= d) == 1) return start + end;

	if (a == 0.f || a < abs(end))
	{
		a = end;
		s = p / 4;
	}
	else
	{
		s = p / (2 * XM_PI) * asinf(end / a);
	}

	return -(a * pow(2, 10 * (value -= 1)) * sinf((value * d - s) * (2 * XM_PI) / p)) + start;
}

_float CEasing::EaseOutElastic(_float start, _float end, _float value)
{
	end -= start;

	float d = 1.f;
	float p = d * .3f;
	float s;
	float a = 0;

	if (value == 0) return start;

	if ((value /= d) == 1) return start + end;

	if (a == 0.f || a < abs(end))
	{
		a = end;
		s = p * 0.25f;
	}
	else
	{
		s = p / (2 * XM_PI) * asinf(end / a);
	}

	return (a * pow(2, -10 * value) * sinf((value * d - s) * (2 * XM_PI) / p) + end + start);
}

_float CEasing::EaseInOutElastic(_float start, _float end, _float value)
{
	end -= start;

	float d = 1.f;
	float p = d * .3f;
	float s;
	float a = 0;

	if (value == 0) return start;

	if ((value /= d * 0.5f) == 2) return start + end;

	if (a == 0.f || a < abs(end))
	{
		a = end;
		s = p / 4;
	}
	else
	{
		s = p / (2 * XM_PI) * asinf(end / a);
	}

	if (value < 1) return -0.5f * (a * pow(2, 10 * (value -= 1)) * sinf((value * d - s) * (2 * XM_PI) / p)) + start;
	return a * pow(2, -10 * (value -= 1)) * sinf((value * d - s) * (2 * XM_PI) / p) * 0.5f + end + start;
}

_float CEasing::Find_Ease(EASE eEase, _float fStart, _float fEnd, _float fValue)
{
	switch (eEase)
	{
	case Engine::CEasing::Ease_InQuad:
		return EaseInQuad(fStart, fEnd, fValue);
	case Engine::CEasing::Ease_OutQuad:
		return EaseOutQuad(fStart, fEnd, fValue);
	case Engine::CEasing::Ease_InOutQuad:
		return EaseInOutQuad(fStart, fEnd, fValue);
	case Engine::CEasing::Ease_InCubic:
		return EaseInCubic(fStart, fEnd, fValue);
	case Engine::CEasing::Ease_OutCubic:
		return EaseOutCubic(fStart, fEnd, fValue);
	case Engine::CEasing::Ease_InOutCubic:
		return EaseInOutCubic(fStart, fEnd, fValue);
	case Engine::CEasing::Ease_InQuart:
		return EaseInQuart(fStart, fEnd, fValue);
	case Engine::CEasing::Ease_OutQuart:
		return EaseOutQuart(fStart, fEnd, fValue);
	case Engine::CEasing::Ease_InOutQuart:
		return EaseInOutQuart(fStart, fEnd, fValue);
	case Engine::CEasing::Ease_InQuint:
		return EaseInQuint(fStart, fEnd, fValue);
	case Engine::CEasing::Ease_OutQuint:
		return EaseOutQuint(fStart, fEnd, fValue);
	case Engine::CEasing::Ease_InOutQuint:
		return EaseInOutQuint(fStart, fEnd, fValue);
	case Engine::CEasing::Ease_InSine:
		return EaseInSine(fStart, fEnd, fValue);
	case Engine::CEasing::Ease_OutSine:
		return EaseOutSine(fStart, fEnd, fValue);
	case Engine::CEasing::Ease_InOutSine:
		return EaseInOutSine(fStart, fEnd, fValue);
	case Engine::CEasing::Ease_InExpo:
		return EaseInExpo(fStart, fEnd, fValue);
	case Engine::CEasing::Ease_OutExpo:
		return EaseOutExpo(fStart, fEnd, fValue);
	case Engine::CEasing::Ease_InOutExpo:
		return EaseInOutExpo(fStart, fEnd, fValue);
	case Engine::CEasing::Ease_InCirc:
		return EaseInCirc(fStart, fEnd, fValue);
	case Engine::CEasing::Ease_OutCirc:
		return EaseOutCirc(fStart, fEnd, fValue);
	case Engine::CEasing::Ease_InOutCirc:
		return EaseInOutCirc(fStart, fEnd, fValue);
	case Engine::CEasing::Ease_Linear:
		return Linear(fStart, fEnd, fValue);
	case Engine::CEasing::Ease_InBounce:
		return EaseInBounce(fStart, fEnd, fValue);
	case Engine::CEasing::Ease_OutBounce:
		return EaseOutBounce(fStart, fEnd, fValue);
	case Engine::CEasing::Ease_InOutBounce:
		return EaseInOutBounce(fStart, fEnd, fValue);
	case Engine::CEasing::Ease_InBack:
		return EaseInBack(fStart, fEnd, fValue);
	case Engine::CEasing::Ease_OutBack:
		return EaseOutBack(fStart, fEnd, fValue);
	case Engine::CEasing::Ease_InOutBack:
		return EaseInOutBack(fStart, fEnd, fValue);
	case Engine::CEasing::Ease_InElastic:
		return EaseInElastic(fStart, fEnd, fValue);
	case Engine::CEasing::Ease_OutElastic:
		return EaseOutElastic(fStart, fEnd, fValue);
	case Engine::CEasing::Ease_InOutElastic:
		return EaseInOutElastic(fStart, fEnd, fValue);
	case Engine::CEasing::Ease_END:
		break;
	default:
		break;
	}
}

void CEasing::Free()
{
}
