/**
 * @file IUtility.h
 * @brief   The utility functions of project.
 * @author Antmuse|Email:antmuse@live.cn
 * @version 0.3.1.6
 * @date 2010年8月28日, 下午2:26
 */

#ifndef APP_IUTILITY_H
#define	APP_IUTILITY_H

#include "HConfig.h"
#include "CString.h"
#include "fast_atof.h"

namespace app {
namespace core {

extern core::CWString APP_LOCALE_DECIMAL_POINTS;

//! Convert a simple string of base 10 digits into an unsigned 32 bit integer.
/** \param[in] in: The string of digits to convert. No leading chars are
allowed, only digits 0 to 9. Parsing stops at the first non-digit.
\param[out] out: (optional) If provided, it will be set to point at the
first character not used in the calculation.
\return The unsigned integer value of the digits. If the string specifies
too many digits to encode in an u32 then INT_MAX will be returned.
*/
inline u32 strtoul10(const wchar_t* in, const wchar_t** out = 0) {
    if(!in) {
        if(out)
            * out = in;
        return 0;
    }

    bool overflow = false;
    u32 unsignedValue = 0;
    while((*in >= L'0') && (*in <= L'9')) {
        const u32 tmp = (unsignedValue * 10) + (*in - L'0');
        if(tmp < unsignedValue) {
            unsignedValue = (u32)0xffffffff;
            overflow = true;
        }
        if(!overflow)
            unsignedValue = tmp;
        ++in;
    }

    if(out)
        * out = in;

    return unsignedValue;
}


//! Convert a simple string of base 10 digits into a signed 32 bit integer.
/** \param[in] in: The string of digits to convert. Only a leading - or +
followed by digits 0 to 9 will be considered. Parsing stops at the first
non-digit.
\param[out] out: (optional) If provided, it will be set to point at the
first character not used in the calculation.
\return The signed integer value of the digits. If the string specifies
too many digits to encode in an s32 then +INT_MAX or -INT_MAX will be
returned.
*/
inline s32 strtol10(const wchar_t* in, const wchar_t** out = 0) {
    if(!in) {
        if(out)
            * out = in;
        return 0;
    }

    const bool negative = (L'-' == *in);
    if(negative || (L'+' == *in))
        ++in;

    const u32 unsignedValue = strtoul10(in, out);
    if(unsignedValue > (u32)INT_MAX) {
        if(negative)
            return (s32)INT_MIN;
        else
            return (s32)INT_MAX;
    } else {
        if(negative)
            return -((s32)unsignedValue);
        else
            return (s32)unsignedValue;
    }
}



//! Converts a sequence of digits into a whole positive floating point value.
/** Only digits 0 to 9 are parsed.  Parsing stops at any other character,
including sign characters or a decimal point.
\param in: the sequence of digits to convert.
\param out: (optional) will be set to point at the first non-converted
character.
\return The whole positive floating point representation of the digit
sequence.
*/
inline f32 strtof10(const wchar_t* in, const wchar_t** out = 0) {
    if(!in) {
        if(out)
            * out = in;
        return 0.f;
    }

    const u32 MAX_SAFE_U32_VALUE = UINT_MAX / 10 - 10;
    u32 intValue = 0;

    // Use integer arithmetic for as long as possible, for speed
    // and precision.
    while((*in >= L'0') && (*in <= L'9')) {
        // If it looks like we're going to overflow, bail out
        // now and start using floating point.
        if(intValue >= MAX_SAFE_U32_VALUE)
            break;

        intValue = (intValue * 10) + (*in - L'0');
        ++in;
    }

    f32 floatValue = (f32)intValue;

    // If there are any digits left to parse, then we need to use
    // floating point arithmetic from here.
    while((*in >= L'0') && (*in <= L'9')) {
        floatValue = (floatValue * 10.f) + (f32)(*in - L'0');
        ++in;
        if(floatValue > FLT_MAX) // Just give up.
            break;
    }

    if(out)
        * out = in;

    return floatValue;
}


//! Provides a fast function for converting a string into a float.
/** This is not guaranteed to be as accurate as atof(), but is
approximately 6 to 8 times as fast.
\param in: the sequence of digits to convert.
\param out: (optional) will be set to point at the first non-converted
character.
\return The floating point representation of the digit
*/
inline const f32 AppStringToF32(const wchar_t* in, const wchar_t** out = 0) {
    if(!in) {
        if(out)
            * out = in;
        return 0.f;
    }
    // Please run the regression test when making any modifications to this function.
    const bool negative = (L'-' == *in);
    if(negative || (L'+' == *in))
        ++in;

    f32 value = strtof10(in, &in);

    if(APP_LOCALE_DECIMAL_POINTS.findFirst(*in) >= 0) {
        const wchar_t* afterDecimal = ++in;
        const f32 decimal = strtof10(in, &afterDecimal);
        value += decimal * core::fast_atof_table[afterDecimal - in];
        in = afterDecimal;
    }

    if(L'e' == *in || L'E' == *in) {
        ++in;
        // Assume that the exponent is a whole number.
        // strtol10() will deal with both + and - signs,
        // but calculate as f32 to prevent overflow at FLT_MAX
        value *= powf(10.f, (f32)strtol10(in, &in));
    }

    if(out)
        * out = in;

    return negative ? -value : value;
}

/**
 * @brief Copy maxlen wchars from pSrc to pDest,  if in face with div char , then stop copy and return.
 * @param pSrc The source string.
 * @param pDest The dest string.
 * @param maxLen The max length of copy.
 * @param div The divide char for break.
 * @return Number of copied.
 */
static s32 AppCopyString(const wchar_t* pSrc, wchar_t* pDest, s32 maxLen, wchar_t div = L'\0') {
    --maxLen;//leave space for string_end '\0'
    if(pSrc == 0 || pDest == 0) {
        return 0;
    }
    s32 indx = 0;
    while(*pSrc != 0 && *pSrc != div && indx < maxLen) {
        *pDest++ = *pSrc++;
        ++indx;
    }
    *pDest = L'\0';
    return indx;
}
static s32 AppCopyString(const s8* in, s32 inmax, wchar_t* out, s32 outmax) {
    if(in == 0 || out == 0) {
        return 0;
    }
    --outmax;//leave space for string_end '\0'
    s32 inp = 0, outp = 0;
    for(; inp < inmax && outp < outmax && '\0' != *in; ++outp, ++inp) {
        *out++ = *in++;
    }
    *out = L'\0';
    return outp;
}


static void AppStringSwapEndian(wchar_t* str, s32 len) {
    APP_ASSERT(str);
    for(--len; len >= 0; --len) {
        *(str + len) = APP_SWAP16(*(str + len));
    }
}


APP_FORCE_INLINE static bool AppisSmallEndian() {
    const u16 it = 0xFF00;
    //return ((c[0] == 0x00) && (c[1] == 0xFF));
    return (0x00 == (*(u8*)(&it)));
}


/* encode 8 bits unsigned int */
APP_FORCE_INLINE static s8* AppEncodeU8(const u8 c, s8* p) {
    *(u8*)p++ = c;
    return p;
}

/* decode 8 bits unsigned int */
APP_FORCE_INLINE static const s8* AppDecodeU8(const s8* p, u8* c) {
    *c = *(u8*)p++;
    return p;
}

/* encode 16 bits unsigned int (lsb) */
APP_FORCE_INLINE static s8* AppEncodeU16(const u16 w, s8* p) {
#ifdef APP_ENDIAN_BIG
    * (u8*)(p + 0) = (w & 255);
    *(u8*)(p + 1) = (w >> 8);
#else
    * (u16*)(p) = w;
#endif
    p += 2;
    return p;
}

/* decode 16 bits unsigned int (lsb) */
APP_FORCE_INLINE static const s8* AppDecodeU16(const s8* p, u16* w) {
#ifdef APP_ENDIAN_BIG
    * w = *(const u8*)(p + 1);
    *w = *(const u8*)(p + 0) + (*w << 8);
#else
    * w = *(const u16*)p;
#endif
    p += 2;
    return p;
}

/* encode 32 bits unsigned int (lsb) */
APP_FORCE_INLINE static s8* AppEncodeU32(const u32 l, s8* p) {
#ifdef APP_ENDIAN_BIG
    * (u8*)(p + 0) = (u8)((l >> 0) & 0xff);
    *(u8*)(p + 1) = (u8)((l >> 8) & 0xff);
    *(u8*)(p + 2) = (u8)((l >> 16) & 0xff);
    *(u8*)(p + 3) = (u8)((l >> 24) & 0xff);
#else
    * (u32*)p = l;
#endif
    p += 4;
    return p;
}

/* decode 32 bits unsigned int (lsb) */
APP_FORCE_INLINE static const s8* AppDecodeU32(const s8* p, u32* l) {
#ifdef APP_ENDIAN_BIG
    * l = *(const u8*)(p + 3);
    *l = *(const u8*)(p + 2) + (*l << 8);
    *l = *(const u8*)(p + 1) + (*l << 8);
    *l = *(const u8*)(p + 0) + (*l << 8);
#else 
    * l = *(const u32*)p;
#endif
    p += 4;
    return p;
}



/**
*@brief Print data buffer.
*@param iData Data to print.
*@param iSize  Length of data buffer.
*/
void AppPrintToHexString(const void* iData, u32 iSize);

/**
*@brief Print data buffer to a format text.
*@param iData Data to print.
*@param iSize  Length of data buffer.
*/
void AppPrintToHexText(const void* iData, u32 iSize);


/**
*@brief Convert a buffer to a 16 hexadecimal string.
*@note outLen >= inSize*2+1
*@param in The buffer to read from.
*@param inSize  The length of read buffer.
*@param out The cache to write in, cache length must >= (read buffer length * 2 + 1).
*@param outLen  The length of write cache.
*/
u32 AppConvertToHexString(const void* in, u32 inSize, s8* out, u32 outLen);
u32 AppConvertToHexString(const void* in, u32 inSize, wchar_t* out, u32 outLen);

/**
*@brief Convert a 16 hexadecimal string to a buffer.
*@note iSize >= iDataSize/2
*@param iData The buffer to read from.
*@param iDataSize  The length of read buffer.
*@param iResult The cache to write in, cache length must >= (read buffer length / 2).
*@param iSize  The length of write cache.
*/
u32 AppConvertToU8(const s8* iData, u32 iDataSize, u8* iResult, u32 iSize);

/**
*@brief Convert a hexadecimal char to 1 byte.
*@param ch The hexadecimal char.
*@return The converted byte.
*/
APP_FORCE_INLINE static u8 AppConvertToU8(s8 ch) {
    if(ch >= '0' && ch <= '9') {
        return (ch - '0');
    }
    if(ch >= 'A' && ch <= 'F') {
        return (ch - 'A' + 10);
    }
    if(ch >= 'a' && ch <= 'f') {
        return (ch - 'a' + 10);
    }
    return 0;
}


const s8* AppGoNextLine(const s8* iStart, const s8* const iEnd);

// combination of goNextWord followed by copyWord
const s8* AppGoAndCopyNextWord(s8* pOutBuf, const s8* iStart, u32 outBufLength, const s8* const iEnd, bool acrossNewlines = false);

// returns a pointer to the first printable character after the first non-printable
const s8* AppGoNextWord(const s8* iStart, const s8* const iEnd, bool acrossNewlines = true);

// returns a pointer to the first printable character available in the buffer
const s8* AppGoFirstWord(const s8* iStart, const s8* const iEnd, bool acrossNewlines = true);

// copies the current word from the inBuf to the outBuf
u32 AppCopyWord(s8* outBuf, const s8* iStart, u32 outBufLength, const s8* const iEnd);


const s8* AppGoBackFlag(const s8* iStart, const s8* iEnd, const s8 it);

/**
*@brief go to next mark ichar
*/
const s8* AppGoNextFlag(const s8* iBuf, const s8* const iEnd, const s8 ichar);

/**
*@brief skip the codes that marked with ichar & ichar2
*/
const s8* AppSkipFlag(const s8* iStart, const s8* const iEnd, const s8 leftFlag, const s8 rightFlag);


APP_INLINE static bool AppCharEquals(const s8 in, const s8 smallChar) {
    return ((in == smallChar) || (in == (smallChar - 32)));
}

static APP_FORCE_INLINE s32 AppToUpper(s32 ch) {
    return (ch >= 'a' && ch <= 'z') ? (ch - ('a' - 'A')) : ch;
}

static void AppToUpper(s8* iSrc, u64 size) {
    for(u64 i = 0; i < size; ++i) {
        if(*(iSrc) >= 'a' && *(iSrc) <= 'z') {
            *(iSrc) -= 32;
        }
        ++iSrc;
    }
}
static void AppToUpper(s8* iSrc) {
    while(*iSrc) {
        if(*iSrc >= 'a' && *iSrc <= 'z') {
            *iSrc -= 32;
        }
        ++iSrc;
    }
}

/* Fast tolower() alike function that does not care about locale
 * but just returns a-z insetad of A-Z. */
static APP_FORCE_INLINE s32 AppToLower(s32 ch) {
    return (ch >= 'A' && ch <= 'Z') ? (ch + ('a' - 'A')) : ch;
}


static void AppToLower(s8* iSrc) {
    while(*iSrc) {
        if(*iSrc >= 'A' && *iSrc <= 'Z') {
            *iSrc += 32;
        }
        ++iSrc;
    }
}


static void AppToLower(s8* iSrc, u64 size) {
    for(u64 i = 0; i < size; ++i) {
        if(*(iSrc) >= 'A' && *(iSrc) <= 'Z') {
            *(iSrc) += 32;
        }
        ++iSrc;
    }
}


/**
* @brief Copy maxlen chars from pSrc to pDest.
* @param pSrc The source string.
* @param pDest The dest string.
* @param maxLen The max length of copy.
* @param div The divide char for break.
* @return Number of copied.
*/
static s32 AppCopyCharN(const s8* pSrc, s8* pDest, s32 maxLen) {
    if(pSrc == 0 || pDest == 0) {
        return 0;
    }
    --maxLen;
    s32 indx = 0;
    while(*pSrc != 0 && indx < maxLen) {
        *pDest++ = *pSrc++;
        ++indx;
    }
    *pDest = 0;
    return indx;
}

//#define APP_USE_ICONV
#if defined(APP_USE_ICONV)
/**
 *@brief The tool class for engine.
 *@class IUtility
 */
class IUtility {
public:
    static IUtility& getInstance();

    /**
    * @brief Convert a default string to wstring.
    * @in String encoded with the default encoding of the operating system.
    * @inbytesleft length of the cache \p in.
    * @out The cache to store the result wchar_t string.
    * @outbytesleft length of the cache \p out.
    * @return wstring length if success, else <=0;
    */
    static s32 convert2Wchar(const s8* in, size_t inbytesleft, wchar_t* out, size_t outbytesleft);

    /**
    * @brief convert default str to utf8.
    * @in String encoded with the default encoding of the operating system.
    */
    static s32 convert2UTF8(const s8* in, size_t inbytesleft, s8* out, size_t outbytesleft);

    static s32 convertWchar2UTF8(const wchar_t* in, size_t inbytesleft, s8* out, size_t outbytesleft);
    static s32 convertUTF82Wchar(const s8* pChars, size_t inbytesleft, wchar_t* out, size_t outbytesleft);

    static s32 convertGBK2UTF8(const s8* in, size_t inbytesleft, s8* out, size_t outbytesleft);
    static s32 convertGBK2Wchar(const s8* in, size_t inbytesleft, wchar_t* out, size_t outbytesleft);

private:
    static void* m2UTF8;        //default to utf8
    static void* m2Wchar;       //default to wchar
    static void* mGBK2Wchar;    //gbk to utf16
    static void* mGBK2UTF8;     //gbk to utf8
    static void* mWchar2UTF8;   //wchar to utf8
    static void* mUTF8ToWchar;  //utf8 to wchar
    IUtility();
    ~IUtility();
    IUtility(const IUtility& it) = delete;
    IUtility& operator=(const IUtility& it) = delete;
};
#endif //APP_USE_ICONV


} //namespace core
} //namespace app
#endif	/* APP_IUTILITY_H */
