//
// Slug version 1.8
//
// This is the header file for the Slug GPU font rendering library.
// It accompanies the Slug.lib static library.
//
// The noncommercial version of Slug is distributed under the
// Creative Commons BY-NC-SA license:
//
//   https://creativecommons.org/licenses/by-nc-sa/4.0/
//
// Slug was developed by Eric Lengyel at Terathon Software LLC.
// Copyright 2017. Additional IP protections apply.
//


#ifndef Slug_h
#define Slug_h


#define SLUG_API


#if defined(_MSC_VER)

	typedef signed char				int8;
	typedef short					int16;
	typedef int						int32;
	typedef __int64					int64;
	typedef __int64					machine;
	typedef unsigned char			unsigned_int8;
	typedef unsigned short			unsigned_int16;
	typedef unsigned int			unsigned_int32;
	typedef unsigned __int64		unsigned_int64;
	typedef unsigned __int64		unsigned_machine;

#elif defined(__GNUC__)

	typedef signed char				int8;
	typedef short					int16;
	typedef int						int32;
	typedef long long				int64;
	typedef long long				machine;
	typedef unsigned char			unsigned_int8;
	typedef unsigned short			unsigned_int16;
	typedef unsigned int			unsigned_int32;
	typedef unsigned long long		unsigned_int64;
	typedef unsigned long long		unsigned_machine;

#elif defined(__ORBIS__)

	typedef signed char				int8;
	typedef short					int16;
	typedef int						int32;
	typedef long					int64;
	typedef long					machine;
	typedef unsigned char			unsigned_int8;
	typedef unsigned short			unsigned_int16;
	typedef unsigned int			unsigned_int32;
	typedef unsigned long			unsigned_int64;
	typedef unsigned long			unsigned_machine;

#endif


namespace Slug
{
	typedef unsigned_int32 GlyphEffectType;		// Glyph effects are identified by four-character codes.
	typedef unsigned_int16 FontTexel[4];		// Each texel in the curve and band textures has four channels of 16-bit data.


	// The following values give the maximum number of unique vertex shaders and pixel shaders that can ever be used
	// by Slug. The indices returned by the GetShaderIndices() function are always less than these values.

	enum
	{
		kMaxVertexShaderCount				= 1,
		kMaxPixelShaderCount				= 8
	};


	// The following values can be combined (through logical OR) in the
	// glyphFlags field of the GlyphData structure.

	enum
	{
		kGlyphMark							= 1 << 0,		// The glyph is a combining mark.
		kGlyphShadow						= 1 << 1		// The multicolor data for the glyph includes an entry for the shadow effect.
	};


	// The following values can be combined (through logical OR) in the
	// fontFlags field of the FontHeader structure.

	enum
	{
		kFontMulticolor						= 1 << 0,		// The font contains data for multicolor glyphs.
		kFontOutline						= 1 << 1		// The font contains data for outlined glyph effects.
	};


	// The following values can be combined (through logical OR) in the sequenceTypeMask field of the LayoutData structure.
	// The sequence type mask is ignored if the kLayoutSequenceDisable bit is set in the layoutFlags field.

	enum
	{
		kSequenceGlyphComposition			= 1 << 0,		// Glyph compositions expected by a font to be applied in all circumstances. This is used for emoji with skin tone modifiers, for example.
		kSequenceStandardLigatures			= 1 << 1,		// Standard ligatures provided by a font as substitute glyphs for letter groupings such as "fi" or "ffl". (Some fonts may specify these as discretionary.) This also includes sequences containing zero-width joiners.
		kSequenceRequiredLigatures			= 1 << 2,		// Required ligatures considered by a font to be required in some writing systems for various letter groupings.
		kSequenceDiscretionaryLigatures		= 1 << 3,		// Discretionary ligatures provided by a font as additional substitute glyphs that are considered optional.
		kSequenceHistoricalLigatures		= 1 << 4,		// Historical ligatures provided by a font as optional antiquated forms.

		kSequenceDefaultMask				= kSequenceGlyphComposition | kSequenceStandardLigatures | kSequenceRequiredLigatures
	};


	// The following values can be combined (through logical OR) in the alternateTypeMask field of the LayoutData structure.
	// The alternate type mask is ignored if the kLayoutAlternateDisable bit is set in the layoutFlags field.

	enum
	{
		kAlternateStylistic					= 1 << 0,		// Replace glyphs with alternates from a stylistic set. The styleIndex field of the LayoutData structure determines which set is used.
		kAlternateHistorical				= 1 << 1,		// Replace glyphs with historical alternates.
		kAlternateLowerSmallCaps			= 1 << 2,		// Replace lowercase characters with small caps variants.
		kAlternateUpperSmallCaps			= 1 << 3,		// Replace uppercase characters with small caps variants.
		kAlternateTitlingCaps				= 1 << 4,		// Replace glyphs with titling caps variants.
		kAlternateUnicase					= 1 << 5,		// Replace both cases with forms having equal heights.
		kAlternateCaseForm					= 1 << 6,		// Replace case-sensitive punctuation with uppercase forms.
		kAlternateSlashedZero				= 1 << 7,		// Replace the number zero with a slashed variant.
		kAlternateLiningFigures				= 1 << 8,		// Replace oldstyle figures with lining figures.
		kAlternateOldstyleFigures			= 1 << 9,		// Replace lining figures with oldstyle figures.
		kAlternateTabularFigures			= 1 << 10,		// Replace proportional figures with tabular figures.
		kAlternateProportionalFigures		= 1 << 11,		// Replace tabular figures with proportional figures.
		kAlternateSubscript					= 1 << 12,		// Replace glyphs with subscript variants.
		kAlternateSuperscript				= 1 << 13,		// Replace glyphs with superscript variants.
		kAlternateInferiors					= 1 << 14,		// Replace glyphs with subscripts intended for scientific formulas.
		kAlternateOrdinals					= 1 << 15		// Replace glyphs with superscripts intended for ordinal numbers.
	};


	// The following values can be combined (through logical OR) in the formatMask field of the LayoutData structure.
	// The format mask is used only when the kLayoutFormatDirectives bit is set in the layoutFlags field.

	enum
	{
		kFormatSize							= 1 << 0,		// Font size directive size().
		kFormatScale						= 1 << 1,		// Text scale directive scale().
		kFormatShift						= 1 << 2,		// Text scale directive shift().
		kFormatTracking						= 1 << 3,		// Tracking directive track().
		kFormatSkew							= 1 << 4,		// Skew directive skew().
		kFormatColor						= 1 << 5,		// Color directives color() and color2().
		kFormatGradient						= 1 << 6,		// Gradient directives grad() and gcoord().
		kFormatKern							= 1 << 7,		// Kerning directive kern().
		kFormatMark							= 1 << 8,		// Mark placement directive mark().
		kFormatSequence						= 1 << 9,		// Sequence directive sequence() and all specific sequence replacement directives.
		kFormatAlternate					= 1 << 10		// Alternate directive alternate() and all specific alternate substitution directives.
	};


	// The following values can be combined (through logical OR) in the
	// layoutFlags field of the LayoutData structure.

	enum
	{
		kLayoutFormatDirectives				= 1 << 0,		// Formatting directives embedded in the text are applied. See the end of this file for information about using formatting directives.
		kLayoutClippingPlanes				= 1 << 1,		// Each glyph is clipped against left and right boundaries.
		kLayoutSymmetricBands				= 1 << 2,		// The symmetric band split optimization is applied in the glyph shader.
		kLayoutBoundingPolygons				= 1 << 3,		// Bounding polygons are generated instead of bounding boxes, if available.
		kLayoutOpticalWeight				= 1 << 4,		// When rendering, coverage values are remapped to increase the optical weight of the glyphs. This can improve the appearance of small text, but usually looks good only for dark text on a light background.
		kLayoutTextGradient					= 1 << 5,		// Apply a two-color vertical gradient to each glyph instead of rendering with a single constant color.
		kLayoutEffectGradient				= 1 << 6,		// A vertical gradient is independently applied to glyph effects. If this flag is not set, effects are rendered as a single color regardless of whether the kLayoutTextGradient flag is set.
		kLayoutMulticolorDisable			= 1 << 7,		// The text is always rendered using monochrome glyphs, even if multicolor glyphs are available.
		kLayoutKernDisable					= 1 << 8,		// Kerning is not applied to the text. When this flag is not set, kerning is applied according to the tables specified in the original font.
		kLayoutMarkDisable					= 1 << 9,		// Combining marks are not repositioned in the text. When this flag is not set, each combining mark is moved to the appropriate attachment point belonging to the preceding glyph.
		kLayoutSequenceDisable				= 1 << 10,		// Sequences are not matched in the text. When this flag is not set, tables specified in the original font can cause certain sequences of glyphs to be replaced by one or more other glyphs.
		kLayoutAlternateDisable				= 1 << 11		// Alternate glyph substitution is not applied in the text. When this flag is not set, various substitution features can cause glyphs to be replaced by alternate forms.
	};


	// The following values are the glyph effect types that can be specified
	// in the effectType field of the LayoutData structure.

	enum : GlyphEffectType
	{
		kGlyphEffectNone					= 0,			//## No glyph effect is applied.
		kGlyphEffectShadow					= 'SHAD',		//## A drop shadow is rendered beneath each glyph.
		kGlyphEffectOutline					= 'OTLN'		//## An expanded outline is rendered beneath each glyph. (The font must contain outline glyph effect data for this effect to appear.)
	};


	// Each glyph in a font has an associated GlyphData structure that describes all of the glyph's properties.

	struct GlyphData
	{
		unsigned_int16		glyphFlags;					// Flags indicating various properties of the glyph.

		int8				bandCount[2];				// The number of vertical and horizontal bands holding curve index data for the glyph. These are zero if the glyph has no curves.
		float				bandScale;					// The scale by which em-space coordinates are multiplied to calculate band indices.
		float				bandOffset[2];				// The x and y offsets added to em-space coordinates after multiplication by bandScale to calculate band indices.

		float				advanceWidth;				// The advance width of the glyph in em units.
		float				boundingBox[4];				// The bounding box (xmin, ymin, xmax, ymax) of the glyph in the coordinate system of the em square.
		unsigned_int16		dataLocation[2][2];			// The coordinates in the font's band texture at which data for the glyph begins. The first entry is for the monochrome glyph, and the second entry is for the multicolor glyph, if available.

		unsigned_int32		geometryData;				// The high 8 bits contain the geometry code for the glyph, and the low 24 bits contain the offset into the font's geometry data table at which supplemental vertex coordinates begin for the glyph.
		unsigned_int32		kernData;					// The high 12 bits contain the number of kern pairs for which the glyph is on the right side of each pair, and the low 20 bits contain the offset into the font's kerning data table at which kern pairs begin for the glyph.
		unsigned_int32		markData;					// The high 6 bits contain the number of anchor points to which combining marks can be attached to the glyph. The next 6 bits, nonzero only for mark glyphs, contain the number of anchor points at which a mark can attach to another glyph. The low 20 bits contain the offset into the font's combining mark data table at which anchor points begin for the glyph. The number of elements in the table is equal to the sum of the two counts in the high 12 bits.
		unsigned_int32		sequenceData;				// The high 12 bits contain the number of sequences for the glyph, and the low 20 bits contain the offset into the font's sequence data table at which sequences begin for the glyph.
		unsigned_int32		alternateData;				// The high 8 bits contain the number of alternates for the glyph, and the low 24 bits contain the offset into the font's alternate data table at which alternates begin for the glyph.
	};


	// The kern data for a particular glyph consists of an array of KernPair structures.

	struct KernPair
	{
		int32				leftIndex;					// The glyph index for the left side of the kern pair.
		float				kernValue;					// The horizontal adjustment distance, in em units, to apply to the kern pair.
	};


	// The mark data for a particular glyph consists of an array of MarkAnchor structures.

	struct MarkAnchor
	{
		int32				anchorIndex;				// The index of the anchor. This is used to match an anchor point on a mark to an anchor point on the glyph to which it's attached.
		float				anchorPosition[2];			// The position of the anchor in em-space coordinates.
	};


	// The alternate data for a particular glyph consists of an array of AlternateGlyph structures.

	struct AlternateGlyph
	{
		int32				alternateType;				// The type of the alternate glyph.
		int32				glyphIndex;					// The index of the alternate glyph.
	};


	// The FontHeader structure contains general information about a font, and most of the data is used internally.
	// All Slug API functions take a font header as the first parameter. A pointer to a FontHeader structure can be
	// obtained from the raw .slug file data by calling the GetFontHeader() function.

	struct FontHeader
	{
		unsigned_int32		fontFlags;					// Flags indicating various properties of the font.
		float				fontCapHeight;				// The cap height for the font, in em units. This represents the typical distance from the baseline to the tops of the capital roman letters.
		float				fontExHeight;				// The ex height for the font, in em units. This represents the typical distance from the baseline to the tops of lowercase roman letters, disregarding ascenders.

		float				fontOutlineSize;			// The size by which glyphs are expanded for the outline effect, in em units.
		int32				reserved[3];

		int32				pageCount;					// The total number of 256-entry pages covered by the range of code points included in the font. Not every page must contain glyph mappings.
		int32				pageIndexOffset;			// The offset to the page index table. The page index table contains pageCount signed 16-bit entries. An entry of &minus;1 in the table indicates that a page contains no glyph mappings.
		int32				glyphIndexOffset;			// The offset to the glyph index table. There are 256 entries for each page that contains glyph mappings, and each entry is a 32-bit integer.

		int32				glyphCount;					// The total number of unique glyphs in the font.
		int32				glyphDataOffset[2];			// The offsets to the tables of GlyphData structures. The first entry is the offset to regular glyphs, and the second entry is the offset to expanded outline glyphs.

		int32				geometryDataOffset;			// The offset to the geometry data table. The location of the geometry data for a particular glyph within this table is given by the geometryData member of the GlyphData structure.
		int32				kernDataOffset;				// The offset to the kerning data table. The location of the kerning data for a particular glyph within this table is given by the kernData member of the GlyphData structure.
		int32				markDataOffset;				// The offset to the combining mark data table. The location of the combining mark data for a particular glyph within this table is given by the markData member of the GlyphData structure.
		int32				sequenceDataOffset;			// The offset to the sequence data table. The location of the sequence data for a particular glyph within this table is given by the sequenceData member of the GlyphData structure.
		int32				alternateDataOffset;		// The offset to the alternate data table. The location of the alternate data for a particular glyph within this table is given by the alternateData member of the GlyphData structure.

		int32				curveTextureSize[2];		// The dimensions of the texture map containing the control points for the quadratic Bézier curves composing all glyphs in the font.
		unsigned_int32		curveCompressionType;		// The compression type applied to the curve texture map.
		unsigned_int32		curveCompressedDataSize;	// The size of the compressed curve texture map data, in bytes.
		int32				curveTextureOffset;			// The offset to the compressed curve texture map data.

		int32				bandTextureSize[2];			// The dimensions of the texture map containing the multi-band Bézier curve index data for all glyphs in the font.
		unsigned_int32		bandCompressionType;		// The compression type applied to the band texture map.
		unsigned_int32		bandCompressedDataSize;		// The size of the compressed band texture map data, in bytes.
		int32				bandTextureOffset;			// The offset to the compressed band texture map data.

		const int16 *GetPageIndexTable(void) const
		{
			return (reinterpret_cast<const int16 *>(reinterpret_cast<const char *>(this) + pageIndexOffset));
		}

		const int32 *GetGlyphIndexTable(int32 page) const
		{
			return (reinterpret_cast<const int32 *>(reinterpret_cast<const char *>(this) + (glyphIndexOffset + page * 1024)));
		}

		const GlyphData *GetGlyphDataTable(int32 index = 0) const
		{
			return (reinterpret_cast<const GlyphData *>(reinterpret_cast<const char *>(this) + glyphDataOffset[index]));
		}

		const float *GetGeometryDataTable(void) const
		{
			return (reinterpret_cast<const float *>(reinterpret_cast<const char *>(this) + geometryDataOffset));
		}

		const KernPair *GetKernDataTable(void) const
		{
			return (reinterpret_cast<const KernPair *>(reinterpret_cast<const char *>(this) + kernDataOffset));
		}

		const MarkAnchor *GetMarkDataTable(void) const
		{
			return (reinterpret_cast<const MarkAnchor *>(reinterpret_cast<const char *>(this) + markDataOffset));
		}

		const int32 *GetSequenceDataTable(void) const
		{
			return (reinterpret_cast<const int32 *>(reinterpret_cast<const char *>(this) + sequenceDataOffset));
		}

		const AlternateGlyph *GetAlternateDataTable(void) const
		{
			return (reinterpret_cast<const AlternateGlyph *>(reinterpret_cast<const char *>(this) + alternateDataOffset));
		}

		const void *GetCurveTextureImage(void) const
		{
			return (reinterpret_cast<const char *>(this) + curveTextureOffset);
		}

		const void *GetBandTextureImage(void) const
		{
			return (reinterpret_cast<const char *>(this) + bandTextureOffset);
		}
	};


	// The input to the glyph vertex shader consists of a 2D object-space position, 2D texture coordinates
	// corresponding to the vertex position in em space, four floating-point values holding glyph-specific
	// band data, and a 32-bit RGBA color. GlyphVertex structures are generated by the BuildSlug() function.

	struct GlyphVertex
	{
		float					position[2];
		float					texcoord[2];
		float					glyph[4];
		unsigned_int32			color;
	};


	// Triangles generated by the BuildSlug() function are simply three 16-bit vertex indices.

	struct GlyphTriangle
	{
		unsigned_int16			index[3];
	};


	// The GlyphBuffer structure is passed to the BuildSlug() function, and it contains pointers to the
	// locations where the generated vertices and triangles are written. (This is typically in GPU-visible
	// memory that could be write-combined, so the pointers are declared volatile to ensure that write
	// order is preserved by the compiler for best performance.) When the BuildSlug() function returns,
	// the fields of the GlyphBuffer structure have been updated to point to the end of the data that was
	// written, and the same GlyphBuffer structure could be passed to another call to BuildSlug() to append
	// more data to the same vertex buffer.

	struct GlyphBuffer
	{
		volatile GlyphVertex	*glyphVertex;			// A pointer to the location where glyph vertex data is written.
		volatile GlyphTriangle	*glyphTriangle;			// A pointer to the location where glyph triangle data is written.
		unsigned_int32			vertexIndex;			// The index of the first vertex written. The indices stored in the triangle data begin with this value. Unless data for multiple slugs is being built in the same vertex buffer, this should be zero.
	};


	// The LayoutColor structure stores the components of an RGBA color as 8-bit values in the range [0,255].
	// The red, green, and blue components are specified in the sRGB color space, and the alpha component is linear.

	struct LayoutColor
	{
		unsigned_int8			red;					// The red component in sRGB space.
		unsigned_int8			green;					// The green component in sRGB space.
		unsigned_int8			blue;					// The blue component in sRGB space.
		unsigned_int8			alpha;					// The alpha component in linear space.
	};


	// The LayoutData structure controls everything about the appearance of a line of text. Many of these values
	// can be dynamically changed by formatting directives embedded in the text if the kLayoutFormatDirectives
	// bit is set in the layoutFlags field.

	struct LayoutData
	{
		float					fontSize;				// The font size, which corresponds to the size of the em square.
		float					textScale;				// An additional scale applied to the text. The font size is multiplied by this scale to obtain the final em size of the text. By default, this should be set to 1.0 for no scaling.
		float					textShift;				// A vertical shift applied to the text. The baseline is shifted by this amount multiplied by the font size. Positive values shift upward, and negative values shift downward. The text scale does not factor into the shift distance. By default, this should be set to 0.0 for no shifting.
		float					textTracking;			// The extra horizontal space added between all pairs of base glyphs, in em units. This can be positive or negative. By default, this should be set to 0.0 for no tracking.
		float					textSkew;				// A skew value representing the ratio of the change in x coordinate to the change in y coordinate measured upward from the baseline. Positive values cause the text to slant to the right, and negative values cause the text to be slanted to the left. By default, this should be set to 0.0 for no skew.

		LayoutColor				textColor[2];			// The text colors. If gradients are not used, then the first color is assigned to all glyph vertices, and the second color is ignored. If the kLayoutColorGradient bit is specified in the layoutFlags field, then a gradient is applied using both colors. The alpha value should be 255 for fully opaque text. Typical default values are {0,0,0,255} for black text and {255,255,255,255} for white text.
		float					textGradient[2];		// The y coordinates at which the gradient color equals the corresponding entry in the textColor field. Each y coordinate is the em-space distance above the baseline. Negative values are allowed. The two y coordinates must not be equal. This field is used only if the kLayoutTextGradient bit is specified in the layoutFlags field.

		unsigned_int32			layoutFlags;			// Various flags that specific rendering and layout options. See the descriptions of the individual bits beginning with "kLayout". A default value of zero causes kerning, combining marks, sequences, and multicolor glyphs to be enabled, and it causes formatting directives, clipping planes, and all rendering optimizations for large text to be disabled.
		unsigned_int32			formatMask;				// A mask that determines which embedded formatting directives can be applied. A one bit indicates that the corresponding formatting directive is enabled. See the descriptions of the individual bits beginning with "kFormat". This field is used only if the kLayoutFormatDirectives bit is specified in the layoutFlags field. A default value of ~0 enables all formatting directives.
		unsigned_int32			sequenceTypeMask;		// A mask that determines which types of sequences are recognized. A one bit indicates that the corresponding type of sequence is enabled. See the descriptions of the individual bits beginning with "kSequence". This field is used only if the kLayoutSequenceDisable is not specified in the layoutFlags field. The constant kSequenceDefaultMask can be used as a default value.
		unsigned_int32			alternateTypeMask;		// A mask that determines which types of alternate substitution features are recognized. A one bit indicates that the corresponding type of alternate substitution is enabled. See the descriptions of the individual bits beginning with "kAlternate". This field is used only if the kLayoutAlternateDisable is not specified in the layoutFlags field. The default value is typically zero to disable all substitutions.
		int32					styleIndex;				// The style index used when the kAlternateStylistic bit is set in the alternateTypeMask field. This value should be in the range 1-20, and it corresponds to the style set defined by the original font.

		GlyphEffectType			effectType;				// The type of the glyph effect to apply. By default, this should be set to kGlyphEffectNone for no glyph effect.
		float					effectOffset[2];		// The x and y offsets at which the glyph effect is rendered, in em units. Positive values offset rightward and downward.
		LayoutColor				effectColor[2];			// The glyph effect colors. These have the same meanings as the textColor fields, but are applied to the glyph effect.
		float					effectGradient[2];		// The y coordinates for the glyph effect gradient. These have the same meanings as the textGradient fields, but are applied to the glyph effect. This field is used only if the kLayoutEffectGradient bit is specified in the layoutFlags field.

		float					clipLeft;				// The x coordinate at which glyphs are clipped on the left. This field is used only if the kLayoutClippingPlanes bit is specified in the layoutFlags field.
		float					clipRight;				// The x coordinate at which glyphs are clipped on the right. This field is used only if the kLayoutClippingPlanes bit is specified in the layoutFlags field.
	};


	// The GetFontHeader() function returns a pointer to the FontHeader structure in the raw data for a .slug file.
	// After a .slug file is loaded into memory, this should be the first function called. The returned pointer is
	// then passed to all other Slug functions when using the font stored in the .slug file.
	//
	// Many data structures inside a .slug file are aligned to 64-byte boundaries to promote good cache performance.
	// In particular, the GlyphData structures are 64-byte aligned and are exactly 64 bytes in size. To benefit
	// from this, the contents of a .slug file must be loaded into a 64-byte aligned region of memory.

	inline const FontHeader *GetFontHeader(const void *slugFile)
	{
		return (reinterpret_cast<const FontHeader *>(static_cast<const char *>(slugFile) + 16));
	}

	// The GetGlyphIndex() function returns the glyph index corresponding to the Unicode character specified by the
	// unicode parameter. If the font does not contain that character, then the return value is zero, which is the
	// valid index for the glyph used in place of a missing character.

	SLUG_API int32 GetGlyphIndex(const FontHeader *fontHeader, unsigned_int32 unicode);

	// The GetGlyphData() function returns a pointer to the GlyphData structure for the Unicode character specified
	// by the unicode parameter. If the font does not contain that character, then the return value is a pointer to
	// the GlyphData structure for the glyph used in place of a missing character.

	inline const GlyphData *GetGlyphData(const FontHeader *fontHeader, unsigned_int32 unicode)
	{
		return (&fontHeader->GetGlyphDataTable()[GetGlyphIndex(fontHeader, unicode)]);
	}

	// The ExtractFontTextures() function decompresses the curve and band texture data for the font and stores it in
	// memory at the locations specified by the curveTexture and bandTexture parameters. This memory storage must be
	// allocated by the caller. The size of the storage is determined by using the dimensions of the textures given
	// by the FontHeader structure. The curveTextureSize and bandTextureSize fields of the FontHeader structure give
	// the texture widths and heights in texels, and each texel is 8 bytes in size. The FontTexel type represents a
	// single texel, so the following expressions can be used to calculate the storage sizes.
	//
	// curveStorageSize = fontHeader->curveTextureSize[0] * fontHeader->curveTextureSize[1] * sizeof(Slug::FontTexel);
	// bandStorageSize = fontHeader->bandTextureSize[0] * fontHeader->bandTextureSize[1] * sizeof(Slug::FontTexel);
	//
	// Once the texture data has been extracted, it can be passed to the rendering API to be used by the glyph shader.
	// The caller is responsible for releasing the texture storage when it is no longer needed.

	SLUG_API void ExtractFontTextures(const FontHeader *fontHeader, void *curveTexture, void *bandTexture);

	// The GetShaderSourceCode() function returns pointers to the source code for the vertex shader and pixel shader
	// used to render glyphs. The function stores the pointers in the locations specified by the vertexShader and
	// pixelShader parameters. The source code is not copied, so no memory needs to be allocated.
	//
	// The vertex shader has a uniform input named "vparam" that consists of four 4D vectors representing the rows of
	// the 4x4 model-view-projection matrix. The vertex shader also has four varying inputs starting at attribute index 0,
	// and these correspond to the fields of the GlyphVertex structure.

	SLUG_API void GetShaderSourceCode(const char **vertexShader, const char **pixelShader);

	// The GetShaderIndices() function returns the internal indices of the vertex shader and pixel shader used to
	// render text with a given font and layout options. The value returned in vertexIndex is always less than
	// kMaxVertexShaderCount, and the value returned in pixelIndex is always less than kMaxPixelShaderCount.
	// These indices should be passed to the GetOpenGLShaderProlog() function when rendering with OpenGL and
	// to the GetD3D11ShaderDefines() function when rendering with D3D11.

	SLUG_API void GetShaderIndices(const FontHeader *fontHeader, const LayoutData *layoutData, unsigned_int32 *vertexIndex, unsigned_int32 *pixelIndex);

	// The GetOpenGLShaderProlog() function returns prologs that must accompany the vertex and pixel shaders when
	// using OpenGL to render. Two strings must be passed to the glShaderSource() function. The first string is the
	// prolog returned by the GetOpenGLShaderProlog() function, and the second string is the source code returned
	// by the GetShaderSourceCode() function.

	SLUG_API void GetOpenGLShaderProlog(unsigned_int32 vertexIndex, unsigned_int32 pixelIndex, const char **vertexProlog, const char **pixelProlog);

	// The GetD3D11ShaderDefines() function returns an array of preprocessor defines that must accompany the vertex
	// and pixel shaders when using D3D11 to render. The returned values should be cast to pointers to D3D_SHADER_MACRO
	// structures and passed to the D3DCompile() function along with the source code returned by the GetShaderSourceCode()
	// function.

	SLUG_API void GetD3D11ShaderDefines(unsigned_int32 vertexIndex, unsigned_int32 pixelIndex, const char *(**vertexDefines)[2], const char *(**pixelDefines)[2]);

	// The MeasureSlug() function calculates the overall width of a line of text and the maximum numbers of vertices and
	// triangles that will be generated by the BuildSlug() function. The text parameter should point to a null-terminated
	// string of characters encoded as UTF-8.
	//
	// The physical width of the entire string is stored in the location pointed to by the textWidth parameter. The textWidth
	// parameter can be nullptr if the width is not needed.
	//
	// The maximum number of vertices and maximum number of triangles are stored in the locations pointed to by the vertexCount
	// and triangleCount parameters. The vertex count and triangle count are always returned, and the corresponding parameters
	// cannot be nullptr. This information can be used to allocate vertex buffers of the proper size before calling the
	// BuildSlug() function to fill them with data. The BuildSlug() function may generate fewer than the maximum numbers of
	// vertices and triangles when clipping planes are enabled.
	//
	// Any characters in the string having a Unicode value less than 32 are ignored. These characters never contribute to
	// the overall width of the text, even if the font defines nonzero advance widths for them, and they never contribute
	// to the vertex and triangle counts.

	SLUG_API void MeasureSlug(const FontHeader *fontHeader, const LayoutData *layoutData, const char *text, float *textWidth, int32 *vertexCount, int32 *triangleCount);

	// The BuildSlug() function generates the vertices and triangles for a line of text, or "slug". The text parameter
	// should point to a null-terminated string of characters encoded as UTF-8. The x and y parameters specify the
	// coordinates of the left side of the first glyph at the baseline. The glyphBuffer parameter points to a GlyphBuffer
	// structure containing the addresses of the storage into which vertex and triangle data are written.
	//
	// Upon return from the BuildSlug() function, the GlyphBuffer structure is updated so that the glyphVertex and
	// glyphTriangle fields point to the next element past the end of the data that was written. The vertexIndex field
	// is advanced to one greater than the largest vertex index written. This updated information allows for multiple
	// slugs having the same shaders to be built in the same vertex buffer and drawn with a single rendering command.
	//
	// The actual numbers of vertices and triangles generated by the BuildSlug() function should be determined by examining
	// the pointers in the GlyphBuffer structure upon return and subtracting the original values of those pointers.
	// The resulting differences can be less than the maximum values returned by the MeasureSlug() function in the case
	// that clipping planes are enabled.
	//
	// Any characters in the string having a Unicode value less than 32 are ignored. These characters never contribute
	// any spacing in the slug layout, even if the font defines nonzero advance widths for them, and they never cause
	// any vertices or triangles to be generated.

	SLUG_API void BuildSlug(const FontHeader *fontHeader, const LayoutData *layoutData, const char *text, float x, float y, GlyphBuffer *glyphBuffer);
}


// Formatting Directives
// ==========================
//
// When the kLayoutFormatDirectives bit is set in the layoutFlags field of the LayoutData structure, Slug recognizes
// formatting directives embedded in the text strings passed to the MeasureSlug() and BuildSlug() functions. These
// directives allow properties of the text, such as the size and color, to be changed dynamically within a single line.
//
// A list of formatting directives begins with the two-character sequence "{#" and ends with the closing brace "}".
// Each directive has the form "directive(params)", where "params" lists the parameters that apply to the directive.
// The following table lists the formatting directives recognized by Slug.
//
// size(value)						Set the font size to value in pixels, where value is a floating-point number. Ignored if value is not greater than zero.
// scale(value)						Set the text scale to value, where value is a floating-point number. Ignored if value is not greater than zero.
// shift(value)						Set the text shift to value, where value is a floating-point number.
// track(value)						Set the text tracking to value in em units, where value is a floating-point number.
// skew(value)						Set the text skew to value, where value is a floating-point number. Positive values skew to the right, and negative values skew to the left.
// kern(value)						Set the glyph kerning state to value, where value is either "true" or "false".
// mark(value)						Set the combining mark positioning state to value, where value is either "true" or "false".
// seq(value)						Set the sequence replacement state to value, where value is either "true" or "false".
// alt(value)						Set the alternate substitution state to value, where value is either "true" or "false".
// color(red,green,blue,alpha)		Set the primary text color to (red, green, blue, alpha), where each component is an integer in the range [0,255]. The red, green, and blue components are specified in the sRGB color space, and the alpha component is linear. The alpha component may be omitted, in which case it is 255 by default. Ignored if any component is out of range.
// color2(red,green,blue,alpha)		Set the secondary text color to (red, green, blue, alpha) using the same format as the primary color. The secondary color is used only when gradients are enabled.
// gcoord(y1,y2)					Set the gradient coordinates to y1 and y2. These are the distances above the baseline at which the gradient is equal to the primary and secondary color, respectively. Negative values are allowed.
// grad(value)						Set the gradient state to value, where value is either "true" or "false".
// reset()							Reset all formatting state to the initial values given by the layout data.
//
// When sequence replacement is enabled, the following directives can be used to control what types of sequences are recognized.
//
// comp(value)						Set the glyph composition state to value, where value is either "true" or "false".
// slig(value)						Set the standard ligatures state to value, where value is either "true" or "false".
// rlig(value)						Set the required ligatures state to value, where value is either "true" or "false".
// dlig(value)						Set the discretionary ligatures state to value, where value is either "true" or "false".
// hlig(value)						Set the historical ligatures state to value, where value is either "true" or "false".
//
// When alternate substitution is enabled, the following directives can be used to control what types of alternates are applied.
//
// style(value)						Set the stylistic alternates state to value, where value is an integer in the range 0-20. If value is nonzero, stylistic alternates are enabled, and they use the set specified by value. If value is zero, stylistic alternates are disabled. Ignored if value is out of range.
// historical(value)				Set the historical alternates state to value, where value is either "true" or "false".
// smallcap(value)					Set the lowercase small caps state to value, where value is either "true" or "false".
// capsmall(value)					Set the uppercase small caps state to value, where value is either "true" or "false".
// titling(value)					Set the titling caps state to value, where value is either "true" or "false".
// unicase(value)					Set the unicase state to value, where value is either "true" or "false".
// caseform(value)					Set the case-sensitive forms state to value, where value is either "true" or "false".
// slashzero(value)					Set the slashed zero state to value, where value is either "true" or "false".
// lining(value)					Set the lining figures state to value, where value is either "true" or "false". If value is "true", then the oldstyle figures state is disabled.
// oldstyle(value)					Set the oldstyle figures state to value, where value is either "true" or "false". If value is "true", then the lining figures state is disabled.
// tabfig(value)					Set the tabular figures state to value, where value is either "true" or "false". If value is "true", then the proportional figures state is disabled.
// propfig(value)					Set the proportional figures state to value, where value is either "true" or "false". If value is "true", then the tabular figures state is disabled.
// sub(value)						Set the subscript state to value, where value is either "true" or "false". If value is "true", then the superscript, scientific inferiors, and ordinals states are disabled.
// sup(value)						Set the superscript state to value, where value is either "true" or "false". If value is "true", then the subscript, scientific inferiors, and ordinals states are disabled.
// inf(value)						Set the scientific inferiors state to value, where value is either "true" or "false". If value is "true", then the subscript, superscript, and ordinals states are disabled.
// ord(value)						Set the ordinals state to value, where value is either "true" or "false". If value is "true", then the subscript, superscript, and scientific inferiors states are disabled.
//
// Multiple formatting directives may be issued inside one set of braces by separating the directives with semicolons.
// For example, the directive "{#size(24);color(255,0,0)}" sets the font size to 24 and the color to bright red.
//
// Integer and floating-point values accepted by formatting directives are parsed using the OpenDDL syntax (openddl.org).
//
// If formatting directives are not enabled, then any braced sequences of directives appearing in the text string are
// rendered as if they had no meaning.


#endif
