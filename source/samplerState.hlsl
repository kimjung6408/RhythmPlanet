
SamplerState samLinear
{
	Filter=MIN_MAG_MIP_LINEAR;
	AddressU=Wrap;
	AddressV=Wrap;
};

//shadow mapping¿¡ ÇÊ¿ä.
SamplerComparisonState samShadow
{
	Filter=COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
	AddressU=BORDER;
	AddressV=BORDER;
	AddressW=BORDER;
	BorderColor=float4(0.0f,0.0f,0.0f,0.0f);

	ComparisonFunc=LESS_EQUAL;
};

