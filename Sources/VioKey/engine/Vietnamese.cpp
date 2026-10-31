//
//  Vietnamese.cpp
//  VioKey - Vietnamese Input Engine
//
//  Based on OpenKey engine by Tuyen Mai (GPL)
//  https://github.com/tuyenvm/OpenKey
//

#include "Vietnamese.h"
#include <cstring>
using namespace std;

map<Uint16, vector<vector<Uint16>>> _vowel = {
    {
        KEY_A, {
            {KEY_A, KEY_N, KEY_G}, {KEY_A, KEY_G | END_CONSONANT_MASK},
            {KEY_A, KEY_N},
            {KEY_A, KEY_M},
            {KEY_A, KEY_U},
            {KEY_A, KEY_Y},
            {KEY_A, KEY_T},
            {KEY_A, KEY_P},
            {KEY_A},
            {KEY_A, KEY_C},
        }
    }, {
        KEY_O, {
            {KEY_O, KEY_N, KEY_G}, {KEY_O, KEY_G | END_CONSONANT_MASK},
            {KEY_O, KEY_N},
            {KEY_O, KEY_M},
            {KEY_O, KEY_I},
            {KEY_O, KEY_C},
            {KEY_O, KEY_T},
            {KEY_O, KEY_P},
            {KEY_O}
        }
    }, {
        KEY_E, {
            {KEY_E, KEY_N, KEY_H}, {KEY_E, KEY_H | END_CONSONANT_MASK},
            {KEY_E, KEY_N, KEY_G}, {KEY_E, KEY_G | END_CONSONANT_MASK},
            {KEY_E, KEY_C, KEY_H}, {KEY_E, KEY_K | END_CONSONANT_MASK},
            {KEY_E, KEY_C},
            {KEY_E, KEY_T},
            {KEY_E, KEY_Y},
            {KEY_E, KEY_U},
            {KEY_E, KEY_P},
            {KEY_E, KEY_C},
            {KEY_E, KEY_N},
            {KEY_E, KEY_M},
            {KEY_E}
        }
    }, {
        KEY_W, {
            {KEY_O, KEY_N},

            {KEY_U, KEY_O, KEY_N, KEY_G}, {KEY_U, KEY_O, KEY_G | END_CONSONANT_MASK},

            {KEY_U, KEY_O, KEY_N},
            {KEY_U, KEY_O, KEY_I},
            {KEY_U, KEY_O, KEY_C},

            {KEY_O, KEY_I},
            {KEY_O, KEY_P},
            {KEY_O, KEY_M},
            {KEY_O, KEY_A},
            {KEY_O, KEY_T},

            {KEY_U, KEY_N, KEY_G}, {KEY_U, KEY_G | END_CONSONANT_MASK},
            {KEY_A, KEY_N, KEY_G}, {KEY_A, KEY_G | END_CONSONANT_MASK},
            {KEY_U, KEY_N},
            {KEY_U, KEY_M},
            {KEY_U, KEY_C},
            {KEY_U, KEY_A},
            {KEY_U, KEY_I},
            {KEY_U, KEY_T},
            {KEY_U},

            {KEY_A, KEY_P},
            {KEY_A, KEY_T},
            {KEY_A, KEY_M},

            {KEY_A, KEY_N},
            {KEY_A},
            {KEY_A, KEY_C},
            {KEY_A, KEY_C, KEY_H}, {KEY_A, KEY_K | END_CONSONANT_MASK},

            {KEY_O},
            {KEY_U, KEY_U}
        }
    }
};

map<Uint16, vector<vector<Uint32>>> _vowelCombine = {
    {
        KEY_A, {
            //fist elem can has end consonant or not
            {0, KEY_A, KEY_I},
            {0, KEY_A, KEY_O},
            {0, KEY_A, KEY_U},
            {0, KEY_A|TONE_MASK, KEY_U},
            {0, KEY_A, KEY_Y},
            {0, KEY_A|TONE_MASK, KEY_Y},
        }
    }, {
        KEY_E, {
            {0, KEY_E, KEY_O},
            {0, KEY_E|TONE_MASK, KEY_U},
        }
    }, {
        KEY_I, {
            {1, KEY_I, KEY_E|TONE_MASK, KEY_U},
            {0, KEY_I, KEY_A},
            {1, KEY_I, KEY_E|TONE_MASK},
            {0, KEY_I, KEY_U},

        }
    }, {
        KEY_O, {
            {0, KEY_O, KEY_A, KEY_I},
            {0, KEY_O, KEY_A, KEY_O},
            {0, KEY_O, KEY_A, KEY_Y},
            {0, KEY_O, KEY_E, KEY_O},
            {1, KEY_O, KEY_A},
            {1, KEY_O, KEY_A|TONEW_MASK},
            {1, KEY_O, KEY_E},
            {0, KEY_O, KEY_I},
            {0, KEY_O|TONE_MASK, KEY_I},
            {0, KEY_O|TONEW_MASK, KEY_I},
            {1, KEY_O, KEY_O},
            {1, KEY_O|TONE_MASK, KEY_O|TONE_MASK},
        }
    }, {
        KEY_U, {
            {0, KEY_U, KEY_Y, KEY_U},
            {1, KEY_U, KEY_Y, KEY_E|TONE_MASK},
            {0, KEY_U, KEY_Y, KEY_A},
            {0, KEY_U|TONEW_MASK, KEY_O|TONEW_MASK, KEY_U},
            {0, KEY_U|TONEW_MASK, KEY_O|TONEW_MASK, KEY_I},
            {0, KEY_U, KEY_O|TONE_MASK, KEY_I},
            {0, KEY_U, KEY_A|TONE_MASK, KEY_Y},
            {1, KEY_U, KEY_A, KEY_O},
            {1, KEY_U, KEY_A},
            {1, KEY_U, KEY_A|TONEW_MASK},
            {1, KEY_U, KEY_A|TONE_MASK},
            {0, KEY_U|TONEW_MASK, KEY_A},
            {1, KEY_U, KEY_E|TONE_MASK},
            {0, KEY_U, KEY_I},
            {0, KEY_U|TONEW_MASK, KEY_I},
            {1, KEY_U, KEY_O},
            {1, KEY_U, KEY_O|TONE_MASK},
            {0, KEY_U, KEY_O|TONEW_MASK},
            {1, KEY_U|TONEW_MASK, KEY_O|TONEW_MASK},
            {0, KEY_U|TONEW_MASK, KEY_U},
            {1, KEY_U, KEY_Y},
        }
    }, {
        KEY_Y, {
            {0, KEY_Y, KEY_E|TONE_MASK, KEY_U},
            {1, KEY_Y, KEY_E|TONE_MASK},
        }
    }
};

vector<vector<Uint16>> _consonantD = {
    {KEY_D, KEY_E, KEY_N, KEY_H}, {KEY_D, KEY_E, KEY_H | END_CONSONANT_MASK},
    {KEY_D, KEY_E, KEY_N, KEY_G}, {KEY_D, KEY_E, KEY_G | END_CONSONANT_MASK},
    {KEY_D, KEY_E, KEY_C, KEY_H}, {KEY_D, KEY_E, KEY_K | END_CONSONANT_MASK},
    {KEY_D, KEY_E, KEY_N},
    {KEY_D, KEY_E, KEY_C},
    {KEY_D, KEY_E, KEY_M},
    {KEY_D, KEY_E},
    {KEY_D, KEY_E, KEY_T},
    {KEY_D, KEY_E, KEY_U},
    {KEY_D, KEY_E, KEY_O},
    {KEY_D, KEY_E, KEY_P},

    {KEY_D, KEY_U, KEY_N, KEY_G}, {KEY_D, KEY_U, KEY_G | END_CONSONANT_MASK},
    {KEY_D, KEY_U, KEY_N},
    {KEY_D, KEY_U, KEY_M},
    {KEY_D, KEY_U, KEY_C},
    {KEY_D, KEY_U, KEY_O},
    {KEY_D, KEY_U, KEY_A},
    {KEY_D, KEY_U, KEY_O, KEY_I},
    {KEY_D, KEY_U, KEY_O, KEY_C},
    {KEY_D, KEY_U, KEY_O, KEY_N},
    {KEY_D, KEY_U, KEY_O, KEY_N, KEY_G}, {KEY_D, KEY_U, KEY_O, KEY_G | END_CONSONANT_MASK},
    {KEY_D, KEY_U},
    {KEY_D, KEY_U, KEY_P},
    {KEY_D, KEY_U, KEY_T},
    {KEY_D, KEY_U, KEY_I},

    {KEY_D, KEY_I, KEY_C, KEY_H}, {KEY_D, KEY_I, KEY_K | END_CONSONANT_MASK},
    {KEY_D, KEY_I, KEY_C},
    {KEY_D, KEY_I, KEY_N, KEY_H}, {KEY_D, KEY_I, KEY_H | END_CONSONANT_MASK},
    {KEY_D, KEY_I, KEY_N},
    {KEY_D, KEY_I},
    {KEY_D, KEY_I, KEY_A},
    {KEY_D, KEY_I, KEY_E},
    {KEY_D, KEY_I, KEY_E, KEY_C},
    {KEY_D, KEY_I, KEY_E, KEY_U},
    {KEY_D, KEY_I, KEY_E, KEY_N},
    {KEY_D, KEY_I, KEY_E, KEY_M},
    {KEY_D, KEY_I, KEY_E, KEY_P},
    {KEY_D, KEY_I, KEY_T},

    {KEY_D, KEY_O},
    {KEY_D, KEY_O, KEY_A},
    {KEY_D, KEY_O, KEY_A, KEY_N},
    {KEY_D, KEY_O, KEY_A, KEY_N, KEY_G}, {KEY_D, KEY_O, KEY_A, KEY_G | END_CONSONANT_MASK},
    {KEY_D, KEY_O, KEY_A, KEY_N, KEY_H}, {KEY_D, KEY_O, KEY_A, KEY_H | END_CONSONANT_MASK},
    {KEY_D, KEY_O, KEY_A, KEY_M},
    {KEY_D, KEY_O, KEY_E},
    {KEY_D, KEY_O, KEY_I},
    {KEY_D, KEY_O, KEY_P},
    {KEY_D, KEY_O, KEY_C},
    {KEY_D, KEY_O, KEY_N},
    {KEY_D, KEY_O, KEY_N, KEY_G}, {KEY_D, KEY_O, KEY_G | END_CONSONANT_MASK},
    {KEY_D, KEY_O, KEY_M},
    {KEY_D, KEY_O, KEY_T},

    {KEY_D, KEY_A},
    {KEY_D, KEY_A, KEY_T},
    {KEY_D, KEY_A, KEY_Y},
    {KEY_D, KEY_A, KEY_U},
    {KEY_D, KEY_A, KEY_I},
    {KEY_D, KEY_A, KEY_O},
    {KEY_D, KEY_A, KEY_P},
    {KEY_D, KEY_A, KEY_C},
    {KEY_D, KEY_A, KEY_C, KEY_H}, {KEY_D, KEY_A, KEY_K | END_CONSONANT_MASK},
    {KEY_D, KEY_A, KEY_N},
    {KEY_D, KEY_A, KEY_N, KEY_H}, {KEY_D, KEY_A, KEY_H | END_CONSONANT_MASK},
    {KEY_D, KEY_A, KEY_N, KEY_G}, {KEY_D, KEY_A, KEY_G | END_CONSONANT_MASK},
    {KEY_D, KEY_A, KEY_M},

    {KEY_D}
};

map<Uint16, vector<vector<Uint16>>> _vowelForMark = {
    {
        KEY_A, {
            {KEY_A, KEY_N, KEY_G}, {KEY_A, KEY_G | END_CONSONANT_MASK},
            {KEY_A, KEY_N},
            {KEY_A, KEY_N, KEY_H}, {KEY_A, KEY_H | END_CONSONANT_MASK},
            {KEY_A, KEY_M},
            {KEY_A, KEY_U},
            {KEY_A, KEY_Y},
            {KEY_A, KEY_T},
            {KEY_A, KEY_P},
            {KEY_A},
            {KEY_A, KEY_C},
            {KEY_A, KEY_I},
            {KEY_A, KEY_O},
            {KEY_A, KEY_C, KEY_H}, {KEY_A, KEY_K | END_CONSONANT_MASK},
        }
    }, {
        KEY_O, {
            {KEY_O, KEY_O, KEY_N, KEY_G}, {KEY_O, KEY_O, KEY_G | END_CONSONANT_MASK},
            {KEY_O, KEY_N, KEY_G}, {KEY_O, KEY_G | END_CONSONANT_MASK},
            {KEY_O, KEY_O, KEY_N},
            {KEY_O, KEY_O, KEY_C},
            {KEY_O, KEY_O},
            {KEY_O, KEY_N},
            {KEY_O, KEY_M},
            {KEY_O, KEY_I},
            {KEY_O, KEY_C},
            {KEY_O, KEY_T},
            {KEY_O, KEY_P},
            {KEY_O}
        }
    }, {
        KEY_E, {
            {KEY_E, KEY_N, KEY_H}, {KEY_E, KEY_H | END_CONSONANT_MASK},
            {KEY_E, KEY_N, KEY_G}, {KEY_E, KEY_G | END_CONSONANT_MASK},
            {KEY_E, KEY_C, KEY_H}, {KEY_E, KEY_K | END_CONSONANT_MASK},
            {KEY_E, KEY_C},
            {KEY_E, KEY_T},
            {KEY_E, KEY_Y},
            {KEY_E, KEY_U},
            {KEY_E, KEY_P},
            {KEY_E, KEY_C},
            {KEY_E, KEY_N},
            {KEY_E, KEY_M},
            {KEY_E}
        }
    }, {
        KEY_I, {
            {KEY_I, KEY_N, KEY_H}, {KEY_I, KEY_H | END_CONSONANT_MASK},
            {KEY_I, KEY_C, KEY_H}, {KEY_I, KEY_K | END_CONSONANT_MASK},
            {KEY_I, KEY_N},
            {KEY_I, KEY_T},
            {KEY_I, KEY_U},
            {KEY_I, KEY_U, KEY_P},
            {KEY_I, KEY_N},
            {KEY_I, KEY_M},
            {KEY_I, KEY_P},
            {KEY_I, KEY_A},
            {KEY_I, KEY_C},
            {KEY_I},
        }
    }, {
        KEY_U, {
            { KEY_U, KEY_N, KEY_G}, {KEY_U, KEY_G | END_CONSONANT_MASK},
            { KEY_U, KEY_I},
            { KEY_U, KEY_O},
            { KEY_U, KEY_Y},
            { KEY_U, KEY_Y, KEY_N},
            { KEY_U, KEY_Y, KEY_T},
            { KEY_U, KEY_Y, KEY_P},
            { KEY_U, KEY_Y, KEY_C, KEY_H}, {KEY_U, KEY_Y, KEY_K | END_CONSONANT_MASK},
            { KEY_U, KEY_Y, KEY_N, KEY_H}, {KEY_U, KEY_Y, KEY_H | END_CONSONANT_MASK},
            { KEY_U, KEY_T},
            { KEY_U, KEY_U},
            { KEY_U, KEY_A},
            { KEY_U, KEY_I},
            { KEY_U, KEY_C},
            { KEY_U, KEY_N},
            { KEY_U, KEY_M},
            { KEY_U, KEY_P},
            { KEY_U}
        }
    }, {
        KEY_Y, {
            {KEY_Y}
        }
    }
};

vector<vector<Uint16>> _consonantTable = {
    { KEY_N, KEY_G, KEY_H },
    { KEY_P, KEY_H },
    { KEY_T, KEY_H },
    { KEY_T, KEY_R },
    { KEY_G, KEY_I },
    { KEY_C, KEY_H },
    { KEY_N, KEY_H },
    { KEY_N, KEY_G},
    { KEY_K, KEY_H },
    { KEY_G, KEY_H },
    { KEY_G },
    { KEY_C },
    { KEY_Q },
    { KEY_K },
    { KEY_T },
    { KEY_R },
    { KEY_H },
    { KEY_B },
    { KEY_M },
    { KEY_V },
    { KEY_N },
    { KEY_L },
    { KEY_X },
    { KEY_P },
    { KEY_S },
    { KEY_D },
    { KEY_F | CONSONANT_ALLOW_MASK},
    { KEY_W | CONSONANT_ALLOW_MASK},
    { KEY_Z | CONSONANT_ALLOW_MASK},
    { KEY_J | CONSONANT_ALLOW_MASK},
    { KEY_F | END_CONSONANT_MASK},
    { KEY_W | END_CONSONANT_MASK},
    { KEY_J | END_CONSONANT_MASK},
};

vector<vector<Uint16>> _endConsonantTable = {
    { KEY_T }, { KEY_P }, { KEY_C }, { KEY_N }, { KEY_M },
    { KEY_G | END_CONSONANT_MASK }, { KEY_K | END_CONSONANT_MASK }, { KEY_H | END_CONSONANT_MASK },
    { KEY_C, KEY_H }, { KEY_N, KEY_H }, { KEY_N, KEY_G },
};

vector<Uint16> _standaloneWbad = {
    KEY_W, KEY_E, KEY_Y, KEY_F, KEY_J, KEY_K, KEY_Z
};

vector<vector<Uint16>> _doubleWAllowed = {
    {KEY_T, KEY_R},
    {KEY_T, KEY_H},
    {KEY_C, KEY_H},
    {KEY_N, KEY_H},
    {KEY_N, KEY_G},
    {KEY_K, KEY_H},
    {KEY_G, KEY_I},
    {KEY_P, KEY_H},
    {KEY_G, KEY_H},
};

map<Uint16, vector<Uint16>> _quickStartConsonant = {
    {KEY_F, {KEY_P, KEY_H}},
    {KEY_J, {KEY_G, KEY_I}},
    {KEY_W, {KEY_Q, KEY_U}},
};

map<Uint16, vector<Uint16>> _quickEndConsonant = {
    {KEY_G, {KEY_N, KEY_G}},
    {KEY_H, {KEY_N, KEY_H}},
    {KEY_K, {KEY_C, KEY_H}},
};

/*===========================================================================================*/
/*
 * Unicode precomposed table - the only supported charset
 * Row layout follows the same parity convention as before:
 *  - base rows: {CAPS, normal, CAPS_W, normal_W, then 5 tone pairs}
 *  - tone rows: {CAPS, normal, then 4 tone pairs}
 */
static const Uint16 _codesA[]            = {0x00C2, 0x00E2, 0x0102, 0x0103, 0x00C1, 0x00E1, 0x00C0, 0x00E0, 0x1EA2, 0x1EA3, 0x00C3, 0x00E3, 0x1EA0, 0x1EA1};
static const Uint16 _codesO[]            = {0x00D4, 0x00F4, 0x01A0, 0x01A1, 0x00D3, 0x00F3, 0x00D2, 0x00F2, 0x1ECE, 0x1ECF, 0x00D5, 0x00F5, 0x1ECC, 0x1ECD};
static const Uint16 _codesU[]            = {0x0000, 0x0000, 0x01AF, 0x01B0, 0x00DA, 0x00FA, 0x00D9, 0x00F9, 0x1EE6, 0x1EE7, 0x0168, 0x0169, 0x1EE4, 0x1EE5};
static const Uint16 _codesE[]            = {0x00CA, 0x00EA, 0x0000, 0x0000, 0x00C9, 0x00E9, 0x00C8, 0x00E8, 0x1EBA, 0x1EBB, 0x1EBC, 0x1EBD, 0x1EB8, 0x1EB9};
static const Uint16 _codesD[]            = {0x0110, 0x0111};
static const Uint16 _codesATone[]        = {0x1EA4, 0x1EA5, 0x1EA6, 0x1EA7, 0x1EA8, 0x1EA9, 0x1EAA, 0x1EAB, 0x1EAC, 0x1EAD};
static const Uint16 _codesAToneW[]       = {0x1EAE, 0x1EAF, 0x1EB0, 0x1EB1, 0x1EB2, 0x1EB3, 0x1EB4, 0x1EB5, 0x1EB6, 0x1EB7};
static const Uint16 _codesOTone[]        = {0x1ED0, 0x1ED1, 0x1ED2, 0x1ED3, 0x1ED4, 0x1ED5, 0x1ED6, 0x1ED7, 0x1ED8, 0x1ED9};
static const Uint16 _codesOToneW[]       = {0x1EDA, 0x1EDB, 0x1EDC, 0x1EDD, 0x1EDE, 0x1EDF, 0x1EE0, 0x1EE1, 0x1EE2, 0x1EE3};
static const Uint16 _codesUToneW[]       = {0x1EE8, 0x1EE9, 0x1EEA, 0x1EEB, 0x1EEC, 0x1EED, 0x1EEE, 0x1EEF, 0x1EF0, 0x1EF1};
static const Uint16 _codesETone[]        = {0x1EBE, 0x1EBF, 0x1EC0, 0x1EC1, 0x1EC2, 0x1EC3, 0x1EC4, 0x1EC5, 0x1EC6, 0x1EC7};
static const Uint16 _codesI[]            = {0x00CD, 0x00ED, 0x00CC, 0x00EC, 0x1EC8, 0x1EC9, 0x0128, 0x0129, 0x1ECA, 0x1ECB};
static const Uint16 _codesY[]            = {0x00DD, 0x00FD, 0x1EF2, 0x1EF3, 0x1EF6, 0x1EF7, 0x1EF8, 0x1EF9, 0x1EF4, 0x1EF5};

//lookup tables indexed by raw keycode (fits in a byte)
static const Uint16* _rowsBase[256];
static const Uint16* _rowsTone[256];
static const Uint16* _rowsToneW[256];
static bool _rowsInited = false;

static void initCodeRows() {
    memset(_rowsBase, 0, sizeof(_rowsBase));
    memset(_rowsTone, 0, sizeof(_rowsTone));
    memset(_rowsToneW, 0, sizeof(_rowsToneW));
    _rowsBase[KEY_A] = _codesA;
    _rowsBase[KEY_O] = _codesO;
    _rowsBase[KEY_U] = _codesU;
    _rowsBase[KEY_E] = _codesE;
    _rowsBase[KEY_D] = _codesD;
    _rowsBase[KEY_I] = _codesI;
    _rowsBase[KEY_Y] = _codesY;
    _rowsTone[KEY_A] = _codesATone;
    _rowsTone[KEY_O] = _codesOTone;
    _rowsTone[KEY_E] = _codesETone;
    _rowsToneW[KEY_A] = _codesAToneW;
    _rowsToneW[KEY_O] = _codesOToneW;
    _rowsToneW[KEY_U] = _codesUToneW;
    _rowsInited = true;
}

const Uint16* getCharRow(const Uint32& key) {
    if (!_rowsInited)
        initCodeRows();
    if (key & TONE_MASK)
        return _rowsTone[key & 0xFF];
    if (key & TONEW_MASK)
        return _rowsToneW[key & 0xFF];
    return _rowsBase[key & 0xFF];
}

//reverse lookup: character -> (row, position), used by macro case conversion
struct VCharLocation {
    const Uint16* row;
    int pos;
};
static map<Uint16, VCharLocation> _charLocation;

static void initCharLocation() {
    _charLocation.clear();
    const struct {
        const Uint16* row;
        int count;
    } rows[] = {
        {_codesA, 14}, {_codesO, 14}, {_codesU, 14}, {_codesE, 14}, {_codesD, 2},
        {_codesI, 10}, {_codesY, 10},
        {_codesATone, 10}, {_codesAToneW, 10}, {_codesOTone, 10}, {_codesOToneW, 10},
        {_codesUToneW, 10}, {_codesETone, 10},
    };
    for (int r = 0; r < sizeof(rows) / sizeof(rows[0]); r++) {
        for (int c = 0; c < rows[r].count; c++) {
            if (rows[r].row[c] != 0) {
                _charLocation[rows[r].row[c]] = {rows[r].row, c};
            }
        }
    }
}

const Uint16* findCharRow(const Uint16& ch, int& rowPos) {
    if (_charLocation.empty())
        initCharLocation();
    map<Uint16, VCharLocation>::iterator it = _charLocation.find(ch);
    if (it == _charLocation.end())
        return NULL;
    rowPos = it->second.pos;
    return it->second.row;
}

bool isValidVietChar(const Uint16& ch) {
    if (_charLocation.empty())
        initCharLocation();
    return _charLocation.find(ch) != _charLocation.end();
}

//for quick telex
map<Uint32, vector<Uint16>> _quickTelex = {
    {KEY_C, {KEY_C, KEY_H}},
    {KEY_G, {KEY_G, KEY_I}},
    {KEY_K, {KEY_K, KEY_H}},
    {KEY_N, {KEY_N, KEY_G}},
    {KEY_Q, {KEY_Q, KEY_U}},
    {KEY_P, {KEY_P, KEY_H}},
    {KEY_T, {KEY_T, KEY_H}},
    {KEY_U, {KEY_U, KEY_U}},
};

map<Uint32, Uint32> _characterMap = {
    {'a', KEY_A}, {'A', KEY_A|CAPS_MASK},
    {'b', KEY_B}, {'B', KEY_B|CAPS_MASK},
    {'c', KEY_C}, {'C', KEY_C|CAPS_MASK},
    {'d', KEY_D}, {'D', KEY_D|CAPS_MASK},
    {'e', KEY_E}, {'E', KEY_E|CAPS_MASK},
    {'f', KEY_F}, {'F', KEY_F|CAPS_MASK},
    {'g', KEY_G}, {'G', KEY_G|CAPS_MASK},
    {'h', KEY_H}, {'H', KEY_H|CAPS_MASK},
    {'i', KEY_I}, {'I', KEY_I|CAPS_MASK},
    {'j', KEY_J}, {'J', KEY_J|CAPS_MASK},
    {'k', KEY_K}, {'K', KEY_K|CAPS_MASK},
    {'l', KEY_L}, {'L', KEY_L|CAPS_MASK},
    {'m', KEY_M}, {'M', KEY_M|CAPS_MASK},
    {'n', KEY_N}, {'N', KEY_N|CAPS_MASK},
    {'o', KEY_O}, {'O', KEY_O|CAPS_MASK},
    {'p', KEY_P}, {'P', KEY_P|CAPS_MASK},
    {'q', KEY_Q}, {'Q', KEY_Q|CAPS_MASK},
    {'r', KEY_R}, {'R', KEY_R|CAPS_MASK},
    {'s', KEY_S}, {'S', KEY_S|CAPS_MASK},
    {'t', KEY_T}, {'T', KEY_T|CAPS_MASK},
    {'u', KEY_U}, {'U', KEY_U|CAPS_MASK},
    {'v', KEY_V}, {'V', KEY_V|CAPS_MASK},
    {'w', KEY_W}, {'W', KEY_W|CAPS_MASK},
    {'x', KEY_X}, {'X', KEY_X|CAPS_MASK},
    {'y', KEY_Y}, {'Y', KEY_Y|CAPS_MASK},
    {'z', KEY_Z}, {'Z', KEY_Z|CAPS_MASK},
    {'1', KEY_1}, {'!', KEY_1|CAPS_MASK},
    {'2', KEY_2}, {'@', KEY_2|CAPS_MASK},
    {'3', KEY_3}, {'#', KEY_3|CAPS_MASK},
    {'4', KEY_4}, {'$', KEY_4|CAPS_MASK},
    {'5', KEY_5}, {'%', KEY_5|CAPS_MASK},
    {'6', KEY_6}, {'^', KEY_6|CAPS_MASK},
    {'7', KEY_7}, {'&', KEY_7|CAPS_MASK},
    {'8', KEY_8}, {'*', KEY_8|CAPS_MASK},
    {'9', KEY_9}, {'(', KEY_9|CAPS_MASK},
    {'0', KEY_0}, {')', KEY_0|CAPS_MASK},
    {'`', KEY_BACKQUOTE}, {'~', KEY_BACKQUOTE|CAPS_MASK},
    {'-', KEY_MINUS}, {'_', KEY_MINUS|CAPS_MASK},
    {'=', KEY_EQUALS}, {'+', KEY_EQUALS|CAPS_MASK},
    {'[', KEY_LEFT_BRACKET}, {'{', KEY_LEFT_BRACKET|CAPS_MASK},
    {']', KEY_RIGHT_BRACKET}, {'}', KEY_RIGHT_BRACKET|CAPS_MASK},
    {'\\', KEY_BACK_SLASH}, {'|', KEY_BACK_SLASH|CAPS_MASK},
    {';', KEY_SEMICOLON}, {':', KEY_SEMICOLON|CAPS_MASK},
    {'\'', KEY_QUOTE}, {'"', KEY_QUOTE|CAPS_MASK},
    {',', KEY_COMMA}, {'<', KEY_COMMA|CAPS_MASK},
    {'.', KEY_DOT}, {'>', KEY_DOT|CAPS_MASK},
    {'/', KEY_SLASH}, {'?', KEY_SLASH|CAPS_MASK},
    {' ', KEY_SPACE}
};

map<Uint32, Uint32> _keyCodeToChar;

void initKeyCodeToChar() {
    _keyCodeToChar.clear();
    for (std::map<Uint32, Uint32>::iterator it = _characterMap.begin(); it != _characterMap.end(); ++it) {
        _keyCodeToChar[it->second] = it->first;
    }
}

Uint16 keyCodeToCharacter(const Uint32& keyCode) {
    if (_keyCodeToChar.size() == 0) { //init data if it is empty
        initKeyCodeToChar();
    }
    if (_keyCodeToChar.find(keyCode) != _keyCodeToChar.end()) {
        return _keyCodeToChar[keyCode];
    }
    return 0;
}
