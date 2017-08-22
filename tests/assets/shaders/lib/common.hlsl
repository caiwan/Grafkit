float noise(float2 uv) {
	return frac(sin(dot(uv.xy, float2(12.9898, 78.233))) * 43758.5453);
}

float3 noise3(float2 uv) {
	return 2. * float3(
		noise(uv + float2(.34986, .93485)),
		noise(uv + float2(.92756, .29338)),
		noise(uv + float2(.439578, 403875))
		) - 1.;
}
