#ifndef __CRON_STRING_FUNC_H__
#define __CRON_STRING_FUNC_H__

#include <string>
#include <algorithm>
#include <vector>

template <typename T>
struct general_const_strings
{
    static const T blank_str;
    static const typename T::value_type quote;
};
// FUCK GCC...
#ifdef _MSC_VER
#   define WEAK_SYMBOL __declspec(selectany)
#else
#   define WEAK_SYMBOL __attribute__((weak))
#endif // _MSC_VER
template<>
WEAK_SYMBOL const  std::string general_const_strings<std::string>::blank_str = " \t\r\n";
template<>
WEAK_SYMBOL const  std::wstring general_const_strings<std::wstring>::blank_str = L" \t\r\n";
template<>
WEAK_SYMBOL const  std::string::value_type general_const_strings<std::string>::quote = '"';
template<>
WEAK_SYMBOL const  std::wstring::value_type general_const_strings<std::wstring>::quote = L'"';

template <typename T>
T TrimLeft(const T& str){
    
    const T& sig = general_const_strings<T>::blank_str;
    int index = 0;
    int len = (int)str.size();
    for (; index < len; index++) {
        if ( sig.find(str[index]) == T::npos ) {
            break;
        }
    }
    return str.substr(index, len - index);
}

template <typename T>
T TrimRight(const T& str)
{
    const T& sig = general_const_strings<T>::blank_str;
    int len = (int)str.size();
    int index = len - 1;
    for (; index >= 0; index--) {
        if ( sig.rfind(str[index]) == T::npos ) {
            break;
        }
    }
    return str.substr(0, index + 1);
}

template <typename T>
T TrimString(const T& str)
{
    return TrimLeft(TrimRight(str));
}

template <class T>
std::vector<T> Fields(const T & str) {
    std::vector<T> ret;
    const T& sig = general_const_strings<T>::blank_str;
    T base = str ;
REDO:
    base = TrimString(base);
    if(base.empty()) {
        return ret;
    }
    for( size_t i = 0 ; i < base.size() ; i ++ ) {
        if ( sig.find(base[i]) != T::npos ) {
            ret.push_back(base.substr(0,i));          
            T next = base.substr(i , base.size() - i);
            base = next ;
            goto REDO;
        }
    }
    // base has only 1 filed now 
    ret.push_back(base);
    return ret;
}

template <typename T>
std::vector<T> SplitString(const T& input, typename T::value_type splitter, bool need_null_str = false, bool need_trim = true)
{
    std::vector<T> tmp;

    auto add_to_vec = [&](const T& tmp_str) {
        T str(tmp_str);
        if (need_trim) {
            str = TrimString(str);
        }
        if (!str.empty() || need_null_str) {
            tmp.push_back(str);
        }
    };

    size_t start = 0;
    size_t pos = input.find(splitter, start);
    while (pos != T::npos) {
        T tmp_str(input.substr(start, pos - start));
        add_to_vec(tmp_str);
        start = pos + 1;
        pos = input.find(splitter, start);
    }
    // last one
    if (start != input.size()) {
        T tmp_str(input.substr(start));
        add_to_vec(tmp_str);
    } else {
        add_to_vec(T());
    }
    return tmp;
}

inline std::string ToLower(const std::string & str) {
    std::string  ret(str);
    std::transform(ret.begin() ,ret.end() , ret.begin() ,::tolower);
    return ret;
}
#endif
