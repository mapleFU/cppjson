#ifndef CPP_JSON_JSONPARSER_H
#define CPP_JSON_JSONPARSER_H

#include "Json.h"

class JsonParser {

public:
    enum class token {
        LMPAR, RMPAR,LLPAR, RLPAR,STR, NAME, DIGIT, NONE, COLON, COMMA, QUO, TRUE, FALSE, MINUS
    };
    JsonParser() = delete;
    JsonParser(const std::string & text): _to_interpret(text) {}
    std::shared_ptr<Json_Value> parse();
private:
    // data
    mutable int index = 0;
    const std::string& _to_interpret;
    std::shared_ptr<ValueObject> json_data;
    token current = token::NONE;

    // operations
    void skip_whitespace() const {
        while (_to_interpret.length() > index && (_to_interpret[index] == ' ' || _to_interpret[index] == '\n' || _to_interpret[index] == '\t'))
            ++index;
    }



private:
    std::string get_name();
    std::shared_ptr<Json_Value> get_null();
    std::shared_ptr<Json_Value> get_boolean();
    inline std::shared_ptr<Json_Value> get_string();
    std::shared_ptr<Json_Value> get_digit();
    std::shared_ptr<Json_Value> get_object();
    std::shared_ptr<Json_Value> get_value();
    std::shared_ptr<Json_Value> get_array();
    // 吞字符
    void get_next_token();
    int eat(token typeT);
};



#endif //CPP_JSON_JSONPARSER_H
