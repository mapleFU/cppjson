#include "JsonParser.h"
#include <set>
#include <cctype>

/*
 * object: { + key(str) : value + }
 * array : [ + value(,value)* + ]
 * val : null, false, true, digit
 * name: str or str without ""
 *
 */
std::shared_ptr<Json_Value> JsonParser::parse() {
    skip_whitespace();
    if (_to_interpret[index] == '{')
        return get_object();
    else {
        std::cerr << "error!" << std::endl;
        return nullptr;
    }
}

std::shared_ptr<Json_Value> JsonParser::get_object() {
    eat(token::LLPAR);
    get_next_token();
    std::string name;
    auto pthis = std::make_shared<ValueObject>();
    while (current != token::RLPAR) {
        name = get_name();
        get_next_token();
        eat(token::COLON);      // 吞掉colon
        get_next_token();       // next token
        std::shared_ptr<Json_Value> next;
        if (current == token::LMPAR) {      // 是'['
            next = get_array();
        } else if (current == token::LLPAR) {
            next = get_object();
        } else {
            next = get_value();
        }
//        (*pthis)[name] = next;
        pthis->append(name, next);      // 添加
        get_next_token();
        if (current == token::COMMA)
            eat(token::COMMA);
        else if (current == token::RLPAR)
            break;
        get_next_token();
    }
    eat(token::RLPAR);
    return pthis;
}

std::shared_ptr<Json_Value> JsonParser::get_value() {       // 任意get 一个json对象
    if (current == token::LLPAR)
        return get_object();
    if (current == token::LMPAR)
        return get_array();
    if (current == token::DIGIT || current == token::MINUS)
        return get_digit();
    if (current == token::STR)
        return get_string();
    if (current == token::NAME) {
        switch (_to_interpret[index]) {
            case 'f':
            case 't':
                return get_boolean();
            case 'n':
                return get_null();
            default:
                break;
        }
    }
    else        // 啥都没有找到
        return nullptr;
}

inline bool _in_split(const char *split, int n, char ch) {
    for (int i = 0; i < n; ++i) {
        if (split[i] == ch)
            return false;
    }
    return true;
}

std::shared_ptr<Json_Value> JsonParser::get_string() {
    return std::make_shared<ValueString>(get_name());
}

std::string JsonParser::get_name() {
//    const static char split[] = {'\"'};
    std::string to_get;
    std::string tmp;
    int tmp_index(index);
    if (_to_interpret[index] == '\"') ++index;
    while (1) {
        if (_to_interpret[index] != '\"') {
            to_get.push_back(_to_interpret[index]);
            ++index;
        }
        else {
            tmp_index = index;
            ++index;
            get_next_token();
            // 如果下一个token标志着结束，停止读取，token返回
            if (current == token::COMMA || current == token::RLPAR || current == token::RMPAR || current == token::COLON) {
                index = tmp_index;
                current = token ::STR;
                break;
            } else {
                to_get.push_back(_to_interpret[tmp_index]);
                index = tmp_index + 1;
            }
        }
    }
    if (_to_interpret[index] == '\"') ++index;
    skip_whitespace();
    return to_get;
}

std::shared_ptr<Json_Value> JsonParser::get_null() {
    std::string s;
    for (int i = 0; i < 4; ++i) {
        s.push_back(_to_interpret[index + i]);
    }
    if (s == "null") {
        index += 4;
        skip_whitespace();
        return std::make_shared<ValueNull>();
    }
    else {
        std::cerr << "null wrong! do you mean " << s << "is" << "null?" << std::endl;
        return nullptr;
    }
}



std::shared_ptr<Json_Value> JsonParser::get_array() {
    eat(token::LMPAR);
    get_next_token();
    std::string name;
    // TODO: 傻逼你要知道不能用std::shared_ptr否则你会得到一个nullptr
    auto pthis = std::make_shared<ValueArray>();
    while (current != token::RMPAR) {
        std::shared_ptr<Json_Value> next;
        if (current == token::LMPAR) {      // 是'['
            next = get_array();
        } else if (current == token::LLPAR) {
            next = get_object();
        } else {
            next = get_value();
        }
        pthis->append(next);
        get_next_token();
        if (current == token::COMMA)
            eat(token::COMMA);
        else if (current == token::LMPAR)
            break;
        get_next_token();
    }
    eat(token::RMPAR);
    return pthis;
}

std::shared_ptr<Json_Value> JsonParser::get_boolean() {
    if (_to_interpret[index] == 't') {
        eat(token::TRUE);
        return std::make_shared<ValueTrue>();
    } else {        // false
        eat(token::FALSE);
        return std::make_shared<ValueFalse>();
    }
}

std::shared_ptr<Json_Value> JsonParser::get_digit() {
    int symbol(1);
    if (current == token::MINUS) {
        symbol = -1;
        ++index;
        if (isdigit(_to_interpret[index])) {
            current = token::DIGIT;
        } else {
            std::cerr << "digit error, do you min a nagative number?" << std::endl;
            exit(1);
        }
    }
    double digit(0);
    // int digit
    while (isdigit(_to_interpret[index])) {
        digit *= 10;
        digit += _to_interpret[index] - '0';
        ++index;
    }
    // real digit
    if (_to_interpret[index] == '.') {
        ++index;
        int cnt(0);
        while (isdigit(_to_interpret[index])) {
            digit *= 10;
            digit += _to_interpret[index] - '0';
            ++index;
            ++cnt;
        }
        digit /= pow(10, cnt);
    }
    return std::make_shared<ValueNumber>(symbol * digit);
}

void JsonParser::get_next_token() {
    skip_whitespace();
    char ch = _to_interpret[index];
    if (ch == ':') {
        current = token::COLON;
    } else if (ch == '\"') {
        current = token::STR;
    } else if (ch == '{') {
        current = token::LLPAR;
    } else if (ch == '[') {
        current = token::LMPAR;
    } else if (ch == '}') {
        current = token::RLPAR;
    } else if (ch == ']') {
        current = token::RMPAR;
    } else if (isdigit(ch)) {
        current = token::DIGIT;
    } else if (isalpha(ch)) {
        // TODO:what's this?
        current = token::NAME;
    } else if (ch == ',') {
        current = token::COMMA;
    } else if (ch == '-') {     // 负号
        current = token::MINUS;
    }
}

int JsonParser::eat(JsonParser::token typeT) {
    // 对字长不为1的编写特别版本
    int eaten(0);           // 吞掉的字符
    // eat的可能以此终结
    const static char split[] = {' ', ',', '\n', ':', '}', ']'};
    skip_whitespace();
    switch (typeT) {
        case token::NONE:
            skip_whitespace();
            break;
        case token::STR:
            printf("xxd\n");
        case token::TRUE:
        case token::FALSE:
        case token::DIGIT:
        case token::MINUS:
            while (_in_split(split, 6, _to_interpret[index])) ++index, ++eaten;
            break;
        default:
            ++index;
            ++eaten;
            skip_whitespace();
    }
    return eaten;
}