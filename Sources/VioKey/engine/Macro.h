//
//  Macro.h
//  VioKey - Vietnamese Input Engine
//
//  Based on OpenKey engine by Tuyen Mai (GPL)
//  https://github.com/tuyenvm/OpenKey
//

#ifndef Macro_h
#define Macro_h

#include <vector>
#include <map>
#include <string>
#include "DataType.h"

using namespace std;

struct MacroData {
    string macroText; //ex: "ms"
    string macroContent; //ex: "millisecond"
    vector<Uint32> macroContentCode; //converted of macroContent
};

/**
 * Call when you need to load macro data from disk
 */
void initMacroMap(const Byte* pData, const int& size);

/**
 * convert all macro data to save on disk
 */
void getMacroSaveData(vector<Byte>& outData);

/**
 * Use to find full text by macro
 */
bool findMacro(vector<Uint32>& key, vector<Uint32>& macroContentCode);

/**
 * check has this macro or not
 */
bool hasMacro(const string& macroName);

/**
 * Get all macro to show on macro table
 */
void getAllMacro(vector<vector<Uint32>>& keys, vector<string>& macroTexts, vector<string>& macroContents);

/**
 * add new macro to memory
 */
bool addMacro(const string& macroText, const string& macroContent);

/**
 * delete macro from memory
 */
bool deleteMacro(const string& macroText);

/**
 * Save all macro data to disk (UniKey-compatible format)
 */
void saveToFile(const string& path);

/**
 * Load macro data from disk
 */
void readFromFile(const string& path, const bool& append=true);

#endif /* Macro_h */
