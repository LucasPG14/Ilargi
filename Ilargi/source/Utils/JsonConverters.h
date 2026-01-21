#pragma once

#include <detail/type_vec2.hpp>
#include <detail/type_vec3.hpp>
#include <detail/type_vec4.hpp>
#include <ArduinoJson-v7.0.4.h>

template <>
struct Converter<glm::vec2>
{
	static bool toJson(const glm::vec2& src, JsonVariant dst)
	{
		dst["x"] = src.x;
		dst["y"] = src.y;
		return true;
	}

	static glm::vec2 fromJson(JsonVariantConst src)
	{
		return glm::vec2(src["x"], src["y"]);
	}

	static bool checkJson(JsonVariantConst src)
	{
		return src["x"].is<float>() && src["y"].is<float>();
	}
};

template <>
struct Converter<glm::vec3>
{
	static bool toJson(const glm::vec3& src, JsonVariant dst)
	{
		dst["x"] = src.x;
		dst["y"] = src.y;
		dst["z"] = src.z;
		return true;
	}

	static glm::vec3 fromJson(JsonVariantConst src)
	{
		return glm::vec3(src["x"], src["y"], src["z"]);
	}

	static bool checkJson(JsonVariantConst src)
	{
		return src["x"].is<float>() && src["y"].is<float>() && src["z"].is<float>();
	}
};

template <>
struct Converter<glm::vec4>
{
	static bool toJson(const glm::vec4& src, JsonVariant dst)
	{
		dst["x"] = src.x;
		dst["y"] = src.y;
		dst["z"] = src.z;
		dst["w"] = src.w;
		return true;
	}

	static glm::vec4 fromJson(JsonVariantConst src)
	{
		return { src["x"], src["y"], src["z"], src["w"] };
	}

	static bool checkJson(JsonVariantConst src)
	{
		return src["x"].is<float>() && src["y"].is<float>() && src["z"].is<float>() && src["w"].is<float>();
	}
};