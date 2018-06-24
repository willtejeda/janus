#include "SlugDemo.h"


#if SLUG_OPENGL

	extern const char				*rendererDescription;

	// Define pointers to the GL functions we need that aren't in opengl32.lib.

	#undef GLEXTFUNC
	#define GLEXTFUNC(type, name, params) type (*name)params = nullptr;
	#include "GLExtensions.h"

#elif SLUG_D3D11

	extern ID3D11Device				*d3dDevice;
	extern ID3D11DeviceContext		*d3dContext;
	extern ID3D11RenderTargetView	*d3dTargetView;

	extern ID3DBlob					*vertexBlob[Slug::kMaxVertexShaderCount];
	extern ID3DBlob					*pixelBlob[Slug::kMaxPixelShaderCount];

	extern char						rendererDescription[];

#endif


using namespace Demo;


Matrix4D::Matrix4D(float n00, float n01, float n02, float n03, float n10, float n11, float n12, float n13, float n20, float n21, float n22, float n23, float n30, float n31, float n32, float n33)
{
	n[0][0] = n00; n[1][0] = n01; n[2][0] = n02; n[3][0] = n03;
	n[0][1] = n10; n[1][1] = n11; n[2][1] = n12; n[3][1] = n13;
	n[0][2] = n20; n[1][2] = n21; n[2][2] = n22; n[3][2] = n23;
	n[0][3] = n30; n[1][3] = n31; n[2][3] = n32; n[3][3] = n33;
}

Transform4D::Transform4D(const Vector3D& r0, float n03, const Vector3D& r1, float n13, const Vector3D& r2, float n23)
{
	n[0][0] = r0.x; n[1][0] = r0.y; n[2][0] = r0.z; n[3][0] = n03;
	n[0][1] = r1.x; n[1][1] = r1.y; n[2][1] = r1.z; n[3][1] = n13;
	n[0][2] = r2.x; n[1][2] = r2.y; n[2][2] = r2.z; n[3][2] = n23;

	n[0][3] = n[1][3] = n[2][3] = 0.0F;
	n[3][3] = 1.0F;
}

Matrix4D Demo::operator *(const Matrix4D& m1, const Matrix4D& m2)
{
	return (Matrix4D(m1(0,0) * m2(0,0) + m1(0,1) * m2(1,0) + m1(0,2) * m2(2,0) + m1(0,3) * m2(3,0),
					 m1(0,0) * m2(0,1) + m1(0,1) * m2(1,1) + m1(0,2) * m2(2,1) + m1(0,3) * m2(3,1),
					 m1(0,0) * m2(0,2) + m1(0,1) * m2(1,2) + m1(0,2) * m2(2,2) + m1(0,3) * m2(3,2),
					 m1(0,0) * m2(0,3) + m1(0,1) * m2(1,3) + m1(0,2) * m2(2,3) + m1(0,3) * m2(3,3),
					 m1(1,0) * m2(0,0) + m1(1,1) * m2(1,0) + m1(1,2) * m2(2,0) + m1(1,3) * m2(3,0),
					 m1(1,0) * m2(0,1) + m1(1,1) * m2(1,1) + m1(1,2) * m2(2,1) + m1(1,3) * m2(3,1),
					 m1(1,0) * m2(0,2) + m1(1,1) * m2(1,2) + m1(1,2) * m2(2,2) + m1(1,3) * m2(3,2),
					 m1(1,0) * m2(0,3) + m1(1,1) * m2(1,3) + m1(1,2) * m2(2,3) + m1(1,3) * m2(3,3),
					 m1(2,0) * m2(0,0) + m1(2,1) * m2(1,0) + m1(2,2) * m2(2,0) + m1(2,3) * m2(3,0),
					 m1(2,0) * m2(0,1) + m1(2,1) * m2(1,1) + m1(2,2) * m2(2,1) + m1(2,3) * m2(3,1),
					 m1(2,0) * m2(0,2) + m1(2,1) * m2(1,2) + m1(2,2) * m2(2,2) + m1(2,3) * m2(3,2),
					 m1(2,0) * m2(0,3) + m1(2,1) * m2(1,3) + m1(2,2) * m2(2,3) + m1(2,3) * m2(3,3),
					 m1(3,0) * m2(0,0) + m1(3,1) * m2(1,0) + m1(3,2) * m2(2,0) + m1(3,3) * m2(3,0),
					 m1(3,0) * m2(0,1) + m1(3,1) * m2(1,1) + m1(3,2) * m2(2,1) + m1(3,3) * m2(3,1),
					 m1(3,0) * m2(0,2) + m1(3,1) * m2(1,2) + m1(3,2) * m2(2,2) + m1(3,3) * m2(3,2),
					 m1(3,0) * m2(0,3) + m1(3,1) * m2(1,3) + m1(3,2) * m2(2,3) + m1(3,3) * m2(3,3)));
}

Transform4D Demo::Inverse(const Transform4D& m)
{
	// See FGED1, pages 49-50 and 80.

	const Vector3D& a = m[0];
	const Vector3D& b = m[1];
	const Vector3D& c = m[2];
	const Vector3D& d = m[3];

	Vector3D s = Cross(a, b);
	Vector3D t = Cross(c, d);

	float invDet = 1.0F / Dot(s, c);

	s *= invDet;
	t *= invDet;
	Vector3D v = c * invDet;

	return (Transform4D(Cross(b, v), -Dot(b, t), Cross(v, a), Dot(a, t), s, -Dot(d, s)));
}


Demo::Font::Font(const char *fileName)
{
	#if SLUG_WINDOWS

		LARGE_INTEGER	fileSize;
		DWORD			actual;

		HANDLE fileHandle = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
		GetFileSizeEx(fileHandle, &fileSize);
		unsigned_int64 size = fileSize.QuadPart;

	#elif SLUG_MACOS || SLUG_LINUX

		struct stat		stat;

		int fileDesc = open(fileName, O_RDONLY, 0);
		fstat(fileDesc, &stat);
		unsigned_int64 size = stat.st_size;

	#endif

	// Allocate enough space for the file contents plus another 63 bytes of padding
	// that might be needed to align the data on a 64-byte boundary.

	dataStorage = new char[size + 0x3F];
	slugFile = dataStorage + (-reinterpret_cast<int64>(dataStorage) & 0x3F);

	#if SLUG_WINDOWS

		ReadFile(fileHandle, slugFile, DWORD(size), &actual, nullptr);
		CloseHandle(fileHandle);

	#elif SLUG_MACOS || SLUG_LINUX

		read(fileDesc, slugFile, size);
		close(fileDesc);

	#endif

	// Extract the curve and band textures from the slug file data and create
	// the texture objects that will be used by the shaders.

	const Slug::FontHeader *fontHeader = GetFontHeader();

	int32 curveTextureWidth = fontHeader->curveTextureSize[0];
	int32 curveTextureHeight = fontHeader->curveTextureSize[1];
	Slug::FontTexel *curveTextureData = new Slug::FontTexel[curveTextureWidth * curveTextureHeight];

	int32 bandTextureWidth = fontHeader->bandTextureSize[0];
	int32 bandTextureHeight = fontHeader->bandTextureSize[1];
	Slug::FontTexel *bandTextureData = new Slug::FontTexel[bandTextureWidth * bandTextureHeight];

	Slug::ExtractFontTextures(fontHeader, curveTextureData, bandTextureData);

	#if SLUG_OPENGL

		glGenTextures(2, textureName);

		glBindTexture(GL_TEXTURE_RECTANGLE, textureName[0]);
		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA16F, curveTextureWidth, curveTextureHeight, 0, GL_RGBA, GL_HALF_FLOAT, curveTextureData);

		glBindTexture(GL_TEXTURE_RECTANGLE, textureName[1]);
		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA16UI, bandTextureWidth, bandTextureHeight, 0, GL_RGBA_INTEGER, GL_UNSIGNED_SHORT, bandTextureData);

		// The OpenGL spec requires that the filtering modes for integer textures be set to GL_NEAREST,
		// or else the results of a texture fetch are undefined. Nvidia and AMD drivers still return the
		// expected texel values, but the Intel driver returns zeros if the default modes aren't changed.

		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	#elif SLUG_D3D11

		D3D11_TEXTURE2D_DESC				textureDesc;
		D3D11_SUBRESOURCE_DATA				subresourceData;
		D3D11_SHADER_RESOURCE_VIEW_DESC		resourceViewDesc;

		textureDesc.Width = curveTextureWidth;
		textureDesc.Height = curveTextureHeight;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_IMMUTABLE;
		textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		subresourceData.pSysMem = curveTextureData;
		subresourceData.SysMemPitch = curveTextureWidth * 8;
		subresourceData.SysMemSlicePitch = 0;

		resourceViewDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		resourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		resourceViewDesc.Texture2D.MostDetailedMip = 0;
		resourceViewDesc.Texture2D.MipLevels = 1;

		d3dDevice->CreateTexture2D(&textureDesc, &subresourceData, &textureObject[0]);
		d3dDevice->CreateShaderResourceView(textureObject[0], &resourceViewDesc, &textureView[0]);

		textureDesc.Width = bandTextureWidth;
		textureDesc.Height = bandTextureHeight;
		textureDesc.Format = DXGI_FORMAT_R16G16B16A16_UINT;

		subresourceData.pSysMem = bandTextureData;
		subresourceData.SysMemPitch = bandTextureWidth * 8;

		resourceViewDesc.Format = DXGI_FORMAT_R16G16B16A16_UINT;

		d3dDevice->CreateTexture2D(&textureDesc, &subresourceData, &textureObject[1]);
		d3dDevice->CreateShaderResourceView(textureObject[1], &resourceViewDesc, &textureView[1]);

	#endif

	delete[] bandTextureData;
	delete[] curveTextureData;

	referenceCount = 1;
}

Demo::Font::~Font()
{
	#if SLUG_OPENGL

		glDeleteTextures(2, textureName);

	#elif SLUG_D3D11

		textureView[1]->Release();
		textureObject[1]->Release();
		textureView[0]->Release();
		textureObject[0]->Release();

	#endif

	delete[] dataStorage;
}


Text::Text(Font *font, const Slug::LayoutData *layoutData, const char *text, float x, float y)
{
	unsigned_int32		vertexIndex;
	unsigned_int32		pixelIndex;
	const char			*vertexCode[2];
	const char			*pixelCode[2];

	textFont = font;
	font->Retain();

	textLayoutData = *layoutData;

	// Set the object-to-world transform to a simple translation to the point (x,y,0).

	textTransform(0,0) = textTransform(1,1) = textTransform(2,2) = textTransform(3,3) = 1.0F;
	textTransform(0,1) = textTransform(0,2) = textTransform(1,0) = textTransform(1,2) = textTransform(2,0) = textTransform(2,1) = textTransform(3,0) = textTransform(3,1) = textTransform(3,2) = 0.0F;
	textTransform(0,3) = x;
	textTransform(1,3) = y;
	textTransform(2,3) = 0.0F;

	// Retrieve the shader code for this font/layout combination.

	const Slug::FontHeader *fontHeader = font->GetFontHeader();
	Slug::GetShaderIndices(fontHeader, layoutData, &vertexIndex, &pixelIndex);
	Slug::GetShaderSourceCode(&vertexCode[1], &pixelCode[1]);

	#if SLUG_OPENGL

		Slug::GetOpenGLShaderProlog(vertexIndex, pixelIndex, &vertexCode[0], &pixelCode[0]);

		// Compile and link shader program.

		vertexShaderName = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShaderName, 2, vertexCode, nullptr);
		glCompileShader(vertexShaderName);

		pixelShaderName = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(pixelShaderName, 2, pixelCode, nullptr);
		glCompileShader(pixelShaderName);

		programName = glCreateProgram();
		glAttachShader(programName, vertexShaderName);
		glAttachShader(programName, pixelShaderName);
		glLinkProgram(programName);
		glUseProgram(programName);

		// We always use texture units 0 and 1 for the curve and band textures.

		glUniform1i(glGetUniformLocation(programName, "curveTexture"), 0);
		glUniform1i(glGetUniformLocation(programName, "bandTexture"), 1);

		// Retrieve the location of the vertex shader uniform input that holds the MVP matrix.

		vparamLocation = glGetUniformLocation(programName, "vparam");

		// Generate names for the vertex and index buffers.

		glGenBuffers(2, bufferName);

	#elif SLUG_D3D11

		const char			*(*vertexDefines)[2];
		const char			*(*pixelDefines)[2];
		D3D11_BUFFER_DESC	bufferDesc;

		// Compile vertex shader if we haven't already done so for this index.

		Slug::GetD3D11ShaderDefines(vertexIndex, pixelIndex, &vertexDefines, &pixelDefines);

		ID3DBlob *vsBlob = vertexBlob[vertexIndex];
		if (!vsBlob)
		{
			D3DCompile(vertexCode[1], strlen(vertexCode[1]), nullptr, reinterpret_cast<D3D_SHADER_MACRO *>(vertexDefines), nullptr, "main", "vs_4_0", D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &vsBlob, nullptr);
			vertexBlob[vertexIndex] = vsBlob;
		}

		d3dDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vertexShaderObject);

		// Compile pixel shader if we haven't already done so for this index.

		ID3DBlob *psBlob = pixelBlob[pixelIndex];
		if (!psBlob)
		{
			D3DCompile(pixelCode[1], strlen(pixelCode[1]), nullptr, reinterpret_cast<D3D_SHADER_MACRO *>(pixelDefines), nullptr, "main", "ps_4_0", D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &psBlob, nullptr);
			pixelBlob[pixelIndex] = psBlob;
		}

		d3dDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &pixelShaderObject);

		// Create an input layout corresponding to the GlyphVertex structure.

		static const D3D11_INPUT_ELEMENT_DESC elementDesc[4] =
		{
			{"ATTRIB", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"ATTRIB", 1, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"ATTRIB", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"ATTRIB", 3, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		d3dDevice->CreateInputLayout(elementDesc, 4, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayoutObject);

		// Create a constant buffer for the MVP matrix used by the vertex shader.

		bufferDesc.ByteWidth = 64;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		d3dDevice->CreateBuffer(&bufferDesc, nullptr, &constantBufferObject);

		vertexBufferObject = nullptr;
		indexBufferObject = nullptr;

	#endif

	if (text)
	{
		Build(text);
	}
}

Text::~Text()
{
	#if SLUG_OPENGL

		glDeleteBuffers(2, bufferName);
		glDeleteProgram(programName);
		glDeleteShader(pixelShaderName);
		glDeleteShader(vertexShaderName);

	#elif SLUG_D3D11

		if (indexBufferObject) indexBufferObject->Release();
		if (vertexBufferObject) vertexBufferObject->Release();

		constantBufferObject->Release();
		inputLayoutObject->Release();
		pixelShaderObject->Release();
		vertexShaderObject->Release();

	#endif

	textFont->Release();
}

void Text::Build(const char *text)
{
	int32				maxVertexCount[2];
	int32				maxTriangleCount[2];
	Slug::GlyphBuffer	glyphBuffer[2];

	// Calculate the vertex and triangle counts for the text.

	maxVertexCount[1] = 0;
	maxTriangleCount[1] = 0;

	const Slug::FontHeader *fontHeader = textFont->GetFontHeader();
	Slug::MeasureSlug(fontHeader, &textLayoutData, text, nullptr, maxVertexCount, maxTriangleCount);

	int32 totalVertexCount = maxVertexCount[0] + maxVertexCount[1];
	int32 totalTriangleCount = maxTriangleCount[0] + maxTriangleCount[1];

	#if SLUG_OPENGL

		// Allocate space in the vertex and index buffers.

		glBindBuffer(GL_ARRAY_BUFFER, bufferName[0]);
		glBufferData(GL_ARRAY_BUFFER, totalVertexCount * sizeof(Slug::GlyphVertex), nullptr, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferName[1]);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, totalTriangleCount * sizeof(Slug::GlyphTriangle), nullptr, GL_STATIC_DRAW);

		// Map the vertex buffer and index buffer for writing.

		glyphBuffer[0].glyphVertex = static_cast<Slug::GlyphVertex *>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
		glyphBuffer[0].glyphTriangle = static_cast<Slug::GlyphTriangle *>(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));

	#elif SLUG_D3D11

		D3D11_BUFFER_DESC			bufferDesc;
		D3D11_MAPPED_SUBRESOURCE	vertexSubresource;
		D3D11_MAPPED_SUBRESOURCE	indexSubresource;

		if (indexBufferObject) indexBufferObject->Release();
		if (vertexBufferObject) vertexBufferObject->Release();

		// Allocate space in the vertex and index buffers.

		bufferDesc.ByteWidth = totalVertexCount * sizeof(Slug::GlyphVertex);
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		d3dDevice->CreateBuffer(&bufferDesc, nullptr, &vertexBufferObject);

		bufferDesc.ByteWidth = totalTriangleCount * sizeof(Slug::GlyphTriangle);
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.StructureByteStride = 0;

		d3dDevice->CreateBuffer(&bufferDesc, nullptr, &indexBufferObject);

		// Map the vertex buffer and index buffer for writing.

		d3dContext->Map(vertexBufferObject, 0, D3D11_MAP_WRITE_DISCARD, 0, &vertexSubresource);
		d3dContext->Map(indexBufferObject, 0, D3D11_MAP_WRITE_DISCARD, 0, &indexSubresource);

		glyphBuffer[0].glyphVertex = static_cast<Slug::GlyphVertex *>(vertexSubresource.pData);
		glyphBuffer[0].glyphTriangle = static_cast<Slug::GlyphTriangle *>(indexSubresource.pData);

	#endif

	// Generate the vertex and triangle data for the text, storing it directly in the buffers.

	glyphBuffer[1].glyphVertex = glyphBuffer[0].glyphVertex + maxVertexCount[0];
	glyphBuffer[1].glyphTriangle = glyphBuffer[0].glyphTriangle + maxTriangleCount[0];

	glyphBuffer[0].vertexIndex = 0;
	glyphBuffer[1].vertexIndex = maxVertexCount[0];

	volatile Slug::GlyphTriangle *triangleBase[2] = {glyphBuffer[0].glyphTriangle, glyphBuffer[1].glyphTriangle};

	Slug::BuildSlug(fontHeader, &textLayoutData, text, 0.0F, 0.0F, glyphBuffer);

	#if SLUG_OPENGL

		glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
		glUnmapBuffer(GL_ARRAY_BUFFER);

	#elif SLUG_D3D11

		d3dContext->Unmap(indexBufferObject, 0);
		d3dContext->Unmap(vertexBufferObject, 0);

	#endif

	// Set the element count to the actual number of triangles generated by the BuildSlug() function.
	// This can be less than the maximum number returned by the MeasureSlug() function if clipping
	// planes are enabled in the layout data.

	elementCount[0] = int32(glyphBuffer[0].glyphTriangle - triangleBase[0]) * 3;
	elementCount[1] = int32(glyphBuffer[1].glyphTriangle - triangleBase[1]) * 3;
	effectOffset = maxTriangleCount[0] * sizeof(Slug::GlyphTriangle);
}

void Text::Render(const Matrix4D& viewproj)
{
	Matrix4D m = viewproj * textTransform;

	#if SLUG_OPENGL

		// Bind the curve and band textures.

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_RECTANGLE, textFont->GetTextureName(0));
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_RECTANGLE, textFont->GetTextureName(1));

		// Bind the shader program.

		glUseProgram(programName);

		// Set the MVP matrix for the vertex shader.

		glUniform4f(vparamLocation, m(0,0), m(0,1), m(0,2), m(0,3));
		glUniform4f(vparamLocation + 1, m(1,0), m(1,1), m(1,2), m(1,3));
		glUniform4f(vparamLocation + 2, m(2,0), m(2,1), m(2,2), m(2,3));
		glUniform4f(vparamLocation + 3, m(3,0), m(3,1), m(3,2), m(3,3));

		// Bind the VBOs, and specify the attribute arrays.

		glBindBuffer(GL_ARRAY_BUFFER, bufferName[0]);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferName[1]);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);

		glVertexAttribPointer(0, 2, GL_FLOAT, false, sizeof(Slug::GlyphVertex), nullptr);
		glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(Slug::GlyphVertex), (char *) 8);
		glVertexAttribPointer(2, 4, GL_FLOAT, false, sizeof(Slug::GlyphVertex), (char *) 16);
		glVertexAttribPointer(3, 4, GL_UNSIGNED_BYTE, true, sizeof(Slug::GlyphVertex), (char *) 32);

		// Draw the text!

		if (elementCount[1] != 0)
		{
			// The effect gets drawn first, then the ordinary text.

			GLsizei count[2] = {elementCount[1], elementCount[0]};
			const void *const address[2] = {(void *) effectOffset, 0};
			glMultiDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, address, 2);
		}
		else
		{
			glDrawElements(GL_TRIANGLES, elementCount[0], GL_UNSIGNED_SHORT, nullptr);
		}

	#elif SLUG_D3D11

		D3D11_MAPPED_SUBRESOURCE	constantSubresource;

		// Set the MVP matrix for the vertex shader.

		d3dContext->Map(constantBufferObject, 0, D3D11_MAP_WRITE_DISCARD, 0, &constantSubresource);
		volatile float *vparam = static_cast<float *>(constantSubresource.pData);
		for (machine k = 0; k < 16; k++) vparam[k] = m(k >> 2, k & 3);
		d3dContext->Unmap(constantBufferObject, 0);

		// Bind the constant buffer and textures.

		d3dContext->VSSetConstantBuffers(0, 1, &constantBufferObject);
		d3dContext->PSSetShaderResources(0, 2, textFont->GetTextureViews());

		// Bind the vertex and pixel shaders.

		d3dContext->VSSetShader(vertexShaderObject, nullptr, 0);
		d3dContext->PSSetShader(pixelShaderObject, nullptr, 0);

		// Set the input layout and bind the vertex/index buffers.

		UINT stride = sizeof(Slug::GlyphVertex);
		UINT offset = 0;

		d3dContext->IASetInputLayout(inputLayoutObject);
		d3dContext->IASetVertexBuffers(0, 1, &vertexBufferObject, &stride, &offset);

		// Draw the text!

		d3dContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		if (elementCount[1] != 0)
		{
			// The effect gets drawn first, then the ordinary text.

			d3dContext->IASetIndexBuffer(indexBufferObject, DXGI_FORMAT_R16_UINT, UINT(effectOffset));
			d3dContext->DrawIndexed(elementCount[1], 0, 0);
		}

		d3dContext->IASetIndexBuffer(indexBufferObject, DXGI_FORMAT_R16_UINT, 0);
		d3dContext->DrawIndexed(elementCount[0], 0, 0);

	#endif
}


int32 Demo::UintToString(unsigned_int32 num, char *text, int32 max)
{
	char	c[16];

	machine length = 0;
	do
	{
		unsigned_int32 p = num % 10;
		c[length++] = char(p + 48);
		num /= 10;
	} while (num != 0);

	machine a = -1;
	do
	{
		if (++a < max)
		{
			text[a] = c[--length];
		}
		else
		{
			text[a] = 0;
			return (int32(a));
		}
	} while (length != 0);

	text[++a] = 0;
	return (int32(a));
}


// SCENE INITIALIZATION
// ========================
// This is where the Text objects for everything drawn in the main scene are created.

void Demo::InitSceneText(Array<Text *> (& textArray)[kSceneCount], bool (& orthoFlag)[kSceneCount], unsigned_int32 extraLayoutFlags)
{
	Slug::LayoutData		layoutData;

	layoutData.fontSize = 40.0F;
	layoutData.textScale = 1.0F;
	layoutData.textShift = 0.0F;
	layoutData.textTracking = 0.0F;
	layoutData.textSkew = 0.0F;
	layoutData.textColor[0].red = 0;
	layoutData.textColor[0].green = 0;
	layoutData.textColor[0].blue = 0;
	layoutData.textColor[0].alpha = 255;
	layoutData.sequenceTypeMask = Slug::kSequenceDefaultMask;
	layoutData.alternateTypeMask = 0;
	layoutData.styleIndex = 0;
	layoutData.layoutFlags = Slug::kLayoutSymmetricBands | Slug::kLayoutBoundingPolygons | extraLayoutFlags;
	layoutData.formatMask = ~0;
	layoutData.effectType = Slug::kGlyphEffectNone;

	Font *arialFont = new Font("../Font/arial.slug");
	Font *timesFont = new Font("../Font/times.slug");
	Font *segoeFont = new Font("../Font/seguiemj.slug");
	Font *minionFont = new Font("../Font/minion.slug");
	Font *gillbdFont = new Font("../Font/gillbd.slug");

	// Create the text "Slug" with all the combining diacritical marks.

	Text *text = new Text(timesFont, &layoutData, u8"S\u0327\u0304\u0301l\u030Au\u0320\u030C\u0308g\u0303\u033B", -38.0F, -100.0F);
	textArray[0].AppendArrayElement(text);

	// Create the line containing the multicolor emoji. (The first emoji on the right side is a ZWJ sequence.)

	layoutData.fontSize = 12.0F;
	text = new Text(segoeFont, &layoutData, u8"\U0001F600 \U0001F609 \U0001F644 \U0001F60E \U0001F61C Multicolor Emoji \U0001F468\u200D\U0001F680 \U0001F355 \U0001F383 \U0001F480 \U0001F409", -145.0F, -60.0F);
	textArray[0].AppendArrayElement(text);

	// Create the line that demonstrates kerning. Embedded formatting directives are used to turn kerning on and off.

	layoutData.layoutFlags |= Slug::kLayoutFormatDirectives;
	text = new Text(minionFont, &layoutData, u8"{#kern(false)}\u201CToo Wavy.\u201D {#color(128,128,128)}with kerning is {#color(0,0,0);kern(true)}\u201CToo Wavy.\u201D", -105.0F, -20.0F);
	textArray[0].AppendArrayElement(text);

	// Create the line that demonstrates sequences. Embedded formatting directives are used to turn sequence replacement on and off.

	text = new Text(minionFont, &layoutData, u8"{#seq(false)}Th fi ffl {#color(128,128,128)}with ligatures is {#color(0,0,0);seq(true)}Th fi ffl", -80.0F, 20.0F);
	textArray[0].AppendArrayElement(text);

	// Create the line that demonstrates skew and subscripts. Embedded formatting directives are required to generate subscripts.

	text = new Text(minionFont, &layoutData, u8"In {#skew(0.25)}photosynthesis{#skew(0)}, CO{#inf(true)}2{#inf(false)} + 2H{#inf(true)}2{#inf(false)}O + \u03B3 \u2192 [CH{#inf(true)}2{#inf(false)}O] + O{#inf(true)}2{#inf(false)} + H{#inf(true)}2{#inf(false)}O.", -150.0F, 60.0F);
	textArray[0].AppendArrayElement(text);

	// Create the line that demonstrates glyph composition used by skin tone modifiers.

	text = new Text(segoeFont, &layoutData, u8"Skin tone modifiers:  \U0001F590 \U0001F590\U0001F3FB \U0001F590\U0001F3FC \U0001F590\U0001F3FD \U0001F590\U0001F3FE \U0001F590\U0001F3FF", -115.0F, 100.0F);
	textArray[0].AppendArrayElement(text);

	// Create the shadow effect.

	layoutData.effectType = Slug::kGlyphEffectShadow;
	layoutData.effectOffset[0] = 0.02F;
	layoutData.effectOffset[1] = 0.04F;
	layoutData.effectColor[0].red = 0;
	layoutData.effectColor[0].green = 0;
	layoutData.effectColor[0].blue = 0;
	layoutData.effectColor[0].alpha = 248;

	layoutData.fontSize = 15.0F;
	layoutData.textColor[0].red = 0;
	layoutData.textColor[0].green = 160;
	layoutData.textColor[0].blue = 0;
	layoutData.layoutFlags &= ~Slug::kLayoutOpticalWeight;

	text = new Text(gillbdFont, &layoutData, "Shadow Effect", -132.0F, 140.0F);
	textArray[0].AppendArrayElement(text);

	// Create the outline effect.

	layoutData.effectType = Slug::kGlyphEffectOutline;
	layoutData.effectOffset[0] = 0.0F;
	layoutData.effectOffset[1] = 0.0F;
	layoutData.effectColor[0].red = 0;
	layoutData.effectColor[0].green = 0;
	layoutData.effectColor[0].blue = 0;
	layoutData.effectColor[0].alpha = 255;

	layoutData.textTracking = 0.04F;
	layoutData.textColor[0].red = 255;
	layoutData.textColor[0].green = 192;
	layoutData.textColor[0].blue = 0;

	text = new Text(gillbdFont, &layoutData, "Outline Effect", 10.0F, 140.0F);
	textArray[0].AppendArrayElement(text);

	// Restore some settings.

	layoutData.textTracking = 0.0F;
	layoutData.textColor[0].red = 0;
	layoutData.textColor[0].green = 0;
	layoutData.textColor[0].blue = 0;
	layoutData.layoutFlags |= extraLayoutFlags;
	layoutData.effectType = Slug::kGlyphEffectNone;

	// Create large paragraphs of text for the second scene.

	static const char *const string1[11] =
	{
		u8"Your name is Gus Graves, and you\u2019re a firefighter in the small town of Timber Valley, where the largest employer is the",
		u8"mysterious research division of the MGL Corporation, a powerful and notoriously secretive player in the military-industrial",
		u8"complex. It\u2019s sunset on Halloween, and just as you\u2019re getting ready for a stream of trick-or-treaters at home, your",
		u8"chief calls you into the station. There\u2019s a massive blaze at the MGL building on the edge of town. You jump off the fire",
		u8"engine as it rolls up to the inferno and gasp not only at the incredible size of the fire but at the strange beams of light",
		u8"brilliantly flashing through holes in the building\u2019s crumbling walls. As you approach the structure for a closer look,",
		u8"the wall and floor of the building collapse to expose a vast underground chamber where all kinds of debris are being pulled",
		u8"into a blinding light at the center of a giant metallic ring. The ground begins to fall beneath your feet, and you try to",
		u8"scurry up the steepening slope to escape, but it\u2019s too late. You\u2019re pulled into the device alongside some mangled",
		u8"equipment and the bodies of lab technicians who didn\u2019t survive the accident. You see your fire engine gravitating toward",
		u8"you as you accelerate into a tunnel of light."
	};

	static const char *const string2[6] =
	{
		u8"After a few seconds, you slam to the ground in a grassy meadow. It\u2019s raining debris and corpses through a portal not far",
		u8"above you. You see the fire engine falling toward you and roll out of the way just in time to avoid being crushed. You pull",
		u8"yourself up off the ground and take a look around. You\u2019re not in Timber Valley any more. You can see only dense forest",
		u8"in every direction, but there are some dirt paths leading into the woods indicating some kind of recent activity by local",
		u8"inhabitants. The portal soon closes, leaving only a bunch of smoldering junk, your wrecked fire engine, and the remains of",
		u8"some unfortunate MGL workers behind. You\u2019re on your own."
	};

	layoutData.fontSize = 10.0F;
	layoutData.layoutFlags &= ~Slug::kLayoutFormatDirectives;
	layoutData.sequenceTypeMask |= Slug::kSequenceDiscretionaryLigatures;

	for (machine i = 0; i < 11; i++)
	{
		text = new Text(arialFont, &layoutData, string1[i], -250.0F, float(i) * 15.0F - 130.0F);
		textArray[1].AppendArrayElement(text);
	}

	layoutData.fontSize = 11.0F;

	for (machine i = 0; i < 6; i++)
	{
		text = new Text(timesFont, &layoutData, string2[i], -250.0F, float(i) * 15.0F + 50.0F);
		textArray[1].AppendArrayElement(text);
	}

	// Create third scene with a variety of complex fonts.

	Font *chalkFont = new Font("../Font/chalk.slug");
	Font *jokermanFont = new Font("../Font/jokerman.slug");
	Font *royalFont = new Font("../Font/royal.slug");
	Font *scratchFont = new Font("../Font/scratch.slug");
	Font *websFont = new Font("../Font/webs.slug");
	Font *wildwoodFont = new Font("../Font/wildwood.slug");

	layoutData.fontSize = 24.0F;

	text = new Text(arialFont, &layoutData, "Chalk", -220.0F, -120.0F);
	textArray[2].AppendArrayElement(text);

	text = new Text(chalkFont, &layoutData, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", -70.0F, -120.0F);
	textArray[2].AppendArrayElement(text);

	text = new Text(arialFont, &layoutData, "Jokerman", -220.0F, -70.0F);
	textArray[2].AppendArrayElement(text);

	text = new Text(jokermanFont, &layoutData, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", -70.0F, -70.0F);
	textArray[2].AppendArrayElement(text);

	text = new Text(arialFont, &layoutData, "Royal", -220.0F, -20.0F);
	textArray[2].AppendArrayElement(text);

	text = new Text(royalFont, &layoutData, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", -70.0F, -20.0F);
	textArray[2].AppendArrayElement(text);

	text = new Text(arialFont, &layoutData, "Scratch", -220.0F, 30.0F);
	textArray[2].AppendArrayElement(text);

	text = new Text(scratchFont, &layoutData, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", -70.0F, 30.0F);
	textArray[2].AppendArrayElement(text);

	text = new Text(arialFont, &layoutData, "Webs", -220.0F, 80.0F);
	textArray[2].AppendArrayElement(text);

	text = new Text(websFont, &layoutData, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", -70.0F, 80.0F);
	textArray[2].AppendArrayElement(text);

	text = new Text(arialFont, &layoutData, "Wildwood", -220.0F, 130.0F);
	textArray[2].AppendArrayElement(text);

	text = new Text(wildwoodFont, &layoutData, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", -70.0F, 130.0F);
	textArray[2].AppendArrayElement(text);

	wildwoodFont->Release();
	websFont->Release();
	scratchFont->Release();
	royalFont->Release();
	jokermanFont->Release();
	chalkFont->Release();

	// Create emoji table in fourth scene.

	layoutData.fontSize = 12.0F;
	layoutData.layoutFlags &= ~Slug::kLayoutOpticalWeight;

	enum {kMaxEmojiCount = 25};
	char string[kMaxEmojiCount * 5];

	int32 emojiCount = 0;
	float y = -120.0F;

	for (unsigned_int32 unicode = 0x1F300; unicode < 0x1FA00; unicode++)
	{
		if (Slug::GetGlyphIndex(segoeFont->GetFontHeader(), unicode) != 0)
		{
			// Emoji exists in font. Encode as UTF-8.

			char *c = &string[emojiCount * 5];
			c[0] = char(((unicode >> 18) & 0x07) | 0xF0);
			c[1] = char(((unicode >> 12) & 0x3F) | 0x80);
			c[2] = char(((unicode >> 6) & 0x3F) | 0x80);
			c[3] = char((unicode & 0x3F) | 0x80);

			if (++emojiCount < kMaxEmojiCount)
			{
				c[4] = ' ';
			}
			else
			{
				c[4] = 0;
				emojiCount = 0;

				text = new Text(segoeFont, &layoutData, string, -240.0F, y);
				textArray[3].AppendArrayElement(text);
				y += 28.0F;
			}
		}
	}

	if (emojiCount != 0)
	{
		string[emojiCount * 5 - 1] = 0;
		text = new Text(segoeFont, &layoutData, string, -240.0F, y);
		textArray[3].AppendArrayElement(text);
	}

	// Create the pixel-aligned text in the fifth scene.

	layoutData.layoutFlags = extraLayoutFlags;
	layoutData.textScale = 1.0F / arialFont->GetFontHeader()->fontCapHeight;

	layoutData.fontSize = 14.0F;
	text = new Text(arialFont, &layoutData, u8"The baseline and cap height have been pixel aligned for this text to produce sharper glyphs at small sizes.", 512.0F, 320.0F);
	textArray[4].AppendArrayElement(text);

	text = new Text(arialFont, &layoutData, u8"(This scene cannot be moved with the camera controls.)", 512.0F, 350.0F);
	textArray[4].AppendArrayElement(text);

	layoutData.fontSize = 12.0F;
	text = new Text(arialFont, &layoutData, u8"Arial 12 \u2022 ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz 0123456789", 512.0F, 400.0F);
	textArray[4].AppendArrayElement(text);

	layoutData.fontSize = 11.0F;
	text = new Text(arialFont, &layoutData, u8"Arial 11 \u2022 ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz 0123456789", 512.0F, 420.0F);
	textArray[4].AppendArrayElement(text);

	layoutData.fontSize = 10.0F;
	text = new Text(arialFont, &layoutData, u8"Arial 10 \u2022 ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz 0123456789", 512.0F, 440.0F);
	textArray[4].AppendArrayElement(text);

	layoutData.fontSize = 9.0F;
	text = new Text(arialFont, &layoutData, u8"Arial 9 \u2022 ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz 0123456789", 512.0F, 460.0F);
	textArray[4].AppendArrayElement(text);

	layoutData.fontSize = 8.0F;
	text = new Text(arialFont, &layoutData, u8"Arial 8 \u2022 ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz 0123456789", 512.0F, 480.0F);
	textArray[4].AppendArrayElement(text);

	layoutData.textScale = 1.0F / timesFont->GetFontHeader()->fontCapHeight;

	layoutData.fontSize = 12.0F;
	text = new Text(timesFont, &layoutData, u8"Times 12 \u2022 ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz 0123456789", 512.0F, 520.0F);
	textArray[4].AppendArrayElement(text);

	layoutData.fontSize = 11.0F;
	text = new Text(timesFont, &layoutData, u8"Times 11 \u2022 ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz 0123456789", 512.0F, 540.0F);
	textArray[4].AppendArrayElement(text);

	layoutData.fontSize = 10.0F;
	text = new Text(timesFont, &layoutData, u8"Times 10 \u2022 ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz 0123456789", 512.0F, 560.0F);
	textArray[4].AppendArrayElement(text);

	layoutData.fontSize = 9.0F;
	text = new Text(timesFont, &layoutData, u8"Times 9 \u2022 ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz 0123456789", 512.0F, 580.0F);
	textArray[4].AppendArrayElement(text);

	layoutData.fontSize = 8.0F;
	text = new Text(timesFont, &layoutData, u8"Times 8 \u2022 ABCDEFGHIJKLMNOPQRSTUVWXYZ abcdefghijklmnopqrstuvwxyz 0123456789", 512.0F, 600.0F);
	textArray[4].AppendArrayElement(text);

	gillbdFont->Release();
	minionFont->Release();
	segoeFont->Release();
	timesFont->Release();
	arialFont->Release();

	// The last scene uses the orthographic projection, and all others use the perpsective projection.

	for (machine scene = 0; scene < kSceneCount - 1; scene++)
	{
		orthoFlag[scene] = false;
	}

	orthoFlag[kSceneCount - 1] = true;
}

void Demo::TermSceneText(Array<Text *> (& textArray)[kSceneCount])
{
	for (machine scene = kSceneCount - 1; scene >= 0; scene--)
	{
		for (machine a = textArray[scene].GetArrayElementCount() - 1; a >= 0; a--)
		{
			delete textArray[scene][a];
		}

		textArray[scene].ClearArray();
	}
}


// OVERLAY INITIALIZATION
// ========================
// This is where the Text objects for the information shown in the upper-left corner are created.

void Demo::InitOverlayText(int32 width, int32 height, Array<Text *>& textArray, Text **timeText, unsigned_int32 extraLayoutFlags)
{
	Slug::LayoutData		layoutData;

	Font *font = new Font("../Font/arial.slug");

	layoutData.fontSize = 15.0F;
	layoutData.textScale = 1.0F / font->GetFontHeader()->fontCapHeight;		// Setting the scale to the inverse cap height vertically aligns letters to the pixel grid.
	layoutData.textShift = 0.0F;
	layoutData.textTracking = 0.0F;
	layoutData.textSkew = 0.0F;
	layoutData.textColor[0].red = 0;
	layoutData.textColor[0].green = 0;
	layoutData.textColor[0].blue = 0;
	layoutData.textColor[0].alpha = 255;
	layoutData.sequenceTypeMask = Slug::kSequenceDefaultMask;
	layoutData.alternateTypeMask = 0;
	layoutData.styleIndex = 0;
	layoutData.layoutFlags = extraLayoutFlags;
	layoutData.formatMask = ~0;
	layoutData.effectType = Slug::kGlyphEffectNone;

	Text *text = new Text(font, &layoutData, rendererDescription, 16.0F, 32.0F);
	textArray.AppendArrayElement(text);

	// Create the line showing the screen resolution.

	static char resolution[32] = "Resolution: ";

	int32 len = 12;
	len += UintToString(width, &resolution[len], 31 - len);
	resolution[len++] = ' ';
	resolution[len++] = '\xC3';		// U+00D7 = multiplication sign
	resolution[len++] = '\x97';
	resolution[len++] = ' ';
	len += UintToString(height, &resolution[len], 31 - len);
	resolution[len] = 0;

	text = new Text(font, &layoutData, resolution, 16.0F, 64.0F);
	textArray.AppendArrayElement(text);

	// Create the line showing that the Escape key quits.

	text = new Text(font, &layoutData, u8"ESC \u2022 Quit demo", 16.0F, 160.0F);	// U+2022 = bullet
	textArray.AppendArrayElement(text);

	// Create the line showing that the Space key cycles the demo pages.

	text = new Text(font, &layoutData, u8"SPACE \u2022 Cycle scene", 16.0F, 192.0F);	// U+2022 = bullet
	textArray.AppendArrayElement(text);

	// Create the line showing that the Tab key toggles the optical weight.

	text = new Text(font, &layoutData, u8"TAB \u2022 Toggle weight", 16.0F, 224.0F);	// U+2022 = bullet
	textArray.AppendArrayElement(text);

	// Create the line showing that the F1 key resets the view transform.

	text = new Text(font, &layoutData, u8"F1 \u2022 Reset view", 16.0F, 256.0F);	// U+2022 = bullet
	textArray.AppendArrayElement(text);

	// Create the line showing the rendering time. The text string is generated in the main loop every frame.

	layoutData.alternateTypeMask |= Slug::kAlternateTabularFigures;
	layoutData.layoutFlags |= Slug::kLayoutKernDisable;

	text = new Text(font, &layoutData, nullptr, 16.0F, 96.0F);
	textArray.AppendArrayElement(text);
	*timeText = text;

	font->Release();
}

void Demo::TermOverlayText(Array<Text *>& textArray)
{
	for (machine a = textArray.GetArrayElementCount() - 1; a >= 0; a--)
	{
		delete textArray[a];
	}

	textArray.ClearArray();
}


void Demo::Render(const ImmutableArray<Text *>& textArray, const Matrix4D& viewproj)
{
	for (Text *text : textArray)
	{
		text->Render(viewproj);
	}
}
