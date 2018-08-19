float3 getCoordFromGrid(float2 uv, int gridDim, int texDim)
{
    float f = float(texDim) / float(gridDim);
    float3 uvw = float3(0, 0, 0);
    uvw.xy = frac(uv * f);
    float2 xy = floor(uv * f);
    uvw.z = (xy.x + f * xy.y) / gridDim;
    return saturate(uvw);
}

float2 getGridCoordFromPos(float3 uvw, int gridDim, int texDim)
{
    float s = float(texDim) / float(gridDim);
    float s2 = s * s;
    float z = floor(uvw.z * s2);
    float2 zo = float2(frac(z / s), floor(z / s) / s);
    float2 xy = floor(uvw.xy * s2) / s2;
    float2 uv = zo + (xy + (.5 / s2)) / s;
    return uv;
}
