// this might come in handy later on
// https://gist.github.com/patriciogonzalezvivo/670c22f3966e662d2f83

float rand(float2 n) {
	return frac(sin(dot(n, float2(12.9898, 4.1414))) * 43758.5453);
}

float noise(float2 p) {
	float2 ip = floor(p);
	float2 u = frac(p);
	u = u*u*(3.0 - 2.0*u);

	float res = lerp(
		lerp(rand(ip), rand(ip + float2(1.0, 0.0)), u.x),
		lerp(rand(ip + float2(0.0, 1.0)), rand(ip + float2(1.0, 1.0)), u.x), u.y);
	return res*res;
}

float3 noise3(float2 p) {
	return float3(
		noise(p + float2(333, 367)*3.),
		noise(p + float2(343, 347)*5.),
		noise(p + float2(363, 467)*9.)
		);
}

//  
matrix mat3row(float3 row1, float3 row2, float3 row3) {
	return
		matrix(
			row1.x, row1.y, row1.z, 0.,
			row2.x, row2.y, row2.z, 0.,
			row3.x, row3.y, row3.z, 0.,
			0., 0., 0., 1.
			);
}


matrix mat3col(float3 col1, float3 col2, float3 col3) {
	return
		matrix(
			col1.x, col2.y, col3.z, 0.,
			col1.x, col2.y, col3.z, 0.,
			col1.x, col2.y, col3.z, 0.,
			0., 0., 0., 1.
			);
}
