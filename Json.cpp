#include "Json.h"
#include "JsonParser.h"
#include <fstream>
#include <sstream>

Json::Json() {
    // an empty object
    ptr_data = std::make_shared<ValueObject>();
}

Json::Json(const std::string &text) {
    // parse the text
    auto parse = JsonParser(text);
    ptr_data = parse.parse();
}

Json createJsonWithFile(const std::string& filename) {
    std::ifstream is(filename);
    std::string s, tmp;
    Json json;
    while (getline(is, tmp)) {
        s.append(tmp);
    }
//    std::cout << s << std::endl;
    is.close();
    auto parse = JsonParser(s);
    json.ptr_data = parse.parse();
    return json;
}

std::ostream &operator<<(std::ostream &os, const Json &json) {
    auto s = json.ptr_data->get_rep();
    std::cout << s;
    // 傻子你忘记return os 了
    return os;
}

Json_Value& Json::operator[](const std::string &attr_name) {
    auto p = std::dynamic_pointer_cast<ValueObject>(ptr_data);
    return *(*p)[attr_name];
}

const Json_Value &Json::operator[](const std::string &attr_name) const {
    auto p = std::dynamic_pointer_cast<ValueObject>(ptr_data);
    return *(*p)[attr_name];
}

void Json::append(const Json &json) {
    if (this->ptr_data->get_type() == JSON_ARRAY) {
        auto arr_ptr = std::dynamic_pointer_cast<ValueArray>(ptr_data);
        arr_ptr->append(json.ptr_data);
    } else {
        std::cout << "Error, this json object is not a json array" << std::endl;
        exit(1);
    }
}

void Json::append_to_object(const std::string &key, const Json &value) {
    if (this->ptr_data->get_type() == JSON_OBJECT) {
        auto obj_ptr = std::dynamic_pointer_cast<ValueObject>(ptr_data);
        obj_ptr->json_data[key] = value.ptr_data;
    } else {
        std::cout << "Error, this json object is not a json object" << std::endl;
        exit(1);
    }
}




