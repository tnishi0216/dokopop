//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "StringLib.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)

// posから単語を切り出す
// posはstrからのオフセット
// start,endはstrからのオフセットを返す
// prevstartは、startの１つ前の単語、無い場合はstartと同じ値
// fLongestがTRUEの場合は、最高10語までgetする(.)を終わりとみなす
// about   : いい加減なところをクリックしても単語さえあれば必ずgetする
// alnum   : 英数字以外は無視

bool GetWord( const tchar *str, int pos, int &start, int &end, int &prevstart, bool fLongest, int wordcount, bool about, bool alnum )
{
	if ( !about )
		if ( !IsWordChar( *(str+pos) ) )
			return false;
//	while ( *(str+pos) == ' ' || *(str+pos) == '\t' ){
//		pos++;
//	}

	// 改行の場合は無し
	if ( str[pos] == '\r' || str[pos] == '\n' ){
		return false;
	}

	// posが文字列の終端、あるいは文字列の終端から１つ前の改行である場合はだめ //
	if ( !str[pos] || (!str[pos+1] && ((tuchar)str[pos] < ' ')) )
		return false; 

	const tchar *orgp = str;
	const tchar *p = orgp;
rescan:
	// 単語の先頭を探す //
	const tchar *wordtop = NULL;
	while ( *p ){
		if ( alnum ){
			// 英数字のみ
			if ( isalphanum(*p) ){
				// 単語の始まり
				wordtop = p;
				break;
			}
			p = NEXT_CHAR(p);
		} else {
			// 文字種を区別しない
			if ( IsWordChar( *p ) )
			{
				wordtop = p;
				break;
			}
			p++;
		}
	}
	if ( !wordtop )
		wordtop = p; 
	const tchar *wordtail = NULL;
	const tchar *wordprev = NULL;
	bool fSpc = false;
	// posにある単語のstartとendを求める
	while ( *p ){
		if ( !IsWordChar( *p ) || (alnum && !isalphanum(*p)) ){
			// 非単語文字
			if ( orgp + pos < p ){
				// 単語の終わり
				wordtail = p;
				break;
			} else {
				if ( alnum ){
					if ( _ismbblead(*p) ){
						// 日本語の場合は、単純な非単語文字とはみなさず、
						// 最初からscanをやり直す
						goto rescan;
					}
				}
			}
			fSpc = true;
		} else {
			// 単語文字
			if ( fSpc ){
				wordprev = wordtop;
				wordtop = p;
				fSpc = FALSE;
			}
		}
		if ( alnum ){
			p = NEXT_CHAR( p );
		} else {
			p++;
		}
	}

	// wordtopがposより後ろ
	if ( (unsigned)wordtop - (about ? 3 : 0) > (unsigned)(orgp + pos) ){
		// 単語の区切りだった、単語の最初がposより後ろにあった
		// about = trueの場合は、３文字分ほどいいかげん差を足しておく
		return false;
	}

	if ( fLongest ){
//		int wordcount = 10;	// 10語まで
		while ( *p && *p != '.' ){
			if ( !IsWordChar( *p ) || (alnum && !isalphanum(*p)) ){
				if ( !fSpc ){
					if ( --wordcount == 0 )
						break;
					fSpc = true;
				}
			} else {
				fSpc = FALSE;
			}
			if ( alnum ){
				p = NEXT_CHAR( p );
			} else {
				p++;
			}
		}
		wordtail = p;
	}

	if ( !wordtail )
		wordtail = p;
	start = STR_DIFF( wordtop, orgp );
	end = STR_DIFF( wordtail, orgp );
	if ( start == end )
		return false;
	if ( wordprev ){
		prevstart = STR_DIFF( wordprev, orgp );
	} else {
		prevstart = start;
	}
	return true;
}
#if 0
bool mbGetWord( const tchar *str, int pos, int &start, int &end, bool fLongest, int wordcount )
{
	ushort c;
	const tchar *sp = str + pos;
	LD_CHAR( c, sp );
	if ( !mbIsWordChar( c ) )
		return false;

	const tchar *orgp = str;
	const tchar *p = orgp;
	const tchar *wordtop = p;
	const tchar *wordtail = NULL;
	bool fSpc = false;
	while ( 1 ){
		sp = p;
		LD_CHAR( c, p );
		if ( (tuchar)c == 0x00 )
			break;
		if ( !mbIsWordChar( c ) ){
			if ( orgp + pos < sp ){
				wordtail = sp;
				break;
			}
			fSpc = true;
		} else {
			if ( fSpc ){
				wordtop = sp;
				fSpc = false;
			}
		}
	}
	p = sp;
	if ( wordtop > orgp + pos ){
		// 単語の区切りだった
		return false;
	}

	if ( fLongest ){
//		int wordcount = 10;	// 10語まで
		while ( 1 ){
			sp = p;
			LD_CHAR( c, p );
			if ( (tuchar)c == 0x00 )
				break;
			if ( c == '.' )
				break;
			if ( !mbIsWordChar( c ) ){
				if ( !fSpc ){
					if ( --wordcount == 0 )
						break;
					fSpc = true;
				}
			} else {
				fSpc = false;
			}
		}
		wordtail = sp;
	}

	if ( !wordtail )
		wordtail = sp;
	start = STR_DIFF( wordtop, orgp );
	end = STR_DIFF( wordtail, orgp );
	return true;
}
bool mbIsWordChar( unsigned short c )
{
	if ( c < 0x100 ){
		// 半角文字
		if ( isalphanum( c ) || c == '-' || c == '\'' ||
			 (c >= 0x86 && c <= 0xBF ) )
			return true;
		else
			return false;
	}
	// 全角文字
	if (
		// 英数字、カタカナ、ひらがな、ギリシャ、ロシア
		(c >= 0x824f && c <= 0x8491) ||
		// 漢字
		(c >= 0x889F) ||
		// アポストロフィ
		(c == CODE_APOSTROPHE)
		)
		return true;
	else
		return false;
}
#endif
//英数字の判別
int isalphanum( tuchar c )
{
	if ( c >= _t('A') && c <= _t('Z') ){
		return 1;
	}
	if ( c >= _t('a') && c <= _t('z') ){
		return 1;
	}
	if ( c >= _t('0') && c <= _t('9') ){
		return 1;
	}
	return 0;
}

