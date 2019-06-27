/******************************************************************************
 * File:    json.h
 *
 * Version: 1.0
 *
 * Date:    2019/03/01
 *
 * Copyright (C) 2019 J C Gonzalez
 *_____________________________________________________________________________
 *
 * Topic: General Information
 *
 * Purpose:
 *   Declare Json related classes
 *
 * Created by:
 *   J C Gonzalez
 *
 * Status:
 *   Prototype
 *
 * Dependencies:
 *   none
 *
 * Files read / modified:
 *   none
 *
 * History:
 *   See <Changelog>
 *
 * About: License Conditions
 *   See <License>
 *
 ******************************************************************************/

#ifndef JSON_H
#define JSON_H

#include <iostream>
#include <sstream>
#include <fstream>

#include <string>
#include <vector>
#include <map>
#include <stack>

#include <cassert>

#define TLISTOF_JSON_TYPE                       \
    T(UNKNOWN),                                 \
        T(BOOL),                                \
        T(INT),                                 \
        T(FLOAT),                               \
        T(STRING),                              \
        T(ARRAY),                               \
        T(OBJECT)

#define TLISTOF_TOKENS                          \
        T(OPEN_BRACE, '{'),                     \
            T(CLOSE_BRACE, '}'),                \
            T(OPEN_BRACKET, '['),               \
            T(CLOSE_BRACKET, ']'),              \
            T(QUOTES, '"'),                     \
            T(COLON, ':'),                      \
            T(COMMA, ','),                      \
            T(BACKSLASH, '\\'),                 \
            T(SLASH, '/'),                      \
            T(EOL_CHAR, 'n'),                   \
            T(CR_CHAR, 'r'),                    \
            T(BACKSPACE, 'b'),                  \
            T(FORMFEED, 'f'),                   \
            T(TAB_CHAR, 't'),                   \
            T(UNICODE_CHAR, 'u'),               \
            T(SIGN_MINUS, '-'),                 \
            T(SIGN_PLUS, '+'),                  \
            T(SEP_DOT, '.'),                    \
            T(SPACE, ' '),                      \
            T(TABULATOR, '\t'),                 \
            T(NEWLINE, '\n'),                   \
            T(COMMENT_DASH, '-'),               \
            T(COMMENT_SLASH, '/'),              \
            T(COMMENT_HASH, '#'),               \
            T(DIGIT_ZERO, '0'),                 \
            T(DIGIT_NINE, '9'),                 \
            T(FALSE_START, 'f'),                \
            T(TRUE_START, 't')

////////////////////////////////////////////////////////////////////////////
// Namespace: json
//
// Library namespace
////////////////////////////////////////////////////////////////////////////
namespace json {

#define T(x) JSON_ ## x
    enum ValueType { TLISTOF_JSON_TYPE };
#undef T

    // Forward class declaration
    class Value;
    class Array;

    //==========================================================================
    // Class: Object
    //
    // JSON Object abstraction
    //==========================================================================
    class Object {
    public:
        Object();
        Object(std::string key, bool bvalue);       
        Object(std::string key, int ivalue);        
        Object(std::string key, double xvalue);     
        Object(std::string key, std::string svalue);
        Object(std::string key, Array & avalue);    
        Object(std::string key, Object & ovalue);   

        ~Object();

        void append(std::string key, bool bvalue);
        void append(std::string key, int ivalue);
        void append(std::string key, double xvalue);
        void append(std::string key, std::string svalue);
        void append(std::string key, Array avalue);
        void append(std::string key, Object ovalue);

        void append(std::string key, Value & ovalue);

        int size();

        std::pair<std::string, Value> operator[](int i);
        Value & operator[](std::string s);

        bool exists(std::string s);

        friend std::ostream & operator<<(std::ostream & os, const Object & o);

        typedef std::map<std::string, Value>::iterator iterator;

        iterator begin();
        iterator end();

        std::vector<std::string> & getKeys();
	std::string str();

    private:
        std::map<std::string, Value> obj;
        std::vector<std::string> keys;
    
        ValueType type;
    };

    //==========================================================================
    // Class: Array
    //
    // JSON Array abstraction
    //==========================================================================
    class Array {
    public:
        Array();
        Array(std::vector<bool> & vb);
        Array(std::vector<int> & vi);
        Array(std::vector<double> & vx);
        Array(std::vector<std::string> & vs);
        Array(std::vector<Array> & va);
        Array(std::vector<Object> & vo);

        void append(bool b);       
        void append(int i);        
        void append(double x);     
        void append(std::string s);
        void append(Array a);      
        void append(Object o);     
        void append(Value & v);
        
        void assign(std::vector<bool> & v);
        void assign(std::vector<int> & v);
        void assign(std::vector<double> & v);
        void assign(std::vector<std::string> & v);
        void assign(std::vector<Array> & v);
        void assign(std::vector<Object> & v);

        Value & operator[](int i);

        int size();

        void clear();
    
        friend std::ostream & operator<<(std::ostream & os, const Array & a);

        typedef std::vector<Value>::iterator iterator;

        iterator begin();
        iterator end();

    private:
        void ensureTypeIs(ValueType t);

    public:
        ValueType type;

    private:
        std::vector<Value> arr;
    };

    //==========================================================================
    // Class: Value
    //
    // JSON Value abstraction
    //==========================================================================
    class Value {
    public:
        Value();
        Value(bool x);
        Value(int x);
        Value(double x);
        Value(std::string & x);
        Value(Array & x);
        Value(Object & x);

        ~Value();

        void assign(bool x);
        void assign(int x);
        void assign(double x);
        void assign(std::string & x);
        void assign(Array & x);
        void assign(Object & x);

        void operator=(bool x);
        void operator=(int x);
        void operator=(double x);
        void operator=(std::string & x);
        void operator=(Array & x);
        void operator=(Object & x);

        bool asBool();
        int asInt();
        double asFloat();
        std::string asString();
        Array asArray();
        Object asObject();

        Value & operator[](int i);
        Value & operator[](const char * s);
        Value & operator[](std::string s);

        friend std::ostream & operator<<(std::ostream & os, const Value & v);

    public:
        ValueType type;

    private:
        bool        bvalue;
        int         ivalue;
        double      xvalue;
        std::string svalue;
        Array       avalue;
        Object      ovalue;

    };

    //==========================================================================
    // Class: Parser
    //
    // JSON Parser, to generate underlying JSON structure from string or file
    //==========================================================================
    class Parser {
    public:
        Parser();
        ~Parser();

    public:
        bool parse(std::string & s, Object & root);
        bool parseFile(std::string filename, Object & root);

    };

    void enableFormattedOutput(std::string tab = std::string("    "),
                               int maxlvl = 999);
    void disableFormattedOutput();

    enum FloatFormat { SCI, FIX, DEFAULT };
    
    void setFloatFormat(FloatFormat fmt = DEFAULT, int prec = -1);
    
#define T(a,b)  TOKEN_ ## a
    // Enum: Tokens
    enum Tokens { TLISTOF_TOKENS };
#undef T
    
#define T(a,b)  a
    // Variables: Individual tokens
    extern const char TLISTOF_TOKENS;
#undef T

    // Variable: TokenName
    extern std::map<char, std::string> TokenName;

    // Variables: Used in formatting output
    // - formattedOutput: Boolean, false means compressed output
    // - indent: Int, indent level, starting at 0
    // - indentStr: String used for indentation at each level
    // - indentStr: String repeated "indent" times at each level
    extern bool        formattedOutput;
    extern int         indent;
    extern int         formattedMaxLevel;
    extern std::string indentStr;
    extern std::string indentStep;      

    extern FloatFormat floatFmt;
    extern int floatPrecision;

}



#endif // JSON_H
