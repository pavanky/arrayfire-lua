#include "utils.h"

af_dtype GetDataType (lua_State * L, int index)
{
	af_dtype types[] = {
		f32, c32, f64, c64, b8, s32, u32, u8, s64, u64,
		// s16, u16???
	};

	const char * names[] = {
		"f32", "c32", "f64", "c64", "b8", "s32", "u32", "u8", "s64", "u64"
	};

	return types[luaL_checkoption(L, index, "f32", names)];
}

#define RESULT_CODE(what) case what: \
	lua_pushliteral(L, #what);		 \
	break;

void PushResult (lua_State * L, af_err err)
{
	switch (err)
	{
	RESULT_CODE(AF_SUCCESS)
	RESULT_CODE(AF_ERR_NO_MEM)
	RESULT_CODE(AF_ERR_DRIVER)
	RESULT_CODE(AF_ERR_RUNTIME)
	RESULT_CODE(AF_ERR_INVALID_ARRAY)
	RESULT_CODE(AF_ERR_ARG)
	RESULT_CODE(AF_ERR_SIZE)
	RESULT_CODE(AF_ERR_TYPE)
	RESULT_CODE(AF_ERR_DIFF_TYPE)
	RESULT_CODE(AF_ERR_BATCH)
	RESULT_CODE(AF_ERR_NOT_SUPPORTED)
	RESULT_CODE(AF_ERR_NOT_CONFIGURED)
	RESULT_CODE(AFF_ERR_NONFREE)
	RESULT_CODE(AF_ERR_NO_DBL)
	RESULT_CODE(AF_ERR_NO_GFX)
	RESULT_CODE(AF_ERR_INTERNAL)
	RESULT_CODE(AF_ERR_UNKNOWN)
	}
}

#undef RESULT_CODE

af_array GetArray (lua_State * L, int index)
{
	return *(af_array *)lua_touserdata(L, index);
}

void * NewArray (lua_State * L)
{
	void * amem = lua_newuserdata(L, sizeof(af_array)); // amem

	luaL_newmetatable(L, "af_array"); // amem, mt
	lua_setmetatable(L, -2); // amem

	return amem;
}

LuaDimsAndType::LuaDimsAndType (lua_State * L, int first)
{
	luaL_checktype(L, first + 1, LUA_TTABLE);

	int n;

	if (!lua_isnil(L, first)) n = lua_tointeger(L, first);

	else n = lua_objlen(L, first + 1);

	for (int i = 0; i < n; ++i)
	{
		lua_rawgeti(L, first + 1, i + 1);	// ..., n?, t[, type], ..., dim

		mDims.push_back(lua_tointeger(L, -1));

		lua_pop(L, 1);	// ..., n?, t[, type], ...
	}

	mType = GetDataType(L, first + 2);
}

template<typename T> void AddToVector (std::vector<char> & arr, T value)
{
	const char * bytes = (const char *)&value;

	for (auto i = arr.size(); i < arr.size() + sizeof(T); ++i) arr[i] = *bytes++;
}

template<typename T> T Float (lua_State * L, int index)
{
	return (T)lua_tonumber(L, index);
}

template<typename T> T Int (lua_State * L, int index)
{
	return (T)lua_tointeger(L, index);
}

template<typename T> void AddFloat (std::vector<char> & arr, lua_State * L, int index, int pos)
{
	lua_rawgeti(L, index, pos + 1);	// ..., arr, ..., float

	AddToVector(arr, Float<T>(L, -1));

	lua_pop(L, 1); // ..., arr, ...
}

template<typename T> void AddInt (std::vector<char> & arr, lua_State * L, int index, int pos)
{
	lua_rawgeti(L, index, pos + 1);	// ..., arr, ..., int

	AddToVector(arr, Int<T>(L, -1));

	lua_pop(L, 1); // ..., arr, ...
}

LuaData::LuaData (lua_State * L, int index, af_dtype type) : mType(type)
{
	// Non-string: build up from Lua array.
	if (!lua_isstring(L, index))
	{
		int count = lua_objlen(L, index);

		switch (type)
		{
		case b8:
		case u8:
			mData.reserve(count);
			break;
		// Docs list an s16, u16... ??
		case f32:
		case s32:
		case u32:
			mData.reserve(count * 4);
			break;
		case c32:
		case f64:
		case s64:
		case u64:
			mData.reserve(count * 8);
			break;
		case c64:
			mData.reserve(count * 16);
			break;
		}

		for (int i = 0; i < count; ++i)
		{
			switch (type)
			{
			case f32:
				AddFloat<float>(mData, L, index, i);
				break;
			case c32:
				// ?
				break;
			case f64:
				AddFloat<double>(mData, L, index, i);
				break;
			case c64:
				// ?
				break;
			case b8:
				AddInt<char>(mData, L, index, i);
				break;
			case s32:
				AddInt<int>(mData, L, index, i);
				break;
			case u32:
				AddInt<unsigned int>(mData, L, index, i);
				break;
			case u8:
				AddInt<unsigned char>(mData, L, index, i);
				break;
			case s64:
				AddInt<intl>(mData, L, index, i);
				break;
			case u64:
				AddInt<uintl>(mData, L, index, i);
				break;
			}
		}
	}

	// Already a Lua string: make a copy.
	else
	{
		const char * str = lua_tostring(L, index);

		mData.assign(str, str + lua_objlen(L, index));
	}
}