/******************************************************************************
 * File:    json.cpp
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
 *   Implementation of Json related classes
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

#include "json.h"

#include <iomanip>
#include <type_traits>

template<class Fun>
inline bool DoLog(Fun f, std::string message, const char *expression,
                  const char *filename, int line) {
    static_assert(std::is_same<bool, decltype(f())>::value,
                  "Predicate must return a bool.");
    if (!(f())) {
        std::cerr << filename << '@' << line << ": '"
                  << expression << "' is false.";
        if (!message.empty()) { std::cerr << ' ' << message; }
        std::cerr << std::endl;
        return false;
    }
    return true;
}

#if defined(_DEBUG) || defined(DEBUG)
#    define HALT true

#    define WITH_MESSAGE_(expr, x) [&](){return (expr);}, x, #expr
#    define WITHOUT_MESSAGE_(expr) [&](){return (expr);}, std::string{}, #expr
#    define PICK_ASSERTION_ARGS_(_1, _2, WHICH_, ...) WHICH_
#    define CREATE_ASSERTION_ARGS_(...) PICK_ASSERTION_ARGS_(__VA_ARGS__, WITH_MESSAGE_, WITHOUT_MESSAGE_)(__VA_ARGS__)
#    define NT_ASSERT(...) if (!DoLog(CREATE_ASSERTION_ARGS_(__VA_ARGS__), __FILE__, __LINE__)) __debugbreak()
#else
#    define HALT false
#    define NT_ASSERT(...)
#endif

namespace json {

#define T(a,b)  a = b
    const char TLISTOF_TOKENS;
#undef T

#define T(a,b)  {a, #a}
    std::map<char, std::string> TokenName = { TLISTOF_TOKENS };
#undef T

    bool formattedOutput = false;
    int formattedMaxLevel = 999;    
    int         indent = 0;
    std::string indentStr("");
    std::string indentStep("    ");

    FloatFormat floatFmt = DEFAULT;
    int floatPrecision = -1;

    //======================================================================
    
    Object::Object() : type(JSON_OBJECT) {}
    Object::~Object() {}

    int Object::size() { return keys.size(); }

    Object::Object(std::string key, bool bvalue)        : type(JSON_OBJECT) { append(key, bvalue); }
    Object::Object(std::string key, int ivalue)         : type(JSON_OBJECT) { append(key, ivalue); }
    Object::Object(std::string key, double xvalue)      : type(JSON_OBJECT) { append(key, xvalue); }
    Object::Object(std::string key, std::string svalue) : type(JSON_OBJECT) { append(key, svalue); }
    Object::Object(std::string key, Array & avalue)     : type(JSON_OBJECT) { append(key, avalue); }
    Object::Object(std::string key, Object & ovalue)    : type(JSON_OBJECT) { append(key, ovalue); }

    void Object::append(std::string key, bool bvalue)
    {
	std::map<std::string, Value>::iterator it = obj.find(key);
	if (it == obj.end()) { keys.push_back(key); }
	obj[key] = Value(bvalue); 
    }
    void Object::append(std::string key, int ivalue)
    {
	std::map<std::string, Value>::iterator it = obj.find(key);
	if (it == obj.end()) { keys.push_back(key); }
        obj[key] = Value(ivalue); 
    }
    void Object::append(std::string key, double xvalue)
    {
	std::map<std::string, Value>::iterator it = obj.find(key);
	if (it == obj.end()) { keys.push_back(key); }
        obj[key] = Value(xvalue); 
    }
    void Object::append(std::string key, std::string svalue)
    {
	std::map<std::string, Value>::iterator it = obj.find(key);
	if (it == obj.end()) { keys.push_back(key); }
        obj[key] = Value(svalue); 
    }
    void Object::append(std::string key, Array avalue)
    {
	std::map<std::string, Value>::iterator it = obj.find(key);
	if (it == obj.end()) { keys.push_back(key); }
        obj[key] = Value(avalue); 
    }
    void Object::append(std::string key, Object ovalue)
    {
	std::map<std::string, Value>::iterator it = obj.find(key);
	if (it == obj.end()) { keys.push_back(key); }
        obj[key] = Value(ovalue); 
    }

    void Object::append(std::string key, Value & v)
    {
        switch (v.type) {
        case JSON_BOOL:   append(key, v.asBool());    break;
        case JSON_INT:    append(key, v.asInt());     break;
        case JSON_FLOAT:  append(key, v.asFloat());   break;
        case JSON_STRING: append(key, v.asString());  break;
        case JSON_ARRAY:  append(key, v.asArray());   break;
        case JSON_OBJECT: append(key, v.asObject());  break;
        case JSON_UNKNOWN: 
        defaut: break;
        }
    }           

    std::pair<std::string, Value> Object::operator[](int i)
    {
        std::string k(keys.at(i));
        return std::pair<std::string, Value>(k, obj[k]);
    }

    Value & Object::operator[](std::string s)
    {
        std::map<std::string, Value>::iterator it = obj.find(s);
        return it->second;
    }

    std::vector<std::string> & Object::getKeys() { return keys; }
    std::string Object::str() {
	std::stringstream ss("");
	ss << *this;
	return ss.str();
    }

    Object::iterator Object::begin() { return obj.begin(); }
    
    Object::iterator Object::end() { return obj.end(); }

    bool Object::exists(std::string s)
    {
        return (obj.find(s) != obj.end());
    }

    //======================================================================
    
    Array::Array() : type(JSON_UNKNOWN) {}
    Array::Array(std::vector<bool> & vb)        { assign(vb); }
    Array::Array(std::vector<int> & vi)         { assign(vi); }
    Array::Array(std::vector<double> & vx)      { assign(vx); }
    Array::Array(std::vector<std::string> & vs) { assign(vs); }
    Array::Array(std::vector<Array> & va)       { assign(va); }
    Array::Array(std::vector<Object> & vo)      { assign(vo); }

    void Array::append(bool b)          { ensureTypeIs(JSON_BOOL);   arr.push_back(Value(b)); }
    void Array::append(int i)           { ensureTypeIs(JSON_INT);    arr.push_back(Value(i)); }
    void Array::append(double x)        { ensureTypeIs(JSON_FLOAT);  arr.push_back(Value(x)); }
    void Array::append(std::string s)   { ensureTypeIs(JSON_STRING); arr.push_back(Value(s)); }
    void Array::append(Array a)         { ensureTypeIs(JSON_ARRAY);  arr.push_back(Value(a)); }
    void Array::append(Object o)        { ensureTypeIs(JSON_OBJECT); arr.push_back(Value(o)); }

    void Array::append(Value & v)
    {
        ensureTypeIs(v.type);
        switch (v.type) {
        case JSON_BOOL:   append(v.asBool());    break;
        case JSON_INT:    append(v.asInt());     break;
        case JSON_FLOAT:  append(v.asFloat());   break;
        case JSON_STRING: append(v.asString());  break;
        case JSON_ARRAY:  append(v.asArray());   break;
        case JSON_OBJECT: append(v.asObject());  break;
        case JSON_UNKNOWN: 
        defaut: break;
        }
    }           
        
    Value & Array::operator[](int i)
    {
        return arr[i];
    }

    void Array::assign(std::vector<bool> & v)
    {
        type = JSON_BOOL;
        clear();
        for (auto const & i: v) { arr.push_back(Value(i)); }
    }
    void Array::assign(std::vector<int> & v)
    {
        type = JSON_INT;
        clear();
        for (auto & i: v) { arr.push_back(Value(i)); }
    }
    void Array::assign(std::vector<double> & v)
    {
        type = JSON_FLOAT;
        clear();
        for (auto & i: v) { arr.push_back(Value(i)); }
    }
    void Array::assign(std::vector<std::string> & v)
    {
        type = JSON_STRING;
        clear();
        for (auto & i: v) { arr.push_back(Value(i)); }
    }
    void Array::assign(std::vector<Array> & v)
    {
        type = JSON_ARRAY;
        clear();
        for (auto & i: v) { arr.push_back(Value(i)); }
    }
    void Array::assign(std::vector<Object> & v)
    {
        type = JSON_OBJECT;
        clear();
        for (auto & i: v) { arr.push_back(Value(i)); }
    }

    int Array::size() { return arr.size(); }

    void Array::clear() { arr.clear(); }
    
    void Array::ensureTypeIs(ValueType t)
    {
        NT_ASSERT((type == t) || (type == JSON_UNKNOWN));
        type = t;
    }

    Array::iterator Array::begin() { return arr.begin(); }
    
    Array::iterator Array::end() { return arr.end(); }
    
    //======================================================================
    
    Value::Value() : type(JSON_UNKNOWN) {}

    Value::Value(bool x)     :      bvalue(x), type(JSON_BOOL) {}
    Value::Value(int x)      :      ivalue(x), type(JSON_INT) {}
    Value::Value(double x)   :      xvalue(x), type(JSON_FLOAT) {}
    Value::Value(std::string & x) : svalue(x), type(JSON_STRING) {}
    Value::Value(Array & x)  :      avalue(x), type(JSON_ARRAY) {}
    Value::Value(Object & x) :      ovalue(x), type(JSON_OBJECT) {}

    Value::~Value() {}

    void Value::assign(bool x)          { bvalue = x; type = JSON_BOOL; }
    void Value::assign(int x)           { ivalue = x; type = JSON_INT; }
    void Value::assign(double x)        { xvalue = x; type = JSON_FLOAT; }
    void Value::assign(std::string & x) { svalue = x; type = JSON_STRING; }
    void Value::assign(Array & x)       { avalue = x; type = JSON_ARRAY; }
    void Value::assign(Object & x)      { ovalue = x; type = JSON_OBJECT; }
         
    void Value::operator=(bool x)          { bvalue = x; type = JSON_BOOL; }
    void Value::operator=(int x)           { ivalue = x; type = JSON_INT; }
    void Value::operator=(double x)        { xvalue = x; type = JSON_FLOAT; }
    void Value::operator=(std::string & x) { svalue = x; type = JSON_STRING; }
    void Value::operator=(Array & x)       { avalue = x; type = JSON_ARRAY; }
    void Value::operator=(Object & x)      { ovalue = x; type = JSON_OBJECT; }

    bool Value::asBool()
    {
        switch (type) {
        case JSON_BOOL:   return bvalue; break;
        case JSON_INT:    return bool(ivalue); break;
        case JSON_FLOAT:  return bool(int(xvalue)); break;
        case JSON_STRING: return (svalue == "true"); break;
        case JSON_ARRAY:  return avalue[0].asBool(); break;
        case JSON_OBJECT: return false; break;
        case JSON_UNKNOWN: 
        defaut: return false; break;
        }
    }
    int Value::asInt()
    {
        switch (type) {
        case JSON_BOOL:   return int(bvalue); break;
        case JSON_INT:    return ivalue; break;
        case JSON_FLOAT:  return int(xvalue); break;
        case JSON_STRING: return stoi(svalue); break;
        case JSON_ARRAY:  return avalue[0].asInt(); break;
        case JSON_OBJECT: return 0; break;
        case JSON_UNKNOWN: 
        defaut: return 0; break;
        }
    }
    double Value::asFloat()
    {
        switch (type) {
        case JSON_BOOL:   return float(int(bvalue)); break;
        case JSON_INT:    return float(ivalue); break;
        case JSON_FLOAT:  return xvalue; break;
        case JSON_STRING: return stod(svalue); break;
        case JSON_ARRAY:  return avalue[0].asFloat(); break;
        case JSON_OBJECT: return 0.0; break;
        case JSON_UNKNOWN: 
        defaut: return 0.; break;
        }
    }
    std::string Value::asString()
    {
        switch (type) {
        case JSON_BOOL:   return (bvalue ? "true" : "false"); break;
        case JSON_INT:    return std::to_string(ivalue); break;
        case JSON_FLOAT:  return std::to_string(xvalue); break;
        case JSON_STRING: return svalue; break;
        case JSON_ARRAY:  return avalue[0].asString(); break;
        case JSON_OBJECT: return std::string(""); break;
        case JSON_UNKNOWN: 
        defaut: return std::string(""); break;
        }
    }
    Array Value::asArray()
    {
        switch (type) {
        case JSON_BOOL:   {
            std::vector<bool> vb {bvalue};
            return Array(vb); } break;
        case JSON_INT:    {
            std::vector<int> vi {ivalue};
            return Array(vi); } break;
        case JSON_FLOAT:  {
            std::vector<double> vx {xvalue};
            return Array(vx); } break;
        case JSON_STRING: {
            std::vector<std::string> vs {svalue};
            return Array(vs); } break;
        case JSON_ARRAY: 
            return avalue; break;
        case JSON_OBJECT: {
            std::vector<Object> vo {ovalue};
            return Array(vo); } break;
        case JSON_UNKNOWN: 
        defaut:
            return Array(); break;
        }
    }
    Object Value::asObject()
    {
        switch (type) {
        case JSON_BOOL:   return Object("value", bvalue); break;
        case JSON_INT:    return Object("value", ivalue); break;
        case JSON_FLOAT:  return Object("value", xvalue); break;
        case JSON_STRING: return Object("value", svalue); break;
        case JSON_ARRAY:  return Object("value", avalue); break;
        case JSON_OBJECT: return ovalue; break;
        case JSON_UNKNOWN: 
        defaut: return Object(); break;
        }
    }
        
    Value & Value::operator[](int i)
    {
        return avalue[i];
    }

            
    Value & Value::operator[](const char * s)
    {
        std::string ss(s);
        return ovalue[ss];
    }

    Value & Value::operator[](std::string s)
    {
        return ovalue[s];
    }

    //======================================================================
    
    Parser::Parser() {}
    
    Parser::~Parser() {}

    bool Parser::parseFile(std::string filename, Object & root)
    {
        std::ifstream ifs(filename);
        std::stringstream buffer;
        buffer << ifs.rdbuf();
        ifs.close();
        std::string s = buffer.str();
        buffer.str("");
    
        return parse(s, root);
    }

    bool Parser::parse(std::string & s, Object & root)
    {
        Object * o = nullptr;
        Array * a;

        std::string item;
        std::string key;
    
        std::stack<Object*> ostck;
        std::stack<Array*> astck;
        std::stack<std::string> kstck;

        enum State { OUT, IN_STRING, IN_ARRAY, IN_OBJECT } state = OUT;
        const char * StateName[] = {"OUT", "INSTR", "INARR", "INOBJ"};
        std::stack<State> states;
    
        bool escaping = false;
        bool hexNumber = false;
        int hexDigits = 0;
        bool hasKey = false;
    
        bool parseResult = true;
        int k = 0;
        int sLen = s.size();
    
        do {
            char & c = s.at(k);
        
            //std::cerr << c << ' '
            //        << TokenName[c] << ' '
            //        << StateName[state] << '\n';
        
            switch (c) {
            case OPEN_BRACE:
                if (state != IN_STRING) {
                    if (hasKey) {
                        kstck.push(key);
                        hasKey = false;
                    }
                    states.push(state);
                    if (state == IN_OBJECT) { ostck.push(o); }
                    if (o == nullptr) {
                        o = &root;
                    } else {
                        o = new Object;
                    }
                    state = IN_OBJECT;
                } else {
                    item += c;
                }
                break;
            case CLOSE_BRACE:
                if (state != IN_STRING) {
                    state = states.top();
                    states.pop();
                    if (state == IN_OBJECT) {
                        Object * oo = ostck.top();
                        ostck.pop();
                        key = kstck.top();
                        kstck.pop();
                        oo->append(key, *o);
                        o = oo;
                    } else if (state == IN_ARRAY) {
                        a->append(*o);
                    } else {
                        item += c;
                    }
                } else {
                    item += c;
                }
                break;
            case OPEN_BRACKET:
                if (state != IN_STRING) {
                    if (hasKey) {
                        kstck.push(key);
                        hasKey = false;
                    }
                    states.push(state);
                    if (state == IN_ARRAY) { astck.push(a); }
                    a = new Array;
                    state = IN_ARRAY;
                } else {
                    item += c;
                }
                break;
            case CLOSE_BRACKET:
                if (state != IN_STRING) {
                    state = states.top();
                    states.pop();
                    if (state == IN_OBJECT) {
                        key = kstck.top();
                        kstck.pop();
                        o->append(key, *a);
                    } else if (state == IN_ARRAY) {
                        Array * aa = astck.top();
                        astck.pop();
                        aa->append(*a);
                        a = aa;
                    } else {
                        item += c;
                    }
                } else {
                    item += c;
                }
                break;
            case QUOTES:
                if (state == IN_STRING) {
                    if (escaping) {
                        // Escaped quotes
                        item += c;
                    } else {
                        // Closing quotes
                        state = states.top();
                        states.pop();
                        if (state == IN_ARRAY) {
                            a->append(item);
                        } else if (state == IN_OBJECT) {
                            if (hasKey) {
                                //std::cerr << "Appending (\"" << key << "\": \"" << item << "\"\n";
                                o->append(key, item);
                                hasKey = false;
                            } else {
                                key = item;
                                hasKey = true;
                            }
                        } else {
                            parseResult = false; 		std::cerr << "char is '" << c << "'\n";
                    
                        }
                    }
                } else {
                    // Starting quotes
                    states.push(state);
                    state = IN_STRING;
                    item.clear();
                }
                break;
            case COLON:
                if (state == IN_STRING) {
                    item += c;
                } else if (state == IN_ARRAY) {
                    parseResult = false; 		std::cerr << "char is '" << c << "'\n";

                }
                break;
            case COMMA:
                if (state == IN_STRING) {
                    item += c;
                }
                break;
            default:            
                if (state == IN_STRING) {
                    if (escaping) {
                        if ((c == BACKSLASH) ||
                            (c == SLASH) ||
                            (c == EOL_CHAR) ||
                            (c == CR_CHAR) ||
                            (c == BACKSPACE) ||
                            (c == FORMFEED) ||
                            (c == TAB_CHAR)) {
                            item += BACKSLASH + c;
                            escaping = false;
                        } else {
                            if (c == UNICODE_CHAR) {
                                // must four hexadecimal digits
                                hexNumber = true;
                                hexDigits = 0;
                            } else {
                                item += c;
                            }
                        }
                    } else {
                        item += c;
                    }
                } else {
		    if ((c == COMMENT_HASH) ||
			(((c == COMMENT_SLASH) || (c == COMMENT_DASH)) &&
			 (c == s.at(k+1)))) {
			// Comment (//, or -- or #)
			size_t kk = s.find(NEWLINE, k);
			k += (kk - k + 1); // Ignore until new line
			break;
		    }

		    if ((c == SPACE) ||
                        (c == TABULATOR) ||
                        (c == NEWLINE)) { break; }
                
                    if (((DIGIT_ZERO <= c) && (c <= DIGIT_NINE)) ||
                        (c == SIGN_MINUS) || (c == SIGN_PLUS) ||
                        (c == SEP_DOT)) {
                        // Must read number
                        size_t kk = s.find_first_not_of("+-0123456789.eE", k);
                        Value v;
                        if (s.find(SEP_DOT, k) >= kk) {
                            // integer
                            v.assign(std::stoi(s.substr(k, kk - k)));
                        } else {
                            // floating point (double)
                            v.assign(std::stod(s.substr(k, kk - k)));
                        }
                        k += (kk - k - 1);
                        if (state == IN_ARRAY) {
                            a->append(v);
                        } else {
                            if (hasKey) {
                                o->append(key, v);
                                hasKey = false;
                                key = "";
                            } else {
                                parseResult = false; 		std::cerr << "char is '" << c << "'\n";

                            }

                        }
                    } else {
                        parseResult = false; 		std::cerr << "char is '" << c << "'\n";

                    }
                }
                break;
            }

            if (!parseResult) {
		std::cerr << k << " - B R E A K !!\n"; break;
	    }

            ++k;
        } while (k < sLen);

        return parseResult;
    }

    std::ostream & operator<<(std::ostream & os, const Object & o)
    {
        if (json::formattedOutput && (json::indent < json::formattedMaxLevel)) {
            os << "{\n";
            ++json::indent;
            std::string oldIndentStr = json::indentStr;
            json::indentStr += json::indentStep;
            int n = o.keys.size();
            int i = 0;
            for (auto const & key : o.keys) {
                auto it = o.obj.find(key);
                os << json::indentStr
                   << "\"" << key << "\": " << it->second 
                   <<  ((i < n - 1) ? ",\n" : "\n");
                ++i;
            }
            json::indentStr = oldIndentStr;
            os << json::indentStr << "}";
            --json::indent;
        } else {
            os << "{";
            int n = o.keys.size();
            int i = 0;
            for (auto const & key : o.keys) {
                auto it = o.obj.find(key);
                os << "\"" << key << "\": " << it->second 
                   <<  ((i < n - 1) ? ", " : "");
                ++i;
            }
            os << "}";
        }
        return os;
    }

    std::ostream & operator<<(std::ostream & os, const Array & a)
    {
        if (json::formattedOutput && (json::indent < json::formattedMaxLevel)) {
            os << "[\n";
            ++json::indent;
            std::string oldIndentStr = json::indentStr;
            json::indentStr += json::indentStep;
            int n = a.arr.size();
            for (int i = 0; i < n; ++i) {
                os << json::indentStr
                   << a.arr.at(i) << ((i < n - 1) ? ",\n" : "\n");
            }
            json::indentStr = oldIndentStr;
            os << json::indentStr << "]";
            --json::indent;
        } else {
            os << "[";
            int n = a.arr.size();
            for (int i = 0; i < n; ++i) {
                os << a.arr.at(i) << ((i < n - 1) ? ", " : "");
            }
            os << "]";
        }
        return os;
    }

    std::ostream & operator<<(std::ostream & os, const Value & v)
    {
        if (json::formattedOutput && (json::indent < json::formattedMaxLevel)) {
            switch (v.type) {
            case JSON_BOOL:   os << (v.bvalue ? "true" : "false");  break;
            case JSON_INT:    os << v.ivalue;  break;
            case JSON_FLOAT:
                if (floatPrecision > 0) { os << std::setprecision(floatPrecision); }
                os << ((floatFmt == SCI) ? std::scientific :
                       (floatFmt == FIX) ? std::fixed : std::defaultfloat)
                   << v.xvalue;  break;
            case JSON_STRING: os << "\"" << v.svalue << "\"";  break;
            case JSON_ARRAY:  os << v.avalue;  break;
            case JSON_OBJECT: os << v.ovalue;  break;
            default: break;
            }
        } else {
            switch (v.type) {
            case JSON_BOOL:   os << (v.bvalue ? "true" : "false");  break;
            case JSON_INT:    os << v.ivalue;  break;
            case JSON_FLOAT:  os << v.xvalue;  break;
            case JSON_STRING: os << "\"" << v.svalue << "\"";  break;
            case JSON_ARRAY:  os << v.avalue;  break;
            case JSON_OBJECT: os << v.ovalue;  break;
            default: break;
            }
        }
        return os;
    }

    void setFloatFormat(FloatFormat fmt, int prec)
    {
        json::floatFmt = fmt;
        json::floatPrecision = prec;
    }

    void enableFormattedOutput(std::string tab, int maxlvl)
    {
        json::formattedOutput   = true;
        json::indentStep        = tab;
        json::formattedMaxLevel = maxlvl;
    }

    void disableFormattedOutput()
    {
        json::formattedOutput = false;
    }

}

