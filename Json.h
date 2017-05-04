#ifndef CPP_JSON_JSON_H
#define CPP_JSON_JSON_H

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <fstream>

enum JsonType {
    JSON_FALSE, JSON_TRUE, JSON_NULL, JSON_NUMBER, JSON_STRING, JSON_ARRAY, JSON_OBJECT
};

class Json_Value;

// to show json null
class Null_Object {
public:
    bool operator==(const Null_Object& noob) { return true;}
    bool operator<(const Null_Object& noob) { return false;}
};


class Json {
public:
    using json_object = std::unordered_map<std::string, std::shared_ptr<Json_Value>>;
    using json_array = std::vector<std::shared_ptr<Json_Value>>;
    using json_null = Null_Object;
    // TODO:constructor
    Json();         // default, create an empty object
    Json(const std::string& text);
    friend Json createJsonWithFile(const std::string& filename);
    // 内部实现
//    template <typename T>
//    Json(const T& type, bool is_unit) {
//        if (is_unit)        // 如果是小单元类型
//            ptr_data = std::make_shared<Json_Value>(type);
//    }       // 以类型构造

    // TODO:output
    friend std::ostream& operator<<(std::ostream&, const Json& json);
    // TODO:visit
    Json_Value&operator[](const std::string& attr_name);
    const Json_Value &operator[](const std::string& attr_name) const;
    // TODO: update
    std::shared_ptr<Json> append(const Json& json);
private:
    std::shared_ptr<Json_Value> ptr_data;
};
Json createJsonWithFile(const std::string& filename);

class Json_Value {
public:
    JsonType value_type;
    virtual JsonType get_type() const { return value_type;}
    const std::string get_rep() { return _rep;}
    virtual void set_rep() {}               // do nothing for some types
//    Json::json_object symbol_table;
//    double number;
//    Json::json_null null_ob;
//    std::string json_string;
//    Json::json_array arr;
//    bool boolean;
protected:
    std::string _rep;
};

template <typename T, JsonType basic_type>
class Value: public Json_Value {
public:
    Value(const T & dt): json_data(dt) {}
    Value() = default;
    T json_data;
    JsonType value_type = basic_type;
    JsonType get_type() const { return basic_type;}
};

class ValueNumber final:public Value<double, JsonType::JSON_NUMBER> {
public:
    ValueNumber(double val): Value(val) {
        set_rep();
    }
    void set_rep() override  {
        _rep = std::to_string(json_data);
    }
};

class ValueNull final:public Value<Json::json_null, JsonType::JSON_NULL> {
public:
    ValueNull() {
        _rep = "null";
    }
};

class ValueTrue final: public Value<bool, JsonType::JSON_TRUE> {
public:
    ValueTrue() {
        _rep = "true";
    }
};

class ValueFalse final: public Value<bool, JsonType::JSON_FALSE> {
public:
    ValueFalse() {
        _rep = "false";
    }
};

class ValueString final: public Value<std::string, JsonType::JSON_STRING> {
public:
    ValueString(const std::string &s): Value(s) {
        set_rep();
    }
    void set_rep() override  {
        _rep = "\"" + json_data + "\"";
    }
};

class ValueArray final: public Value<Json::json_array, JsonType::JSON_ARRAY> {
public:
    ValueArray() {
        set_rep();
    }
    ValueArray(const Json::json_array& json_array): Value(json_array) {
        set_rep();
    }
    void set_rep() override {
        _rep = "[ ";
        for (auto &ch: json_data) {
            _rep.append(ch->get_rep());
            _rep.append(", ");
        }
        _rep.append("]");
//        _rep.push_back(']');
    }
    void append(std::shared_ptr<Json_Value> js) {
        json_data.push_back(js);
        set_rep();
    }
    void append(const Json_Value& js) {
        std::shared_ptr<Json_Value> jnew(new Json_Value(js));
        json_data.push_back(jnew);
        set_rep();
    }
};

class ValueObject final: public Value<Json::json_object, JsonType::JSON_OBJECT> {
public:
    std::shared_ptr<Json_Value>& operator[](const std::string &attr) {
        auto iter = json_data.find(attr);
        if (iter == json_data.end()) {
            std::cerr << "can't find" << std::endl;
            exit(1);
        } else
            return json_data[attr];
    }
    void append(const std::string&s, const std::shared_ptr<Json_Value> &ptr_json) {
        json_data[s] = ptr_json;
        set_rep();
    }
    ValueObject() {
        set_rep();
    }
    ValueObject(const Json::json_object& json_object): Value(json_object) {
        set_rep();
    }
    void set_rep() override {
        _rep = "{ ";
        for (auto &ch: json_data) {
            _rep.append("\"" + ch.first + "\"");
            _rep.append(":");
            _rep.append(ch.second->get_rep());
            _rep.append(", ");

        }
//        _rep.push_back('}');
        _rep.append("}");
    }
};


#endif //CPP_JSON_JSON_H
