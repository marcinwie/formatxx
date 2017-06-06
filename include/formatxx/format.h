// formatxx - C++ string formatting library.
//
// This is free and unencumbered software released into the public domain.
// 
// Anyone is free to copy, modify, publish, use, compile, sell, or
// distribute this software, either in source code form or as a compiled
// binary, for any purpose, commercial or non - commercial, and by any
// means.
// 
// In jurisdictions that recognize copyright laws, the author or authors
// of this software dedicate any and all copyright interest in the
// software to the public domain. We make this dedication for the benefit
// of the public at large and to the detriment of our heirs and
// successors. We intend this dedication to be an overt act of
// relinquishment in perpetuity of all present and future rights to this
// software under copyright law.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
// IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
// 
// For more information, please refer to <http://unlicense.org/>
//
// Authors:
//   Sean Middleditch <sean@middleditch.us>

#if !defined(_guard_FORMATXX_H)
#define _guard_FORMATXX_H
#pragma once

#include <type_traits>
#include <cstring>
#include <string>

namespace formatxx
{
	template <typename CharT> class basic_string_view;
	template <typename CharT> class basic_format_writer;
	template <typename CharT, std::size_t> class basic_fixed_writer;
	template <typename CharT> class basic_format_spec;
	
	using string_view = basic_string_view<char>;
	using wstring_view = basic_string_view<wchar_t>;
	using format_writer = basic_format_writer<char>;
	using wformat_writer = basic_format_writer<wchar_t>;
	using format_spec = basic_format_spec<char>;
	using wformat_spec = basic_format_spec<wchar_t>;

	template <std::size_t Size = 512> using fixed_writer = basic_fixed_writer<char, Size>;

	template <typename... Args> format_writer& format(format_writer& writer, string_view format, Args const&... args);
	template <typename... Args> format_writer& printf(format_writer& writer, string_view format, Args const&... args);

	template <typename... Args> wformat_writer& format(wformat_writer& writer, wstring_view format, Args const&... args);
	template <typename... Args> wformat_writer& printf(wformat_writer& writer, wstring_view format, Args const&... args);

	template <typename CharT> basic_format_spec<CharT> parse_format_spec(basic_string_view<CharT> spec);

	extern template format_spec parse_format_spec(string_view spec);
	extern template wformat_spec parse_format_spec(wstring_view spec);
}

/// Describes a format string.
template <typename CharT>
class formatxx::basic_string_view
{
public:
	constexpr basic_string_view() = default;
	constexpr basic_string_view(CharT const* first, CharT const* last) : _begin(first), _size(last - first) {}
	constexpr basic_string_view(CharT const* nstr, std::size_t size) : _begin(nstr), _size(size) {}
	basic_string_view(CharT const* zstr) : _begin(zstr), _size(std::char_traits<CharT>::length(zstr)) {}

	constexpr CharT const* data() const { return _begin; }
	constexpr std::size_t size() const { return _size; }
	constexpr bool empty() const { return _size == 0; }

	constexpr CharT const* begin() const { return _begin; }
	constexpr CharT const* end() const { return _begin + _size; }

private:
	CharT const* _begin = nullptr;
	std::size_t _size = 0;
};

/// Interface for any buffer that the format library can write into.
template <typename CharT>
class formatxx::basic_format_writer
{
public:
	virtual ~basic_format_writer() = default;

	/// Write a string slice.
	/// @param str The string to write.
	virtual void write(basic_string_view<CharT> str) = 0;

	/// Extract the current value of the writer.
	virtual basic_string_view<CharT> view() const = 0;
};

/// A writer with a fixed buffer that will never allocate.
template <typename CharT, std::size_t SizeN>
class formatxx::basic_fixed_writer : public basic_format_writer<CharT>
{
public:
	void write(basic_string_view<CharT> str) override;
	basic_string_view<CharT> view() const override { return basic_string_view<CharT>(_buffer, _last); }

	void clear() { _last = _buffer; }
	std::size_t size() const { return _last - _buffer; }
	CharT const* c_str() const { return _buffer; }

private:
	CharT* _last = _buffer;
	CharT _buffer[SizeN] = {CharT(0),};
};

/// Extra formatting specifications.
template <typename CharT>
class formatxx::basic_format_spec
{
public:
	CharT code = 0;
	bool type_prefix = false; // print leading 0x or appropriate type
	enum
	{
		sign_default, // print sign for negative numbers
		sign_always, // print sign for all numbers
		sign_space, // print for for negative numbers or a space for non-negative numbers
	} sign = sign_default;
	formatxx::basic_string_view<CharT> extra;
};

namespace formatxx
{
	/// Default format helpers.
	void format_value(format_writer& out, char* zstr, string_view spec);
	void format_value(format_writer& out, char const* zstr, string_view spec);
	void format_value(format_writer& out, string_view str, string_view spec);
	void format_value(format_writer& out, char ch, string_view spec);
	void format_value(format_writer& out, bool value, string_view spec);
	void format_value(format_writer& out, float value, string_view spec);
	void format_value(format_writer& out, double value, string_view spec);
	void format_value(format_writer& out, signed char value, string_view spec);
	void format_value(format_writer& out, signed int value, string_view spec);
	void format_value(format_writer& out, signed long value, string_view spec);
	void format_value(format_writer& out, signed short value, string_view spec);
	void format_value(format_writer& out, signed long long value, string_view spec);
	void format_value(format_writer& out, unsigned char value, string_view spec);
	void format_value(format_writer& out, unsigned int value, string_view spec);
	void format_value(format_writer& out, unsigned long value, string_view spec);
	void format_value(format_writer& out, unsigned short value, string_view spec);
	void format_value(format_writer& out, unsigned long long value, string_view spec);
	void format_value(format_writer& out, void* value, string_view spec);
	void format_value(format_writer& out, void const* value, string_view spec);

	void format_value(wformat_writer& out, wchar_t* zstr, wstring_view spec);
	void format_value(wformat_writer& out, wchar_t const* zstr, wstring_view spec);
	void format_value(wformat_writer& out, wstring_view str, wstring_view spec);
	void format_value(wformat_writer& out, wchar_t ch, wstring_view spec);
	void format_value(wformat_writer& out, bool value, wstring_view spec);
	void format_value(wformat_writer& out, float value, wstring_view spec);
	void format_value(wformat_writer& out, double value, wstring_view spec);
	void format_value(wformat_writer& out, signed char value, wstring_view spec);
	void format_value(wformat_writer& out, signed int value, wstring_view spec);
	void format_value(wformat_writer& out, signed long value, wstring_view spec);
	void format_value(wformat_writer& out, signed short value, wstring_view spec);
	void format_value(wformat_writer& out, signed long long value, wstring_view spec);
	void format_value(wformat_writer& out, unsigned char value, wstring_view spec);
	void format_value(wformat_writer& out, unsigned int value, wstring_view spec);
	void format_value(wformat_writer& out, unsigned long value, wstring_view spec);
	void format_value(wformat_writer& out, unsigned short value, wstring_view spec);
	void format_value(wformat_writer& out, unsigned long long value, wstring_view spec);
	void format_value(wformat_writer& out, void* value, wstring_view spec);
	void format_value(wformat_writer& out, void const* value, wstring_view spec);

	/// Formatting for enumerations, using their numeric value.
	template <typename EnumT>
	auto format_value(format_writer& out, EnumT value, string_view spec) -> std::enable_if_t<std::is_enum<EnumT>::value>
	{
		format_value(out, std::underlying_type_t<EnumT>(value), spec);
	}

	template <typename PointerT>
	auto format_value(format_writer& out, PointerT value, string_view spec) -> std::enable_if_t<std::is_pointer<PointerT>::value>
	{
		format_value(out, static_cast<void const*>(value), spec);
	}

	/// Cause a friendlier error message on unknown type.
	template <typename T>
	auto format_value(format_writer& writer, T const& value, string_view spec) -> std::enable_if_t<!std::is_enum<T>::value && !std::is_pointer<T>::value> = delete;

	/// @internal
	namespace _detail
	{
		template <typename CharT> using BasicFormatterThunk = void(*)(basic_format_writer<CharT>&, void const*, basic_string_view<CharT>);
		using FormatterParameter = void const*;

		template <typename CharT, typename T>
		void format_value_thunk(basic_format_writer<CharT>& out, void const* ptr, basic_string_view<CharT> spec)
		{
			format_value(out, *static_cast<T const*>(ptr), spec);
		}

		template <typename CharT>
		basic_format_writer<CharT>& format_impl(basic_format_writer<CharT>& out, basic_string_view<CharT> format, std::size_t count, BasicFormatterThunk<CharT> const* funcs, FormatterParameter const* values);
		template <typename CharT>
		basic_format_writer<CharT>& printf_impl(basic_format_writer<CharT>& out, basic_string_view<CharT> format, std::size_t count, BasicFormatterThunk<CharT> const* funcs, FormatterParameter const* values);

		extern template basic_format_writer<char>& format_impl(basic_format_writer<char>& out, basic_string_view<char> format, std::size_t count, BasicFormatterThunk<char> const* funcs, FormatterParameter const* values);
		extern template basic_format_writer<wchar_t>& format_impl(basic_format_writer<wchar_t>& out, basic_string_view<wchar_t> format, std::size_t count, BasicFormatterThunk<wchar_t> const* funcs, FormatterParameter const* values);

		extern template basic_format_writer<char>& printf_impl(basic_format_writer<char>& out, basic_string_view<char> format, std::size_t count, BasicFormatterThunk<char> const* funcs, FormatterParameter const* values);
		extern template basic_format_writer<wchar_t>& printf_impl(basic_format_writer<wchar_t>& out, basic_string_view<wchar_t> format, std::size_t count, BasicFormatterThunk<wchar_t> const* funcs, FormatterParameter const* values);
	}
}

/// Write the string format using the given parameters into a buffer.
/// @param writer The write buffer that will receive the formatted text.
/// @param format The primary text and formatting controls to be written.
/// @param args The arguments used by the formatting string.
template <typename... Args>
formatxx::format_writer& formatxx::format(format_writer& writer, string_view format, Args const&... args)
{
	_detail::FormatterParameter const values[] = {std::addressof(args)..., nullptr};
	_detail::BasicFormatterThunk<char> const funcs[] = {&_detail::format_value_thunk<char, Args>..., nullptr};

	return _detail::format_impl(writer, format, sizeof...(args), funcs, values);
}
template <typename... Args>
formatxx::wformat_writer& formatxx::format(wformat_writer& writer, wstring_view format, Args const&... args)
{
	_detail::FormatterParameter const values[] = {std::addressof(args)..., nullptr};
	_detail::BasicFormatterThunk<wchar_t> const funcs[] = {&_detail::format_value_thunk<wchar_t, Args>..., nullptr};

	return _detail::format_impl(writer, format, sizeof...(args), funcs, values);
}

/// Write the printf format using the given parameters into a buffer.
/// @param writer The write buffer that will receive the formatted text.
/// @param format The primary text and printf controls to be written.
/// @param args The arguments used by the formatting string.
template <typename... Args>
formatxx::format_writer& formatxx::printf(format_writer& writer, string_view format, Args const&... args)
{
	_detail::FormatterParameter const values[] = {std::addressof(args)..., nullptr};
	_detail::BasicFormatterThunk<char> const funcs[] = {&_detail::format_value_thunk<char, Args>..., nullptr};

	return _detail::printf_impl(writer, format, sizeof...(args), funcs, values);
}

template <typename CharT, std::size_t SizeN>
void formatxx::basic_fixed_writer<CharT, SizeN>::write(basic_string_view<CharT> str)
{
	std::size_t const remaining = SizeN - size() - 1;
	std::size_t const length = remaining < str.size() ? remaining : str.size();
	std::memcpy(_last, str.data(), length * sizeof(CharT));
	_last += length;
	*_last = CharT(0);
}

#endif // !defined(_guard_FORMATXX_H)
