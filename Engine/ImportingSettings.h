#pragma once

enum Axis
{
	X,
	Y,
	Z,
	MINUS_X,
	MINUS_Y,
	MINUS_Z
};

struct ModelImportingSettings
{
	float globalScale = 1.0f;
	Axis forwardAxis = Axis::Z;
	Axis upAxis = Axis::Y;
	bool normalizeScales = true;
	bool ignoreCameras = false;
	bool ignoreLights = false;
};

enum class TextureWrap
{
	CLAMP_TO_BORDER,
	CLAMP,
	REPEAT,
	MIRRORED_REPEAT
};

enum class TextureFiltering
{
	NEAREST,
	LINEAR
};

struct TextureImportingSettings
{
	TextureWrap textureWrap = TextureWrap::REPEAT;
	TextureFiltering textureFiltering = TextureFiltering::NEAREST;
	bool flip = false;
	bool alienify = false;
	bool blurAverage = false;
	int blur_average_iterations = 1;
	bool blurGaussian = false;
	int blurGaussianIterations = 1;
	bool equalize = false;
	bool negativity = false;
	bool noise = false;
	bool pixelize = false;
	int pixelizeSize = 1;
	bool sharpening = false;
	float sharpeningFactor = 1.0f;
	int sharpeningIterations = 1;
	float noiseTolerance = 0.5f;
	float gammaCorrection = 1.0f;
	float contrast = 1.0f;
};