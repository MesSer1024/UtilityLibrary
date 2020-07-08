#pragma once

namespace ddahlkvist
{
	using u8 = unsigned char;
	using u16 = unsigned short;
	using u32 = unsigned long;
	using u64 = unsigned long long;
	
	using s8 = signed char;
	using s16 = signed short;
	using s32 = signed long;
	using s64 = signed long long;
	
	using uint = u32;
	using usize = u64;
	using uptr = u64;

	static_assert(sizeof(u8) == 1u);
	static_assert(sizeof(u16) == 2u);
	static_assert(sizeof(u32) == 4u);
	static_assert(sizeof(u64) == 8u);

	static_assert(sizeof(s8) == sizeof(u8));
	static_assert(sizeof(s16) == sizeof(u16));
	static_assert(sizeof(s32) == sizeof(u32));
	static_assert(sizeof(s64) == sizeof(u64));

	static_assert(sizeof(uint) == sizeof(u32));
	static_assert(sizeof(usize) == sizeof(u64));
	static_assert(sizeof(uptr) == sizeof(u64));

	static_assert(alignof(s8) == 1);
	static_assert(alignof(s16) == 2);
	static_assert(alignof(s32) == 4);
	static_assert(alignof(s64) == 8);

	static_assert(alignof(s8) == alignof(u8));
	static_assert(alignof(s16) == alignof(u16));
	static_assert(alignof(s32) == alignof(u32));
	static_assert(alignof(s64) == alignof(u64));

	static_assert(alignof(uint) == alignof(u32));
	static_assert(alignof(usize) == alignof(u64));
	static_assert(alignof(uptr) == alignof(u64));
}