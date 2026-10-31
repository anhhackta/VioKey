//
//  Vietnamese.h
//  VioKey - Vietnamese Input Engine
//
//  Based on OpenKey engine by Tuyen Mai (GPL)
//  https://github.com/tuyenvm/OpenKey
//

#ifndef Vietnamese_h
#define Vietnamese_h
#include "DataType.h"
#include <vector>
#include <map>

using namespace std;

extern map<Uint16, vector<vector<Uint16>>> _vowel;
extern map<Uint16, vector<vector<Uint32>>> _vowelCombine;
extern map<Uint16, vector<vector<Uint16>>> _vowelForMark;
extern vector<vector<Uint16>> _consonantD;
extern vector<vector<Uint16>> _consonantTable;
extern vector<vector<Uint16>> _endConsonantTable;
extern vector<Uint16> _standaloneWbad;
extern vector<vector<Uint16>> _doubleWAllowed;

extern map<Uint32, vector<Uint16>> _quickTelex;
extern map<Uint16, vector<Uint16>> _quickStartConsonant;
extern map<Uint16, vector<Uint16>> _quickEndConsonant;
extern map<Uint32, Uint32> _characterMap;

/**
 * Unicode precomposed is the only supported charset.
 * O(1) row lookup: key = keyCode possibly | TONE_MASK / TONEW_MASK
 * returns NULL if there is no row for this key
 */
const Uint16* getCharRow(const Uint32& key);

/**
 * reverse lookup: find which row and which position a Vietnamese character belongs to
 * (used for macro case conversion). returns NULL if not a known Vietnamese char
 */
const Uint16* findCharRow(const Uint16& ch, int& rowPos);

/**
 * check whether the character belongs to the Vietnamese precomposed set
 */
bool isValidVietChar(const Uint16& ch);

extern Uint16 keyCodeToCharacter(const Uint32& keyCode);
#endif /* Vietnamese_h */
