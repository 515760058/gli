namespace gli
{
	inline texture_cube::texture_cube()
	{}

	inline texture_cube::texture_cube(format_type Format, extent_type const& Extent, swizzles_type const& Swizzles)
		: texture(TARGET_CUBE, Format, texture::extent_type(Extent, 1), 1, 6, gli::levels(Extent), Swizzles)
	{
		this->build_cache();
	}

	inline texture_cube::texture_cube(format_type Format, extent_type const& Extent, size_type Levels, swizzles_type const& Swizzles)
		: texture(TARGET_CUBE, Format, texture::extent_type(Extent, 1), 1, 6, Levels, Swizzles)
	{
		this->build_cache();
	}

	inline texture_cube::texture_cube(texture const& Texture)
		: texture(Texture, TARGET_CUBE, Texture.format())
	{
		this->build_cache();
	}

	inline texture_cube::texture_cube
	(
		texture const& Texture,
		format_type Format,
		size_type BaseLayer, size_type MaxLayer,
		size_type BaseFace, size_type MaxFace,
		size_type BaseLevel, size_type MaxLevel,
		swizzles_type const& Swizzles
	)
		: texture(
			Texture, TARGET_CUBE, Format,
			BaseLayer, MaxLayer,
			BaseFace, MaxFace,
			BaseLevel, MaxLevel,
			Swizzles)
	{
		this->build_cache();
	}

	inline texture_cube::texture_cube
	(
		texture_cube const& Texture,
		size_type BaseFace, size_type MaxFace,
		size_type BaseLevel, size_type MaxLevel
	)
		: texture(
			Texture, TARGET_CUBE, Texture.format(),
			Texture.base_layer(), Texture.max_layer(),
			Texture.base_face() + BaseFace, Texture.base_face() + MaxFace,
			Texture.base_level() + BaseLevel, Texture.base_level() + MaxLevel)
	{
		this->build_cache();
	}

	inline texture2d texture_cube::operator[](size_type Face) const
	{
		GLI_ASSERT(Face < this->faces());

		return texture2d(
			*this, this->format(),
			this->base_layer(), this->max_layer(),
			this->base_face() + Face, this->base_face() + Face,
			this->base_level(), this->max_level());
	}

	inline texture_cube::extent_type texture_cube::extent(size_type Level) const
	{
		return extent_type(this->texture::extent(Level));
	}

	template <typename gen_type>
	inline gen_type texture_cube::load(extent_type const& TexelCoord, size_type Face, size_type Level) const
	{
		GLI_ASSERT(!this->empty());
		GLI_ASSERT(!is_compressed(this->format()));
		GLI_ASSERT(block_size(this->format()) == sizeof(gen_type));
		GLI_ASSERT(Level < this->levels());

		size_type const ImageOffset = this->Storage->image_offset(TexelCoord, this->extent(Level));
		GLI_ASSERT(ImageOffset < this->size<gen_type>(Level));

		return *(this->data<gen_type>(0, Face, Level) + ImageOffset);
	}

	template <typename gen_type>
	inline void texture_cube::store(extent_type const& TexelCoord, size_type Face, size_type Level, gen_type const& Texel)
	{
		GLI_ASSERT(!this->empty());
		GLI_ASSERT(!is_compressed(this->format()));
		GLI_ASSERT(block_size(this->format()) == sizeof(gen_type));
		GLI_ASSERT(Level < this->levels());
		GLI_ASSERT(glm::all(glm::lessThan(TexelCoord, this->extent(Level))));

		size_type const ImageOffset = this->Storage->image_offset(TexelCoord, this->extent(Level));
		GLI_ASSERT(ImageOffset < this->size<gen_type>(Level));

		*(this->data<gen_type>(0, Face, Level) + ImageOffset) = Texel;
	}

	inline texture_cube::size_type texture_cube::index_cache(size_type Face, size_type Level) const
	{
		return Face * this->levels() + Level;
	}

	inline void texture_cube::build_cache()
	{
		this->Caches.resize(this->faces() * this->levels());

		for(size_type Face = 0; Face < this->faces(); ++Face)
		for(size_type Level = 0; Level < this->levels(); ++Level)
		{
			cache& Cache = this->Caches[this->index_cache(Face, Level)];
			Cache.ImageExtent = glm::max(extent_type(this->texture::extent(Level)), extent_type(1));
		}
	}
}//namespace gli
