//
// Created by Nadrino on 28/08/2020.
//

#pragma once
#ifndef CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_IMPL_H
#define CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_IMPL_H

#include "../GenericToolbox.ProgressBar.h"

#include <utility>
#include <cmath>
#include <sys/stat.h>
#include <sstream>
#include <unistd.h>
#include <fstream>
#include <dirent.h>
#include <iostream>
#include <algorithm>
#include <cstring>
#include <thread>
#include <numeric>
#include <regex>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <iomanip>
#include <cstdio>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <vector>
#include "sys/times.h"

extern char* __progname;


// Displaying Tools
namespace GenericToolbox {

  static ProgressBar gProgressBar;

  template<typename T, typename TT> static inline std::string generateProgressBarStr( const T& iCurrent_, const TT& iTotal_, const std::string &title_ ){
    return gProgressBar.template generateProgressBarStr(iCurrent_, iTotal_, title_);
  }
  template<typename T, typename TT> static inline bool showProgressBar(const T& iCurrent_, const TT& iTotal_){
    return gProgressBar.template showProgressBar(iCurrent_, iTotal_);
  }
  template<typename T, typename TT> static inline std::string getProgressBarStr(const T& iCurrent_, const TT& iTotal_, const std::string &title_, bool forcePrint_ ){
    return gProgressBar.template getProgressBarStr(iCurrent_, iTotal_, title_, forcePrint_);
  }
  template<typename T, typename TT> static inline void displayProgressBar(const T& iCurrent_, const TT& iTotal_, const std::string &title_, bool forcePrint_) {
    return gProgressBar.template displayProgressBar(iCurrent_, iTotal_, title_, forcePrint_);
  }
  static inline void resetLastDisplayedValue(){
    gProgressBar.resetLastDisplayedValue();
  }

}

//! Conversion Tools
namespace GenericToolbox{

  template<typename T, typename TT> static inline std::string iterableToString(const T& iterable_, const TT& toStrFct_, bool jumpLine_, bool indentLine_){
    std::stringstream ss;
    for(const auto& element: iterable_){
      if( ss.str().empty() ){ ss << "{ "; }
      else{ ss << ", "; }
      if(jumpLine_){
        ss << std::endl;
        if( indentLine_ ) ss << "  ";
      }
      ss << toStrFct_(element);
    }
    if( ss.str().empty() ) return {"{}"};
    if(jumpLine_) ss << std::endl << "}";
    else ss << " }";
    return ss.str();
  }

}

//! Printout Tools
namespace GenericToolbox{

  static inline void waitProgressBar(unsigned int nbMilliSecToWait_, const std::string &progressTitle_) {

    auto anchorTimePoint = std::chrono::high_resolution_clock::now();
    std::chrono::microseconds totalDurationToWait(nbMilliSecToWait_*1000);
    std::chrono::microseconds cumulatedDuration(0);
    std::chrono::microseconds loopUpdateMaxFrequency(nbMilliSecToWait_); // 1000x faster than the whole time

    GenericToolbox::displayProgressBar( 0, totalDurationToWait.count(), progressTitle_);
    while( true ){
      std::this_thread::sleep_for( loopUpdateMaxFrequency );
      cumulatedDuration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - anchorTimePoint);
      if( cumulatedDuration >= totalDurationToWait ){
        return;
      }
      else{
        GenericToolbox::displayProgressBar( cumulatedDuration.count(), totalDurationToWait.count(), progressTitle_);
      }
    }
    GenericToolbox::displayProgressBar( totalDurationToWait.count(), totalDurationToWait.count(), progressTitle_);

  }
  static inline std::string parseIntAsString(int intToFormat_){
    if(intToFormat_ / 1000 < 10){
        return std::to_string(intToFormat_);
    }
    intToFormat_/=1000.; // in K
    if(intToFormat_ / 1000 < 10){
      return std::to_string(intToFormat_) + "K";
    }
    intToFormat_/=1000.; // in M
    if(intToFormat_ / 1000 < 10){
      return std::to_string(intToFormat_) + "M";
    }
    intToFormat_/=1000.; // in G
    if(intToFormat_ / 1000 < 10){
      return std::to_string(intToFormat_) + "G";
    }
    intToFormat_/=1000.; // in T
    if(intToFormat_ / 1000 < 10){
      return std::to_string(intToFormat_) + "T";
    }
    intToFormat_/=1000.; // in P
    return std::to_string(intToFormat_) + "P";
  }
  static inline std::string highlightIf(bool condition_, const std::string& text_){
    std::stringstream ss;
    ss << (condition_ ? ColorCodes::redBackground : "" );
    ss << text_;
    ss << ( condition_ ? ColorCodes::resetColor : "" );
    return ss.str();
  }
  static inline std::string makeRainbowString(const std::string& inputStr_, bool stripUnicode_){
    std::string outputString;
    std::string inputStrStripped;
    stripUnicode_ ? inputStrStripped = GenericToolbox::stripStringUnicode(inputStr_) : inputStrStripped = inputStr_;
    double nbCharsPerColor = double(inputStrStripped.size()) / double(ColorCodes::rainbowColorList.size());
    int colorSlot{0};
    for( int iChar = 0 ; iChar < inputStrStripped.size() ; iChar++ ){
      if( nbCharsPerColor < 1 or iChar == 0 or ( (iChar+1) / nbCharsPerColor) - colorSlot + 1 > 1 ){
        outputString += ColorCodes::rainbowColorList[colorSlot++];
      }
      outputString += inputStrStripped[iChar];
    }
    outputString += ColorCodes::resetColor;
    return outputString;
  }

}


//! Vector management
namespace GenericToolbox {

  // Content management
  template <typename T> static inline bool doesElementIsInVector(const T& element_, const std::vector<T>& vector_){
    return std::find(vector_.begin(), vector_.end(), element_) != vector_.end();
  }
  static inline bool doesElementIsInVector(const char* element_, const std::vector<std::string>& vector_){
    return std::find(vector_.begin(), vector_.end(), element_) != vector_.end();
  }
  template <typename T> static inline int findElementIndex(const T& element_, const std::vector<T>& vector_ ){ // test
    int outIndex = -1;
    auto it = std::find(vector_.begin(), vector_.end(), element_);
    if (it != vector_.end()){ outIndex = std::distance(vector_.begin(), it); }
    return outIndex;
  }
  static inline int findElementIndex(const char* element_, const std::vector<std::string>& vector_ ){
    int outIndex = -1;
    auto it = std::find(vector_.begin(), vector_.end(), element_);
    if (it != vector_.end()){ outIndex = int(std::distance(vector_.begin(), it)); }
    return outIndex;
  }
  template<typename T> static inline void insertInVector(std::vector<T> &vector_, const std::vector<T> &vectorToInsert_, size_t insertBeforeThisIndex_){
    if( insertBeforeThisIndex_ > vector_.size() ){
      throw std::runtime_error("GenericToolBox::insertInVector error: insertBeforeThisIndex_ >= vector_.size()");
    }
    if( vectorToInsert_.empty() ){
      return;
    }
    if( vector_.empty() ){
      vector_ = vectorToInsert_;
      return;
    }

    int nElementToInsert = int(vectorToInsert_.size());
    for( int iElementToInsert = nElementToInsert-1 ; iElementToInsert >= 0 ; iElementToInsert-- ){
      vector_.insert(vector_.begin() + insertBeforeThisIndex_, vectorToInsert_[iElementToInsert]);
    }
  }
  template<typename T> static inline void insertInVector(std::vector<T> &vector_, const T &elementToInsert_, size_t insertBeforeThisIndex_){
    std::vector<T> vectorToInsert(1, elementToInsert_);
    insertInVector(vector_, vectorToInsert, insertBeforeThisIndex_);
  }

  // Generators
  template<typename T> static inline std::vector<size_t> indices(const std::vector<T> &vector_){
    std::vector<size_t> output(vector_.size(), 0);
    for( size_t iIndex = 0 ; iIndex < output.size() ; iIndex++ ){
      output.at(iIndex) = iIndex;
    }
    return output;
  }
  template <typename T> static inline std::vector<T> getSubVector( const std::vector<T>& vector_, size_t beginIndex_, int endIndex_ ){
    if( endIndex_ < 0 ){ endIndex_ += vector_.size(); }
    if( beginIndex_ >= endIndex_ ){ return std::vector<T> (); }
    return std::vector<T> ( &vector_[beginIndex_] , &vector_[endIndex_+1] );
  }
  template <typename T, typename TT> static inline std::vector<TT> convertVectorType( const std::vector<T>& vector_, std::function<TT(T)>& convertTypeFunction_ ){
    std::vector<TT> outVec;
    for(const auto& element : vector_){
      outVec.emplace_back(convertTypeFunction_(element));
    }
    return outVec;
  }

  // Printout / to string conversions
  template <typename T> static inline void printVector(const std::vector<T>& vector_, bool jumpLine_, bool indentLine_){
    std::cout << parseVectorAsString(vector_, jumpLine_, indentLine_) << std::endl;
  }
  template <typename T> static inline std::string parseVectorAsString(const std::vector<T>& vector_, bool jumpLine_, bool indentLine_){
    return GenericToolbox::iterableToString(vector_, [&](const T& elm_){ return elm_; }, jumpLine_, indentLine_);
  }
  static inline std::string parseVectorAsString(const std::vector<std::string> &vector_, bool jumpLine_, bool indentLine_){
    return GenericToolbox::iterableToString(vector_, [&](const std::string& elm_){ return std::string{"\""+elm_+"\""}; }, jumpLine_, indentLine_);
  }

  // Stats
  template <typename T> static inline double getAverage(const std::vector<T>& vector_, const std::function<double(const T&)>& evalElementFct_){
    double outVal = 0;
    for( auto& element : vector_ ){ outVal += static_cast<double>(evalElementFct_(element)); }
    return outVal / vector_.size();
  }
  template<typename T> static inline double getAveragedSlope(const std::vector<T> &yValues_){
    auto xValues = yValues_;
    for( size_t iVal = 0 ; iVal < yValues_.size() ; iVal++ ){
      xValues.at(iVal) = iVal;
    }
    return getAveragedSlope(yValues_, xValues);
  }
  template<typename T, typename TT> static inline double getAveragedSlope(const std::vector<T> &yValues_, const std::vector<TT> &xValues_){
    if(xValues_.size() != yValues_.size()){
      throw std::logic_error("x and y values list do have the same size.");
    }
    const auto n    = xValues_.size();
    const auto s_x  = std::accumulate(xValues_.begin(), xValues_.end(), 0.0);
    const auto s_y  = std::accumulate(yValues_.begin(), yValues_.end(), 0.0);
    const auto s_xx = std::inner_product(xValues_.begin(), xValues_.end(), xValues_.begin(), 0.0);
    const auto s_xy = std::inner_product(xValues_.begin(), xValues_.end(), yValues_.begin(), 0.0);
    const auto a    = (n * s_xy - s_x * s_y) / (n * s_xx - s_x * s_x);
    return a;
  }

  // Sorting
  template <typename T> static inline std::vector<size_t> getSortPermutation(const std::vector<T>& vectorToSort_, std::function<bool(const T&, const T&)> firstArgGoesFirstFct_ ){
    std::vector<size_t> p(vectorToSort_.size());
    std::iota(p.begin(), p.end(), 0);
    std::sort(p.begin(), p.end(),
              [&](size_t i, size_t j){ return firstArgGoesFirstFct_(vectorToSort_.at(i), vectorToSort_.at(j)); });
    return p;
  }
  template <typename T> static inline std::vector<size_t> getSortPermutation(const std::vector<T>& vectorToSort_, std::function<bool(const T, const T)> firstArgGoesFirstFct_ ){
    std::vector<size_t> p(vectorToSort_.size());
    std::iota(p.begin(), p.end(), 0);
    std::sort(p.begin(), p.end(),
              [&](size_t i, size_t j){ return firstArgGoesFirstFct_(vectorToSort_[i], vectorToSort_[j]); });
    return p;
  }
  template <typename T> static inline std::vector<T> applyPermutation(const std::vector<T>& vectorToPermute_, const std::vector<std::size_t>& sortPermutation_ ){
    std::vector<T> sorted_vec(vectorToPermute_.size());
    std::transform(sortPermutation_.begin(), sortPermutation_.end(), sorted_vec.begin(),
                   [&](std::size_t i){ return vectorToPermute_[i]; });
    return sorted_vec;
  }

  // Others
  template<typename T, typename TT> static inline T& getListEntry(std::list<T>& list_, TT index_){
    typename std::list<T>::iterator it = list_.begin();
    std::advance(it, index_);
    return *it;
  }
  template<typename T, typename TT> static inline const T& getListEntry(const std::list<T>& list_, TT index_){
    typename std::list<T>::const_iterator it = list_.begin();
    std::advance(it, index_);
    return *it;
  }

}


//! Map management
namespace GenericToolbox {

  template <typename K, typename  T> static inline bool doesKeyIsInMap( const K& key_, const std::map<K,T>& map_ ){
    return ( map_.find(key_) != map_.end() );
  }
  template <typename K, typename T> static inline T* getElementPtrIsInMap( const K& key_, std::map<K,T>& map_ ){
    auto it = map_.find(key_);
    if( it == map_.end() ){
      return nullptr;
    }
    return &( it->second );
  }
  template <typename T1, typename T2> static inline void appendToMap(std::map<T1, T2> &mapContainer_, const std::map<T1, T2> &mapToPushBack_, bool overwrite_) {
    for(const auto& newEntry : mapToPushBack_){
      if(not overwrite_ and doesKeyIsInMap(newEntry.first, mapContainer_)){
        continue;
      }
      mapContainer_[newEntry.first] = newEntry.second;
    }
  }
  template <typename T> static inline std::map<std::string, T> getSubMap(const std::map<std::string, T>& map_, const std::string &keyStrStartWith_ ){
    std::map<std::string, T> outSubMap;
    for(const auto& mapPair : map_){
      if(GenericToolbox::doesStringStartsWithSubstring(mapPair.first, keyStrStartWith_)){
        outSubMap[mapPair.first] = mapPair.second;
      }
    }
    return outSubMap;
  }
  template <typename T1, typename T2> static inline std::string parseMapAsString(const std::map<T1, T2>& map_, bool enableLineJump_){
    return GenericToolbox::iterableToString(
        map_,
        [&](const std::pair<T1, T2>& elm_){
          std::stringstream ss;
          ss << "{ " << elm_.first << ": " << elm_.second << " }";
          return ss.str();
        },
        enableLineJump_, enableLineJump_);
  }
  template <typename T1, typename T2> static inline void printMap(const std::map<T1, T2>& map_, bool enableLineJump_){
    std::cout << parseMapAsString(map_, enableLineJump_) << std::endl;
  }

}


// String Management Tools
namespace GenericToolbox {

#if defined(__GNUC__) && !defined(__clang__) && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 9))
#else
  namespace StringManagementUtils{
    static std::regex ansiRegex("\033((\\[((\\d+;)*\\d+)?[A-DHJKMRcfghilmnprsu])|\\(|\\))");
  }
#endif

  static inline std::string addUpDownBars(const std::string& str_, bool stripUnicode_){
    std::stringstream ss;
    size_t strLength = str_.size();
    if( stripUnicode_ ) strLength = GenericToolbox::stripStringUnicode(str_).size();
    std::string bar = GenericToolbox::repeatString("─", int(strLength));
    ss << bar << std::endl << str_ << std::endl << bar;
    return ss.str();
  }

  static inline bool doesStringContainsSubstring(const std::string& str_, const std::string& subStr_, bool ignoreCase_) {
    if (subStr_.empty()) return true;
    if (subStr_.size() > str_.size()) return false;
    if (ignoreCase_) { return doesStringContainsSubstring(toLowerCase(str_), toLowerCase(subStr_)); }
    if (str_.find(subStr_) != std::string::npos) return true;
    else return false;
  }
  static inline bool doesStringStartsWithSubstring(const std::string& str_, const std::string& subStr_, bool ignoreCase_) {
    if (subStr_.empty()) return true;
    if (subStr_.size() > str_.size()) return false;
    if (ignoreCase_) { return doesStringStartsWithSubstring(toLowerCase(str_), toLowerCase(subStr_)); }
    return (not str_.compare(0, subStr_.size(), subStr_));
  }
  static inline bool doesStringEndsWithSubstring(const std::string& str_, const std::string& subStr_, bool ignoreCase_) {
    if (subStr_.empty()) return true;
    if (subStr_.size() > str_.size()) return false;
    if (ignoreCase_) { return doesStringEndsWithSubstring(toLowerCase(str_), toLowerCase(subStr_)); }
    return (not str_.compare(str_.size() - subStr_.size(), subStr_.size(), subStr_));
  }
  static inline std::string toLowerCase(const std::string &inputStr_) {
    std::string output_str(inputStr_);
    std::transform(output_str.begin(), output_str.end(), output_str.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return output_str;
  }
  static inline std::string stripStringUnicode(const std::string &inputStr_){
    std::string outputStr(inputStr_);

    if(GenericToolbox::doesStringContainsSubstring(outputStr, "\033")){
      // remove color
      std::string tempStr;
      auto splitOutputStr = GenericToolbox::splitString(outputStr, "\033");
      for(const auto& sliceStr : splitOutputStr){
        if(sliceStr.empty()) continue;
        if(tempStr.empty()){
          tempStr = sliceStr;
          continue;
        }
        // look for a 'm' char that determines the end of the color code
        bool mCharHasBeenFound = false;
        for(const char& c : sliceStr){
          if(not mCharHasBeenFound){
            if(c == 'm'){
              mCharHasBeenFound = true;
            }
          }
          else{
            tempStr += c;
          }
        }
      }
      outputStr = tempStr;
    }

    outputStr.erase(
      remove_if(
        outputStr.begin(), outputStr.end(),
        [](const char& c){return !isprint( static_cast<unsigned char>( c ) );}
      ),
      outputStr.end()
    );

    return outputStr;
  }
  static inline std::string stripBracket(const std::string &inputStr_, char bra_, char ket_, bool allowUnclosed_, std::vector<std::string>* argBuffer_){
    int iChar{0}; std::string out;
    while( iChar < inputStr_.size() ){
      if ( inputStr_[iChar] == bra_ ){
        iChar++;
        if(argBuffer_!= nullptr){ argBuffer_->emplace_back(); }
        while(iChar < inputStr_.size()){
          if(inputStr_[iChar] == ket_ ) { iChar++; break; }
          if(argBuffer_!= nullptr){ argBuffer_->back() += inputStr_[iChar]; }
          iChar++;
          if(iChar == inputStr_.size()){
            if( not allowUnclosed_ ){
              throw std::runtime_error("unclosed bracket.");
            }
          }
        }
      }
      if(iChar < inputStr_.size()) out += inputStr_[iChar++];
    }
    return out;
  }
  static inline size_t getPrintSize(const std::string& str_){
    if( str_.empty() ) return 0;
#if defined(__GNUC__) && !defined(__clang__) && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 9))
// this is gcc 4.8 or earlier
// std::regex support is buggy, so don't use in this block
  return str_.size();
#else
// this is gcc 4.9 or later, or other compilers like clang
// hopefully std::regex support is ok here
    std::string::iterator::difference_type result = 0;
    std::for_each(
      std::sregex_token_iterator(str_.begin(), str_.end(), StringManagementUtils::ansiRegex, -1),
      std::sregex_token_iterator(),
      [&result](std::sregex_token_iterator::value_type const& e) {
        std::string tmp(e);
        result += std::count_if(tmp.begin(), tmp.end(), ::isprint);
      }
    );
    return result;
#endif
  }
  static inline std::string repeatString(const std::string &inputStr_, int amount_){
    std::string outputStr;
    if(amount_ <= 0) return outputStr;
    for(int i_count = 0 ; i_count < amount_ ; i_count++){
      outputStr += inputStr_;
    }
    return outputStr;
  }
  static inline std::string trimString(const std::string &inputStr_, const std::string &strToTrim_){
      std::string outputStr(inputStr_);
      while(GenericToolbox::doesStringStartsWithSubstring(outputStr, strToTrim_)){
          outputStr = outputStr.substr(strToTrim_.size(), outputStr.size());
      }
      while(GenericToolbox::doesStringEndsWithSubstring(outputStr, strToTrim_)){
          outputStr = outputStr.substr(0, outputStr.size() - strToTrim_.size());
      }
      return outputStr;
  }
  static inline std::string padString(const std::string& inputStr_, unsigned int padSize_, const char& padChar){
    std::string outputString;
//    int padDelta = int(inputStr_.size()) - int(padSize_);
    int padDelta = int(GenericToolbox::getPrintSize(inputStr_)) - int(padSize_);
    while( padDelta < 0 ){
      // add extra chars if needed
      outputString += padChar;
      padDelta++;
    }
    outputString += inputStr_;
    return outputString.substr(0, outputString.size() - padDelta);
  }
  static inline std::string indentString(const std::string& inputStr_, unsigned int indentCount_, const std::string& indentChar){
    std::string outStr = inputStr_;
    GenericToolbox::indentInputString(outStr, indentCount_, indentChar);
    return outStr;
  }
  static inline std::string removeRepeatedCharacters(const std::string &inputStr_, const std::string &repeatedChar_) {
    std::string outStr = inputStr_;
    GenericToolbox::removeRepeatedCharInsideInputStr(outStr, repeatedChar_);
    return outStr;
  }
  std::string joinVectorString(const std::vector<std::string> &string_list_, const std::string &delimiter_, int begin_index_, int end_index_) {

    std::string joined_string;
    if (end_index_ == 0) end_index_ = int(string_list_.size());

    // circular permutation -> python style : tab[-1] = tab[tab.size - 1]
    if (end_index_ < 0 and int(string_list_.size()) > std::abs(end_index_))
      end_index_ = int(string_list_.size()) + end_index_;

    for (int i_list = begin_index_; i_list < end_index_; i_list++) {
      if (not joined_string.empty()) joined_string += delimiter_;
      joined_string += string_list_[i_list];
    }

    return joined_string;
  }
  std::string replaceSubstringInString(const std::string &input_str_, const std::string &substr_to_look_for_, const std::string &substr_to_replace_) {
    std::string stripped_str = input_str_;
    GenericToolbox::replaceSubstringInsideInputString(stripped_str, substr_to_look_for_, substr_to_replace_);
    return stripped_str;
  }
  std::string parseUnitPrefix(double val_, int maxPadSize_){
    std::stringstream ss;

    if( val_ < 0 ){
      ss << "-";
      val_ = -val_;
      maxPadSize_--;
    }

    if(maxPadSize_ > -1){
      ss << std::setprecision(maxPadSize_-1);
    }

    auto reducedVal = size_t(fabs(val_));
    if( reducedVal > 0 ){
      if     ( (reducedVal = (reducedVal / 1000)) == 0 ){
        ss << val_;
      }
      else if( (reducedVal = (reducedVal / 1000)) == 0 ){
        ss << val_/1E3 << "K";
      }
      else if( (reducedVal = (reducedVal / 1000)) == 0 ){
        ss << val_/1E6 << "M";
      }
      else if( (reducedVal = (reducedVal / 1000)) == 0 ){
        ss << val_/1E9 << "G";
      }
      else if( (reducedVal = (reducedVal / 1000)) == 0 ){
        ss << val_/1E12 << "T";
      }
      else {
        ss << val_/1E15 << "P";
      }
    } // K, M, G, T, P
    else{
      if( val_ < 1E-3 ){ // force scientific notation
        ss << std::scientific << val_;
      }
      else{
        ss << val_;
      }
    }


    return ss.str();
  }
  std::string parseSizeUnits(double sizeInBytes_){
    return parseUnitPrefix(sizeInBytes_) + "B";
  }
  static inline std::vector<std::string> splitString(const std::string &inputString_, const std::string &delimiter_, bool removeEmpty_) {

    std::vector<std::string> outputSliceList;

    const char *src = inputString_.c_str();
    const char *next = src;

    std::string out_string_piece;

    while ((next = std::strstr(src, delimiter_.c_str())) != nullptr) {
      out_string_piece = "";
      while (src != next) {
        out_string_piece += *src++;
      }
      outputSliceList.emplace_back(out_string_piece);
      /* Skip the delimiter_ */
      src += delimiter_.size();
    }

    /* Handle the last token */
    out_string_piece = "";
    while (*src != '\0')
      out_string_piece += *src++;

    outputSliceList.emplace_back(out_string_piece);

    if(not removeEmpty_){
      return outputSliceList;
    }
    else{
      std::vector<std::string> strippedOutput;
      for(const auto& slice : outputSliceList){
        if(not slice.empty()){
          strippedOutput.emplace_back(slice);
        }
      }
      return strippedOutput;
    }


  }
  static inline std::string formatString( const std::string& strToFormat_ ){
    return strToFormat_;
  }
  template<typename ... Args> std::string formatString(const std::string& strToFormat_, Args ... args) {
    size_t size = snprintf(nullptr, 0, strToFormat_.c_str(), args ...) + 1; // Extra space for '\0'
    if (size <= 0) { throw std::runtime_error("Error during formatting."); }
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, strToFormat_.c_str(), args ...);
    return {buf.get(), buf.get() + size - 1}; // We don't want the '\0' inside
  }

  static inline void replaceSubstringInsideInputString(std::string &input_str_, const std::string &substr_to_look_for_, const std::string &substr_to_replace_){
    size_t index = 0;
    while ((index = input_str_.find(substr_to_look_for_, index)) != std::string::npos) {
      input_str_.replace(index, substr_to_look_for_.length(), substr_to_replace_);
      index += substr_to_replace_.length();
    }
  }
  static inline void removeRepeatedCharInsideInputStr(std::string &inputStr_, const std::string &doubledChar_){
    std::string doubledCharStr = doubledChar_+doubledChar_;
    std::string lastStr;
    do{
      lastStr = inputStr_;
      GenericToolbox::replaceSubstringInsideInputString(inputStr_, doubledCharStr, doubledChar_);
    } while( lastStr != inputStr_ );
  }
  static inline void indentInputString(std::string& inputStr_, unsigned int indentCount_, const std::string& indentChar){
    int originalSize = int(inputStr_.size());
    for( int iChar = originalSize-1 ; iChar >= 0 ; iChar-- ){
      if( iChar == 0 or inputStr_[iChar] == '\n'){
        int offSet = 1;
        if( iChar == 0 ) offSet = 0;
        for( unsigned int iIndent = 0 ; iIndent < indentCount_ ; iIndent++ ){
          inputStr_.insert(iChar+offSet, indentChar);
        }
      }
    }
  }

}


// Conversion Tools
namespace GenericToolbox {

  static inline std::string toHex(const void* address_, size_t nBytes_){
    std::stringstream ss(std::string(2*nBytes_, 0));
    unsigned char* address{(unsigned char*)(address_) + nBytes_-1};
    do{ ss << std::hex << std::setfill('0') << std::setw(2) << static_cast<unsigned>(*(address--)); } while(address >= address_);
    return ss.str();
  }
  template<typename T> static inline std::string toHex(const T& val_){
    return toHex(&val_, sizeof(val_));
  }
  template<typename T> static inline std::string toHexString(T integerVal_, size_t nbDigit_){
    std::stringstream stream;
    stream << "0x" << toHex(&integerVal_, sizeof(integerVal_));
    if( nbDigit_ == 0 ) return stream.str();
    else return "0x" + stream.str().substr(2 + sizeof(T)*2 - nbDigit_, nbDigit_);
  }
  template<typename T> static inline std::string stackToHex(const std::vector<T> &rawData_, size_t stackSize_) {
    std::stringstream ss;
    size_t nChunks = rawData_.size()*sizeof(T)/stackSize_;
    const unsigned char* address{&rawData_[0]};
    for( int iChunk=0 ; iChunk < nChunks ; iChunk++ ){
      ss.str().empty()? ss << "{ ": ss << ", ";
      ss << "0x" << GenericToolbox::toHex(address, stackSize_);
      address += stackSize_;
    }

    if( address < &(rawData_.back())+sizeof(T) ) {
      ss.str().empty()? ss << "{ ": ss << ", ";
      ss << "0x" << GenericToolbox::repeatString("_-", address+stackSize_ - (&(rawData_.back())+sizeof(T)));
      ss << GenericToolbox::toHex(address, (&(rawData_.back()) + sizeof(T)) - address);
    }

    ss << " }";
    return ss.str();
  }
  static inline bool toBool(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    std::istringstream is(str);
    bool b;
    is >> std::boolalpha >> b;
    return b;
  }

}


// OS Tools
namespace GenericToolbox{

  namespace Internals {
    static inline char * getEnvironmentVariable(char const envVarName_[]){
#if defined _WIN32 // getenv() is deprecated on Windows
      char *buf{nullptr};
    size_t sz;
    std::string val;
    if (_dupenv_s(&buf, &sz, envVarName_) || buf == nullptr) return val;
    val = buf;
    free(buf);
    return val;
#else
      return getenv(envVarName_);
#endif
    }
    static inline bool expandEnvironmentVariables(const char *inputFilePath_, char *extendedFilePath_) {
      int ier, iter, lx, ncopy;
      char *inp, *out, *x, *t, *charBuffer;
      const char *b, *c, *e;
      const char *expandedPathCharArray;
      int bufferSize_ = 8192; // max path length
      charBuffer = new char[bufferSize_ * 4];

      iter = 0;
      extendedFilePath_[0] = 0;
      inp = charBuffer + bufferSize_;
      out = inp + bufferSize_;
      inp[-1] = ' ';
      inp[0] = 0;
      out[-1] = ' ';
      c = inputFilePath_ + strspn(inputFilePath_, " \t\f\r");
      //VP  if (isalnum(c[0])) { strcpy(inp, WorkingDirectory()); strcat(inp, "/"); } // add $cwd

//      strlcat(inp, c, bufferSize_);
      strncat(inp, c, bufferSize_);

      again:
      iter++; c = inp; ier = 0;
      x = out; x[0] = 0;

      expandedPathCharArray = nullptr;
      e = nullptr;
      if (c[0] == '~' && c[1] == '/') { // ~/ case
        std::string homeDirStr = getHomeDirectory();
        expandedPathCharArray = homeDirStr.c_str();
        e = c + 1;
        if (expandedPathCharArray) {                         // we have smth to copy
//          strlcpy(x, expandedPathCharArray, bufferSize_);
          strncpy(x, expandedPathCharArray, bufferSize_);
          x += strlen(expandedPathCharArray);
          c = e;
        } else {
          ++ier;
          ++c;
        }
      }
      else if (c[0] == '~' && c[1] != '/') { // ~user case
        int n = int(strcspn(c+1, "/ "));
//        assert((n+1) < bufferSize_ && "This should have been prevented by the truncation 'strlcat(inp, c, bufferSize_)'");
//        assert((n+1) < bufferSize_ && "This should have been prevented by the truncation 'strncat(inp, c, bufferSize_)'");
        // There is no overlap here as the buffer is segment in 4 strings of at most bufferSize_
//        (void)strlcpy(charBuffer, c + 1, n + 1); // strlcpy copy 'size-1' characters.
        (void)strncpy(charBuffer, c + 1, n + 1); // strncpy copy 'size-1' characters.
        std::string homeDirStr = getHomeDirectory();
        e = c+1+n;
        if (!homeDirStr.empty()) {                   // we have smth to copy
          expandedPathCharArray = homeDirStr.c_str();
//          strlcpy(x, expandedPathCharArray, bufferSize_);
          strncpy(x, expandedPathCharArray, bufferSize_);
          x += strlen(expandedPathCharArray);
          c = e;
        } else {
          x++[0] = c[0];
          //++ier;
          ++c;
        }
      }

      for ( ; c[0]; c++) {

        expandedPathCharArray = nullptr; e = nullptr;

        if (c[0] == '.' && c[1] == '/' && c[-1] == ' ') { // $cwd
          std::string workDirStr = getCurrentWorkingDirectory();
//          strlcpy(charBuffer, workDirStr.c_str(), bufferSize_);
          strncpy(charBuffer, workDirStr.c_str(), bufferSize_);
          expandedPathCharArray = charBuffer;
          e = c + 1;
        }
        if (expandedPathCharArray) {                          // we have smth to copy */
//          strlcpy(x, expandedPathCharArray, bufferSize_); x += strlen(expandedPathCharArray); c = e - 1; continue;
          strncpy(x, expandedPathCharArray, bufferSize_); x += strlen(expandedPathCharArray); c = e - 1; continue;
        }

        if (c[0] != '$') {                // not $, simple copy
          x++[0] = c[0];
        } else {                          // we have a $
          b = c+1;
          if (c[1] == '(') b++;
          if (c[1] == '{') b++;
          if (b[0] == '$')
            e = b+1;
          else
            for (e = b; isalnum(e[0]) || e[0] == '_'; e++) ;
          charBuffer[0] = 0; strncat(charBuffer, b, e - b);
          expandedPathCharArray = getEnvironmentVariable(charBuffer);
          if (!expandedPathCharArray) {                      // too bad, try UPPER case
            for (t = charBuffer; (t[0] = static_cast<char>(toupper(t[0]))); t++) ;
            expandedPathCharArray = getEnvironmentVariable(charBuffer);
          }
          if (!expandedPathCharArray) {                      // too bad, try Lower case
            for (t = charBuffer; (t[0] = static_cast<char>(tolower(t[0]))); t++) ;
            expandedPathCharArray = getEnvironmentVariable(charBuffer);
          }
          if (!expandedPathCharArray && !strcmp(charBuffer, "cwd")) { // it is $cwd
            std::string wd = getCurrentWorkingDirectory();
//            strlcpy(charBuffer, wd.c_str(), bufferSize_);
            strncpy(charBuffer, wd.c_str(), bufferSize_);
            expandedPathCharArray = charBuffer;
          }
          if (!expandedPathCharArray && !strcmp(charBuffer, "$")) { // it is $$ (replace by GetPid())
            snprintf(charBuffer, bufferSize_ * 4, "%d", getpid());
            expandedPathCharArray = charBuffer;
          }
          if (!expandedPathCharArray) {                      // too bad, nothing can help
#ifdef WIN32
            // if we're on windows, we can have \\SomeMachine\C$ - don't
             // complain about that, if '$' is followed by nothing or a
             // path delimiter.
             if (c[1] && c[1]!='\\' && c[1]!=';' && c[1]!='/')
                ier++;
#else
            ier++;
#endif
            x++[0] = c[0];
          } else {                       // It is OK, copy result
            int lp = int(strlen(expandedPathCharArray));
            if (lp >= bufferSize_) {
              // make sure lx will be >= bufferSize_ (see below)
//              strlcpy(x, expandedPathCharArray, bufferSize_);
              strncpy(x, expandedPathCharArray, bufferSize_);
              x += bufferSize_;
              break;
            }
            strcpy(x, expandedPathCharArray);
            x += lp;
            c = (b==c+1) ? e-1 : e;
          }
        }
      }

      x[0] = 0; lx = int(x - out);
//      if (ier && iter < 3) { strlcpy(inp, out, bufferSize_); goto again; }
      if (ier && iter < 3) { strncpy(inp, out, bufferSize_); goto again; }
      ncopy = (lx >= bufferSize_) ? bufferSize_ - 1 : lx;
      extendedFilePath_[0] = 0; strncat(extendedFilePath_, out, ncopy);

      delete[] charBuffer;

//    if (ier || ncopy != lx) {
//      ::Error("TSystem::expandEnvironmentVariables", "input: %s, output: %s", inputFilePath_, extendedFilePath_);
//      return true;
//    }

      return false;
    }
  }

  static inline std::string getHomeDirectory(){
    struct passwd *pw = getpwuid(getuid());
    return {pw->pw_dir};
  }
  static inline std::string getCurrentWorkingDirectory(){
#ifdef PATH_MAX
    char cwd[PATH_MAX];
#else
    char cwd[1024];
#endif
    if( getcwd(cwd, sizeof(cwd)) == nullptr ){
      throw std::runtime_error("getcwd() returned an invalid value.");
    }
    std::string output_cwd(cwd);
    return output_cwd;
  }
  static inline std::string expandEnvironmentVariables(const std::string &filePath_){

//    char outputName[PATH_MAX];
    char outputName[8192];
    Internals::expandEnvironmentVariables(filePath_.c_str(), outputName);

    return {outputName};
  }
  static inline std::string getExecutableName(){
    std::string outStr;
#if defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__) //check defines for your setup
    std::ifstream("/proc/self/comm") >> outStr;
#elif defined(_WIN32)
    char buf[MAX_PATH];
    GetModuleFileNameA(nullptr, buf, MAX_PATH);
    outStr = buf;
#else
    outStr = __progname;
#endif
    return outStr;
  }

}

// FS Tools
namespace GenericToolbox{

  // -- without IO dependencies (string parsing)
  static inline bool doesFilePathHasExtension(const std::string &filePath_, const std::string &extension_){
    return doesStringEndsWithSubstring(filePath_, "." + extension_);
  }
  static inline std::string getFolderPathFromFilePath(const std::string &filePath_){
    std::string folder_path;
    if(filePath_[0] == '/') folder_path += "/";
    auto splitted_path = splitString(filePath_, "/");
    folder_path += joinVectorString(
      splitted_path,
      "/",
      0,
      int(splitted_path.size()) - 1
    );
    return folder_path;
  }
  static inline std::string getFileNameFromFilePath(const std::string &filePath_, bool keepExtension_){
    auto splitStr = GenericToolbox::splitString(filePath_, "/");
    if(not splitStr.empty()){
      return ( keepExtension_ ?
        splitStr[splitStr.size()-1]:
        GenericToolbox::splitString(splitStr[splitStr.size()-1], ".")[0]
      );
    }
    return {};
  }

  // -- with direct IO dependencies
  static inline bool doesPathIsFile(const std::string &filePath_){
    struct stat info{};
    stat(filePath_.c_str(), &info);
    return S_ISREG(info.st_mode);
  }
  static inline bool doesPathIsFolder(const std::string &folderPath_){
    struct stat info{};
    stat( folderPath_.c_str(), &info );
    return bool(S_ISDIR(info.st_mode));
  }
  static inline bool doFilesAreTheSame(const std::string &filePath1_, const std::string &filePath2_){

    if( not doesPathIsFile(filePath1_) ) return false;
    if( not doesPathIsFile(filePath2_) ) return false;

    std::ifstream fileStream1(filePath1_);
    std::ifstream fileStream2(filePath2_);

    // Buffer size 1 Megabyte (or any number you like)
    size_t buffer_size = 1<<20;
    char *buffer1 = new char[buffer_size];
    char *buffer2 = new char[buffer_size];

    std::hash<std::string> hashBuffer1;
    std::hash<std::string> hashBuffer2;

    while (fileStream1 and fileStream2) {
      // Try to read next chunk of data
      fileStream1.read(buffer1, long(buffer_size));
      fileStream2.read(buffer2, long(buffer_size));

      // Get the number of bytes actually read
      if(fileStream1.gcount() != fileStream2.gcount()){
        return false;
      }

      size_t count = fileStream1.gcount();
      // If nothing has been read, break
      if( count == 0 ){
        break;
      }

      // Compare hash files
      if(hashBuffer1(buffer1) != hashBuffer2(buffer2))
        return false;

    }

    delete[] buffer1;
    delete[] buffer2;

    return true;
  }
  static inline bool mkdirPath(const std::string &newFolderPath_){
    bool result = false;
    if(doesPathIsFolder(newFolderPath_)) return true;

    std::string current_level;
    std::string level;
    std::stringstream ss(newFolderPath_);

    // split path using slash as a separator
    while (std::getline(ss, level, '/')){
      current_level += level; // append folder to the current level
      if(current_level.empty()) current_level = "/";
      current_level = removeRepeatedCharacters(current_level, "/");
      // create current level
      if(not doesPathIsFolder(current_level)){
        ::mkdir(current_level.c_str(), 0777);
        result = true;
      }
      current_level += "/"; // don't forget to append a slash
    }

    return result;

  }
  static inline bool deleteFile(const std::string &filePath_){
    if(not doesPathIsFile(filePath_)) return true;
    std::remove(filePath_.c_str());
    return not doesPathIsFile(filePath_);
  }
  static inline bool copyFile(const std::string &source_file_path_, const std::string &destination_file_path_, bool force_){

    if( not doesPathIsFile(source_file_path_) ){
      return false;
    }

    if( doesPathIsFile(destination_file_path_) ){
      if( force_ ){
        deleteFile(destination_file_path_);
      }
      else{
        return false;
      }
    }

    std::ifstream  src(source_file_path_, std::ios::binary);
    std::ofstream  dst(destination_file_path_,   std::ios::binary);

    dst << src.rdbuf();

    return true;
  }
  static inline bool mvFile(const std::string &sourceFilePath_, const std::string &destinationFilePath_, bool force_) {

    if( not doesPathIsFile(sourceFilePath_) ){
      return false;
    }

    if( doesPathIsFile(destinationFilePath_) ){
      if(force_){
        deleteFile(destinationFilePath_);
      }
      else{
        return false;
      }
    }
    else{
      std::string destination_folder_path = getFolderPathFromFilePath(destinationFilePath_);
      if(not doesPathIsFile(destination_folder_path)){
        mkdirPath(destination_folder_path);
      }
    }

    std::rename(sourceFilePath_.c_str(), destinationFilePath_.c_str());

    if(
      doesPathIsFile(destinationFilePath_)
      and not doesPathIsFile(sourceFilePath_)
      ) return true;
    else return false;
  }
  static inline size_t getHashFile(const std::string &filePath_) {
    std::hash<std::string> hashString;
    return hashString(dumpFileAsString(filePath_));
  }
  static inline long int getFileSizeInBytes(const std::string &filePath_){
    long int output_size = 0;
    if(doesPathIsFile(filePath_)){
      std::ifstream testFile(filePath_.c_str(), std::ios::binary);
      const auto begin = testFile.tellg();
      testFile.seekg (0, std::ios::end);
      const auto end = testFile.tellg();
      const auto fsize = (end-begin);
      output_size = fsize;
    }
    return output_size;
  }
  static inline void dumpStringInFile(const std::string &outFilePath_, const std::string &stringToWrite_){
    std::ofstream out(outFilePath_.c_str());
    out << stringToWrite_;
    out.close();
  }
  static inline std::string dumpFileAsString(const std::string &filePath_){
    std::string data;
    if(doesPathIsFile(filePath_)){
      std::ifstream input_file(filePath_.c_str(), std::ios::binary | std::ios::in );
      std::ostringstream ss;
      ss << input_file.rdbuf();
      data = ss.str();
      input_file.close();
    }
    return data;
  }
  static inline std::vector<std::string> dumpFileAsVectorString(const std::string &filePath_){
    std::vector<std::string> lines;
    if(doesPathIsFile(filePath_)){
      std::string data = GenericToolbox::dumpFileAsString(filePath_);
      lines = GenericToolbox::splitString(data, "\n");
    }
    for(auto& line: lines){
      if(GenericToolbox::doesStringEndsWithSubstring(line, "\r")){
        line = line.substr(0, line.size()-1);
      }
    }

    return lines;
  }
  static inline std::vector<std::string> getListOfEntriesInFolder(const std::string &folderPath_, const std::string &entryNameRegex_) {

    std::vector<std::string> entries_list;
    if(not doesPathIsFolder(folderPath_)) return entries_list;
    DIR* directory;
    directory = opendir(folderPath_.c_str()); //Open current-working-directory.
    if( directory == nullptr ) {
      std::cout << "Failed to open directory : " << folderPath_ << std::endl;
      return entries_list;
    }
    else {

      std::vector<std::string> nameElements;
      if(not entryNameRegex_.empty()){
        nameElements = GenericToolbox::splitString(entryNameRegex_, "*");
      }
      bool isValid;

      struct dirent* entry;
      while ( (entry = readdir(directory)) ) {
        isValid = true;

        if(std::string(entry->d_name) == "." or std::string(entry->d_name) == ".."){
          isValid = false;
        }
        else if(not entryNameRegex_.empty()){
          std::string entryCandidate = entry->d_name;

          for( int iElement = 0 ; iElement < int(nameElements.size()) ; iElement++ ){

            if( iElement == 0 ){
              if( not GenericToolbox::doesStringStartsWithSubstring(entryCandidate, nameElements[iElement]) ){
                isValid = false;
                break;
              }
            }
            else if( iElement+1 == int(nameElements.size()) ){
              if(not GenericToolbox::doesStringEndsWithSubstring(entryCandidate, nameElements[iElement]) ){
                isValid = false;
              }
            }
            else{
              if( not GenericToolbox::doesStringContainsSubstring(entryCandidate, nameElements[iElement])
                  ){
                isValid = false;
                break;
              }
            }

            if( iElement+1 != int(nameElements.size()) ){
              entryCandidate = GenericToolbox::splitString(entryCandidate, nameElements[iElement]).back();
            }
          }
        }
        if(isValid) entries_list.emplace_back(entry->d_name);
      }
      closedir(directory);
      return entries_list;
    }

  }
  static inline std::vector<std::string> getListOfSubfoldersInFolder(const std::string &folderPath_, const std::string &entryNameRegex_) {
    std::vector<std::string> subfolders_list;
    if(not doesPathIsFolder(folderPath_)) return subfolders_list;
    DIR* directory;
    directory = opendir(folderPath_.c_str()); //Open current-working-directory.
    if( directory == nullptr ) {
      std::cout << "Failed to open directory : " << folderPath_ << std::endl;
      return subfolders_list;
    } else {

        std::vector<std::string> nameElements;
        if(not entryNameRegex_.empty()){
            nameElements = GenericToolbox::splitString(entryNameRegex_, "*");
        }
        bool isValid;

      struct dirent* entry;
      while ( (entry = readdir(directory)) ) {
          isValid = true;
        std::string folder_candidate = folderPath_ + "/" + std::string(entry->d_name);
        if(doesPathIsFolder(folder_candidate)){
            if(std::string(entry->d_name) == "." or std::string(entry->d_name) == ".."){
                isValid = false;
            }
            else if(not entryNameRegex_.empty()){
              std::string entryCandidate = entry->d_name;

              for( int iElement = 0 ; iElement < int(nameElements.size()) ; iElement++ ){

                if( iElement == 0 ){
                  if( not GenericToolbox::doesStringStartsWithSubstring(entryCandidate, nameElements[iElement]) ){
                    isValid = false;
                    break;
                  }
                }
                else if( iElement+1 == int(nameElements.size()) ){
                  if(not GenericToolbox::doesStringEndsWithSubstring(entryCandidate, nameElements[iElement]) ){
                    isValid = false;
                  }
                }
                else{
                  if( not GenericToolbox::doesStringContainsSubstring(entryCandidate, nameElements[iElement])
                      ){
                    isValid = false;
                    break;
                  }
                }

                if( iElement+1 != int(nameElements.size()) ){
                  entryCandidate = GenericToolbox::splitString(entryCandidate, nameElements[iElement]).back();
                }
              }
            }
            if(isValid) subfolders_list.emplace_back(entry->d_name);
        }
      }
      closedir(directory);
      return subfolders_list;
    }

  }
  static inline std::vector<std::string> getListOfFilesInFolder(const std::string &folderPath_, const std::string &entryNameRegex_){
    std::vector<std::string> files_list;
    if(not doesPathIsFolder(folderPath_)) return files_list;
    DIR* directory;
    directory = opendir(folderPath_.c_str()); //Open current-working-directory.
    if( directory == nullptr ) {
      std::cout << "Failed to open directory : " << folderPath_ << std::endl;
      return files_list;
    } else {

        std::vector<std::string> nameElements;
        if(not entryNameRegex_.empty()){
            nameElements = GenericToolbox::splitString(entryNameRegex_, "*");
        }
        bool isValid;

      struct dirent* entry;
      while ( (entry = readdir(directory)) ) {
          isValid = true;
        std::string file_candidate = folderPath_ + "/" + std::string(entry->d_name);
        if(doesPathIsFile(file_candidate)){
            if(not entryNameRegex_.empty()){
              std::string entryCandidate = entry->d_name;

              for( int iElement = 0 ; iElement < int(nameElements.size()) ; iElement++ ){

                if( iElement == 0 ){
                  if( not GenericToolbox::doesStringStartsWithSubstring(entryCandidate, nameElements[iElement]) ){
                    isValid = false;
                    break;
                  }
                }
                else if( iElement+1 == int(nameElements.size()) ){
                  if(not GenericToolbox::doesStringEndsWithSubstring(entryCandidate, nameElements[iElement]) ){
                    isValid = false;
                  }
                }
                else{
                  if( not GenericToolbox::doesStringContainsSubstring(entryCandidate, nameElements[iElement])
                      ){
                    isValid = false;
                    break;
                  }
                }

                if( iElement+1 != int(nameElements.size()) ){
                  entryCandidate = GenericToolbox::splitString(entryCandidate, nameElements[iElement]).back();
                }
              }
            }
            if(isValid) files_list.emplace_back(entry->d_name);
        }
      }
      closedir(directory);
      return files_list;
    }
  }

  // -- with direct IO dependencies
  static inline bool doesFolderIsEmpty(const std::string &folderPath_){
    if(not doesPathIsFolder(folderPath_)) return false;
    return getListOfEntriesInFolder(folderPath_, std::string()).empty();
  }
  static inline std::vector<std::string> getListFilesInSubfolders(const std::string &folderPath_) {

    // WARNING : Recursive function
    std::vector<std::string> output_file_paths = getListOfFilesInFolder(folderPath_);

    auto subfolder_names_list = getListOfSubfoldersInFolder(folderPath_);
    for(auto &subfolder_name : subfolder_names_list){
      std::string subfolder_full_path = folderPath_;
      subfolder_full_path += "/";
      subfolder_full_path += subfolder_name;
      auto subfile_names_list = getListFilesInSubfolders(subfolder_full_path);
      for(auto &subfile_name : subfile_names_list){
        std::string relative_subfile_path;
        relative_subfile_path += subfolder_name;
        relative_subfile_path += "/";
        relative_subfile_path += subfile_name;
        output_file_paths.emplace_back(removeRepeatedCharacters(relative_subfile_path, "/"));
      }
    }

    return output_file_paths;

  }

}



// Hardware Tools
#if defined(_WIN32)
// Windows
#include <windows.h>
#include <psapi.h>
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <Windows.h>
#elif defined(__APPLE__) && defined(__MACH__)
// MacOS
#include <unistd.h>
#include <sys/resource.h>
#include <mach/mach.h>
#include <sys/ioctl.h>
#include <array>

#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
// Linux
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <sys/ioctl.h>

#elif (defined(_AIX) || defined(__TOS__AIX__)) || (defined(__sun__) || defined(__sun) || defined(sun) && (defined(__SVR4) || defined(__svr4__)))
// AIX and Solaris
#include <unistd.h>
#include <sys/resource.h>
#include <fcntl.h>
#include <procfs.h>
#include <sys/ioctl.h>

#else
// Unsupported
#endif
namespace GenericToolbox{

  struct CpuStat{
    inline CpuStat(){ this->getCpuUsageByProcess(); }
    clock_t lastCPU{}, lastSysCPU{}, lastUserCPU{};
    inline double getCpuUsageByProcess(){
      double percent{0};
#if defined(_WIN32)
#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__) || defined(__APPLE__) && defined(__MACH__)
      struct tms timeSample{};
      clock_t now;

      now = times(&timeSample);
      if (now <= lastCPU || timeSample.tms_stime < lastSysCPU ||
          timeSample.tms_utime < lastUserCPU){
        //Overflow detection. Just skip this value.
        percent = -1.0;
      }
      else{
        percent = double(timeSample.tms_stime - lastSysCPU) +
                  double(timeSample.tms_utime - lastUserCPU);
        percent /= double(now - lastCPU);
        percent *= 100;
      }
      lastCPU = now;
      lastSysCPU = timeSample.tms_stime;
      lastUserCPU = timeSample.tms_utime;
#endif
      return percent;
    }
  };
  static CpuStat cs{};

  static inline size_t getProcessMemoryUsage(){
    /**
     * Returns the current resident set size (physical memory use) measured
     * in bytes, or zero if the value cannot be determined on this OS.
     */
#if defined(_WIN32)
    // Windows
    PROCESS_MEMORY_COUNTERS memCounter;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &memCounter, sizeof memCounter))
        return (size_t)memCounter.WorkingSetSize;
    return (size_t)0; /* get process mem info failed */

#elif defined(__APPLE__) && defined(__MACH__)
    // MacOS
    struct mach_task_basic_info info{};
    mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;
    if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &count) == KERN_SUCCESS)
      return (size_t)info.resident_size;
    return (size_t)0; /* query failed */


#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
    // Linux
//    long rss = 0L;
//    FILE* fp = NULL;
//    if ( (fp = fopen( "/proc/self/statm", "r" )) == NULL )
//        return (size_t)0L;      /* Can't open? */
//    if ( fscanf( fp, "%*s%ld", &rss ) != 1 )
//    {
//        fclose( fp );
//        return (size_t)0L;      /* Can't read? */
//    }
//    fclose( fp );
//    return (size_t)rss * (size_t)sysconf( _SC_PAGESIZE);
    // Physical Memory currently used by current process
    // https://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
    FILE* file = fopen("/proc/self/status", "r");
    size_t result{0};
    char line[128];

    while (fgets(line, 128, file) != nullptr){
      if (strncmp(line, "VmRSS:", 6) == 0){
        result = strlen(line);
        const char* p = line;
        while (*p <'0' || *p > '9') p++;
        line[result-3] = '\0';
        result = size_t(atol(p));
        break;
      }
    }
    fclose(file);
    return result*1000;
#else
    // AIX, BSD, Solaris, and Unknown OS
    return (size_t)0L;          /* Unsupported. */

#endif
  }
  static inline size_t getProcessMaxMemoryUsage(){
    /**
     * Returns the peak (maximum so far) resident set size (physical
     * memory use) measured in bytes, or zero if the value cannot be
     * determined on this OS.
     */
#if defined(_WIN32)
    // Windows
    PROCESS_MEMORY_COUNTERS info;
    GetProcessMemoryInfo( GetCurrentProcess( ), &info, sizeof(info) );
    return (size_t)info.PeakWorkingSetSize;

#elif defined(__APPLE__) && defined(__MACH__)
    // MacOS
    struct mach_task_basic_info info{};
    mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;
    if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO, (task_info_t)&info, &count) == KERN_SUCCESS)
      return (size_t)info.resident_size_max;
    return (size_t)0; /* query failed */

#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__)
    // Linux
    struct rusage rusage;
    if (!getrusage(RUSAGE_SELF, &rusage))
        return (size_t)rusage.ru_maxrss;
    return (size_t)0; /* query failed */

#elif (defined(_AIX) || defined(__TOS__AIX__)) || (defined(__sun__) || defined(__sun) || defined(sun) && (defined(__SVR4) || defined(__svr4__)))
    // AIX and Solaris
    struct psinfo psinfo;
    int fd = -1;
    if ( (fd = open( "/proc/self/psinfo", O_RDONLY )) == -1 )
        return (size_t)0L;      /* Can't open? */
    if ( read( fd, &psinfo, sizeof(psinfo) ) != sizeof(psinfo) )
    {
        close( fd );
        return (size_t)0L;      /* Can't read? */
    }
    close( fd );
    return (size_t)(psinfo.pr_rssize * 1024L);
#else
    // Unknown OS
    return (size_t)0L;          /* Unsupported. */
#endif
  }
  static inline double getCpuUsageByProcess(){
    return cs.getCpuUsageByProcess();
  }
  static inline long getProcessMemoryUsageDiffSinceLastCall(){
      size_t currentProcessMemoryUsage = getProcessMemoryUsage();
      long outVal = static_cast<long>(currentProcessMemoryUsage) - static_cast<long>(Hardware::lastProcessMemoryUsage);
      Hardware::lastProcessMemoryUsage = currentProcessMemoryUsage;
      return outVal;
  }
  static inline int getTerminalWidth(){
    int outWith = 0;
#if defined(_WIN32)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    outWith = (int)(csbi.dwSize.X);
//    outWith = (int)(csbi.dwSize.Y);
#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__) \
    || (defined(_AIX) || defined(__TOS__AIX__)) || (defined(__sun__) || defined(__sun) || defined(sun) && (defined(__SVR4) || defined(__svr4__))) \
    || ( defined(__APPLE__) && defined(__MACH__) )
    struct winsize winSize{};
    ioctl(fileno(stdout), TIOCGWINSZ, &winSize);
    outWith = (int)(winSize.ws_col);
//    outWith = (int)(winSize.ws_row);
#endif // Windows/Linux
    return outWith;
  }
  static inline int getTerminalHeight(){
    int outWith = 0;
#if defined(_WIN32)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
//    outWith = (int)(csbi.dwSize.X);
    outWith = (int)(csbi.dwSize.Y);
#elif defined(__linux__) || defined(__linux) || defined(linux) || defined(__gnu_linux__) \
    || (defined(_AIX) || defined(__TOS__AIX__)) || (defined(__sun__) || defined(__sun) || defined(sun) && (defined(__SVR4) || defined(__svr4__))) \
    || ( defined(__APPLE__) && defined(__MACH__) )
    struct winsize w{};
    ioctl(fileno(stdout), TIOCGWINSZ, &w);
//    outWith = (int)(w.ws_col);
    outWith = (int)(w.ws_row);
#endif // Windows/Linux
    return outWith;
  }

  static inline std::string parseTimeUnit(long long nbMicroSec_){
    if(nbMicroSec_ / 1000 < 9 ){ // print in µs if less than 9ms
      return std::to_string(nbMicroSec_) + "µs";
    }
    nbMicroSec_ /= 1000; // in ms
    if(nbMicroSec_ / nbMicroSec_ < 3){ // print in ms if less than 3s
      return std::to_string(nbMicroSec_) + "ms";
    }
    nbMicroSec_ /= 1000; // in s
    if(nbMicroSec_ / 60 < 5){
      return std::to_string(nbMicroSec_) + "s";
    }
    nbMicroSec_ /= 60; // in min
    if(nbMicroSec_ / 60 < 3){ // print in min
      return std::to_string(nbMicroSec_) + "min";
    }
    nbMicroSec_ /= 60; // in hours
    if(nbMicroSec_ / 24 < 2){ // print in hours
      return std::to_string(nbMicroSec_) + "h";
    }
    nbMicroSec_ /= 24; // in days
    if(nbMicroSec_ / 365 < 2){ // print in hours
      return std::to_string(nbMicroSec_) + "d";
    }
    nbMicroSec_ /= 365; // in days
    return std::to_string(nbMicroSec_) + "y";
  }
  static inline std::string getElapsedTimeSinceLastCallStr( const std::string& key_ ) {
    return GenericToolbox::parseTimeUnit(GenericToolbox::getElapsedTimeSinceLastCallInMicroSeconds(key_));
  }
  static inline std::string getElapsedTimeSinceLastCallStr(int instance_)
  {
    return GenericToolbox::parseTimeUnit(getElapsedTimeSinceLastCallInMicroSeconds(instance_));
  }
  static inline long long getElapsedTimeSinceLastCallInMicroSeconds( const std::string& key_ ) {
    auto newTimePoint = std::chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(
      newTimePoint - Internals::_lastTimePointMapStr_[key_]
    );
    Internals::_lastTimePointMapStr_[key_] = newTimePoint;
    return microseconds.count();
  }
  static inline long long getElapsedTimeSinceLastCallInMicroSeconds(int instance_){
    auto newTimePoint = std::chrono::high_resolution_clock::now();
    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(
      newTimePoint - Internals::_lastTimePointMap_[instance_]
      );
    Internals::_lastTimePointMap_[instance_] = newTimePoint;
    return microseconds.count();
  }
  static inline std::vector<std::string> getOutputOfShellCommand(const std::string& cmd_) {
    // Inspired from: https://stackoverflow.com/questions/478898/how-do-i-execute-a-command-and-get-the-output-of-the-command-within-c-using-po
    std::array<char, 128> buffer{};
    std::string resultStr;
    std::vector<std::string> output;
#if defined(_WIN32)
    std::unique_ptr<FILE, decltype(&_pclose)> pipe(_popen(cmd_.c_str(), "r"), _pclose);
#else
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd_.c_str(), "r"), pclose);
#endif
    if (!pipe) {
//      throw std::runtime_error("popen() failed!");
    }
    else{
      while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        resultStr += buffer.data();
      }
      output = GenericToolbox::splitString(resultStr, "\n", true);
    }
    return output;
  }

}


// Misc Tools
namespace GenericToolbox{

  static inline std::string getClassName(const std::string& PRETTY_FUNCTION_){
    size_t colons = PRETTY_FUNCTION_.find("::");
    if (colons == std::string::npos)
      return "::";
    size_t begin = PRETTY_FUNCTION_.substr(0, colons).rfind(' ') + 1;
    size_t end = colons - begin;

    return PRETTY_FUNCTION_.substr(begin, end);
  }
  static inline std::string getMethodName(const std::string& PRETTY_FUNCTION_){
    size_t colons = PRETTY_FUNCTION_.find("::");
    size_t begin = PRETTY_FUNCTION_.substr(0, colons).rfind(' ') + 1;
    size_t end = PRETTY_FUNCTION_.rfind('(') - begin;

    return PRETTY_FUNCTION_.substr(begin, end) + "()";
  }

}


// Macro Tools
#define GT_INTERNALS_VA_TO_STR(...) #__VA_ARGS__

#define GT_INTERNALS_ENUM_EXPANDER(_enumName_, _intOffset_, _v1_, ...)\
  enum _enumName_ { _v1_ =  _intOffset_, __VA_ARGS__, _enumName_##_OVERFLOW };\
  namespace _enumName_##EnumNamespace{\
    static const char *enumNamesAggregate = GT_INTERNALS_VA_TO_STR(_v1_, __VA_ARGS__); \
                                                                      \
    static inline size_t getEnumSize(){\
      return std::count(&enumNamesAggregate[0], &enumNamesAggregate[strlen(enumNamesAggregate)], ',')+1;\
    }                                                                 \
    static inline std::string getEnumStr(int enumValue_){\
      enumValue_ -= (_intOffset_);\
      if( enumValue_ < 0 || enumValue_ >= getEnumSize() ) throw std::runtime_error("invalid enum.");\
      std::string out; std::stringstream ss{enumNamesAggregate};\
      while (enumValue_-- >= 0) { std::getline(ss, out, ','); } \
      return GenericToolbox::trimString(out, " ");\
    }\
    static inline std::vector<std::string> getEnumNamesList(){              \
      std::vector<std::string> out(getEnumSize());                    \
      for(int iEnum = 0 ; iEnum < int(out.size()) ; iEnum++){ out[iEnum] = getEnumStr(iEnum+(_intOffset_)); } \
      return out;              \
    }                                                              \
    static inline std::vector<_enumName_> getEnumList(){                   \
      std::vector<_enumName_> output(_enumName_##_OVERFLOW);         \
      for( int iIndex = _intOffset_ ; iIndex < _enumName_##_OVERFLOW ; iIndex++ ){     \
        output.at(iIndex) = (static_cast<_enumName_>(iIndex));      \
      }                                                            \
      return output;\
    }\
    static inline std::string toString(int enumValue_, bool excludeEnumName_ = false){      \
      if( excludeEnumName_ ) return getEnumStr(enumValue_);        \
      return {(#_enumName_) + std::string{"::"} + getEnumStr(enumValue_)};\
    }\
    static inline std::string toString(_enumName_ enumValue_, bool excludeEnumName_ = false){\
      return _enumName_##EnumNamespace::toString(static_cast<int>(enumValue_), excludeEnumName_);       \
    }\
    static inline int toEnumInt(const std::string& enumStr_, bool throwIfNotFound_ = false){\
      for( int enumIndex = _intOffset_ ; enumIndex < _enumName_::_enumName_##_OVERFLOW ; enumIndex++ ){             \
        if( _enumName_##EnumNamespace::toString(enumIndex) == enumStr_ ){ return enumIndex; } \
        if( _enumName_##EnumNamespace::toString(enumIndex, true) == enumStr_ ){ return enumIndex; } \
      }                                                            \
      if( throwIfNotFound_ ){                                        \
        std::cout << "Could not find \"" << enumStr_ << "\" in: " << GenericToolbox::parseVectorAsString(getEnumNamesList()) << std::endl; \
        throw std::runtime_error( enumStr_ + " not found in " + #_enumName_ );   \
      }                                                             \
/*      return _intOffset_ - 1; */ \
      return int(_enumName_##_OVERFLOW); /* returns invalid value */\
    }\
    static inline _enumName_ toEnum(const std::string& enumStr_, bool throwIfNotFound_ = false){                         \
      return static_cast<_enumName_>(_enumName_##EnumNamespace::toEnumInt(enumStr_, throwIfNotFound_));  \
    }\
  }

#endif //CPP_GENERIC_TOOLBOX_GENERICTOOLBOX_IMPL_H
