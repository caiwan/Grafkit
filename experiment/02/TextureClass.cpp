#include "TextureClass.h"

using namespace DirectX;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#undef STB_IMAGE_IMPLEMENTATION

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"
#undef STB_IMAGE_RESIZE_IMPLEMENTATION



namespace {

}

TextureClass::TextureClass() : 
	m_tex(0),
	m_texture_resource(0)
{
}

TextureClass::TextureClass(const TextureClass &)
{
}

TextureClass::~TextureClass()
{
}


bool TextureClass::Initialize(ID3D11Device* device, WCHAR* filename)
{
	HRESULT result = 0;

	// filenev elokeszitese wchar -> utf8 az stbi miatt
	size_t filename_len = wcslen(filename); 
	char filename_utf8[FILENAME_MAX]; 
	ZeroMemory(filename_utf8, FILENAME_MAX);
	if (WideCharToMultiByte(CP_UTF8, 0, filename, filename_len, filename_utf8, FILENAME_MAX, NULL, FALSE) == 0)
	{
		///@todo throw exception
		return false;
	}

	int x = 0, y = 0, ch = 0;
	// kikenyszeritett RGBA mod
	UCHAR *data = stbi_load(filename_utf8, &x, &y, &ch, 0);

	if (!data)
	{
		///@todo throw exception
		return false;
	}

	// fill etexture description
	D3D11_TEXTURE2D_DESC desc; ZeroMemory(&desc, sizeof(desc));
	
	desc.Width = x;
	desc.Height = y;
	desc.MipLevels = 1; // 0 = general magatol mipmapet
	desc.ArraySize = 1; // multitextura szama, cubemap eseten n*6

	switch (ch) {
		case 1: desc.Format = DXGI_FORMAT_R8_UNORM; break; 
		case 2: desc.Format = DXGI_FORMAT_R8G8_UNORM; break; 
		case 3: 
		{
			UINT* newdata = new UINT[x*y];
			UCHAR* dst = (UCHAR*)newdata;
			UCHAR* src = (UCHAR*)data;
# if 1

			size_t wx = x;
			size_t wy = y;

			size_t stride_src = wx * 3;
			size_t stride_dst = wx * 4;
			
			while (wy--) {
				while (wx--) {
					dst[4 * wx + 0] = src[3 * wx + 0];
					dst[4 * wx + 1] = src[3 * wx + 1];
					dst[4 * wx + 2] = src[3 * wx + 2];
					dst[4 * wx + 3] = 0xff;
				}
				src += stride_src;
				dst += stride_dst;
				wx = x;
			}
#else 
			for (size_t wy = 0; wy < y; wy++)
			{
				for (size_t wx = 0; wx < x; wx++)
				{
					size_t p = (wx + x * wy);
					dst[4 * p + 0] = wx % 255;
					dst[4 * p + 1] = wy % 255;
					dst[4 * p + 2] = 0x00;
					dst[4 * p + 3] = 0xff;
				}
			}
#endif 

			ch = 4;

			delete data;
			data = (UCHAR*)newdata;

			desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		} break;

		case 4: desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; break; 
		
		default: return false;
	}

	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;	
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

#define N 1
	D3D11_SUBRESOURCE_DATA *initialData = new D3D11_SUBRESOURCE_DATA[N];
	ZeroMemory(initialData, N * sizeof(initialData));
	for (size_t i = 0; i < N; i++)
	{
		initialData[i].pSysMem = data;
		initialData[i].SysMemPitch = x * ch;
		initialData[i].SysMemSlicePitch = 0;

	}
#undef N

	result = device->CreateTexture2D(&desc, initialData, &m_tex);

	if (FAILED(result)) {
		delete data;
		delete[] initialData;
		return false;
	}

	delete data;
	delete[] initialData;


	// --- shderresourceref
	D3D11_SHADER_RESOURCE_VIEW_DESC resDesc;
	ZeroMemory(&resDesc, sizeof(resDesc));

	resDesc.Format = desc.Format;
	resDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	resDesc.Texture2D.MostDetailedMip = 0;
	resDesc.Texture2D.MipLevels = 1;

	result = device->CreateShaderResourceView(&m_tex[0], &resDesc, &m_texture_resource);
	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void TextureClass::Shutdown()
{
	if (m_texture_resource) {
		m_texture_resource->Release();
		m_texture_resource = 0;
	}

	// @todo m_tex torlese 
}
