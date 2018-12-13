#include "phpvalue.hpp"

PhpValue::PhpValue() : php_constructed(true), v() {}
PhpValue::PhpValue(Value v) : php_constructed(false), v(v) {}
PhpValue::PhpValue(std::string n) : php_constructed(true), v(t_string, n) {}

void PhpValue::__construct(Php::Parameters &parameters)
{
    if (parameters.size() == 0)
    {
        v = Value();
    }
    else if (parameters.size() == 1)
    {
        std::string value = parameters[0];
        v = Value(t_string, value);
    }
    else if (parameters.size() == 2)
    {
        if (parameters[0].isNumeric())
        {
            std::string value = parameters[1];
            v = Value((ValueType)((int)parameters[0]), value);
        }
        else
        {
            std::string name = parameters[0];
            std::string value = parameters[1];
            v = Value(name, value);
        }
    }
    else
    {
        throw Php::Exception("Invalid value constructed, number of parameters should be 0, 1 or 2.");
    }
}

std::string PhpValue::__toString()
{
    return v.GetContent();
}

Php::Value PhpValue::__get(const Php::Value &name)
{
    if (name == "type" || name == "t")
        return v.GetType();
    if (name == "content" || name == "c")
        return v.GetContent();
    throw Php::Exception("unknown property");
}

void PhpValue::__set(const Php::Value &name, const Php::Value &value)
{
    throw Php::Exception("MarTeX Value type is immutable.");
}

Value PhpValue::val()
{
    return v;
}

PhpArray::PhpArray() : php_constructed(true), content() {}

PhpArray::PhpArray(std::vector<Value> vs) : php_constructed(false)
{
    Php::Array a;
    int index = 0;
    for (Value v : vs)
        a[index++] = cast_to_php(v);
    content = a;
}

PhpArray::PhpArray(Value v) : PhpArray(v.GetValues()) {}

void PhpArray::setVals(std::vector<Php::Value> params)
{
    Php::Array a;
    int index = 0;
    for (Php::Value v : params)
    {
        if (v.isString())
            a[index++] = Php::Object("\\MarTeX\\Value", new PhpValue(v));
        else if (v.isObject() && (v.instanceOf("\\MarTeX\\Value") || v.instanceOf("\\MarTeX\\Html")))
            a[index++] = v;
        else
            throw Php::Exception("Batch should contain just Values, Htmls or Strings.");
    }
    content = a;
}

void PhpArray::__construct(Php::Parameters &params)
{
    setVals(std::vector<Php::Value>(params.begin(), params.end()));
}

std::string PhpArray::__toString()
{
    std::string ret = "";
    for (auto vp : content)
    {
        Php::Value v = vp.second;
        if (v.instanceOf("\\MarTeX\\Value"))
        {
            PhpValue *_v = (PhpValue *)v.implementation();
            ret += _v->__toString();
        }
        else
        {
            PhpHtml *_v = (PhpHtml *)v.implementation();
            ret += _v->__toString();
        }
    }
    return ret;
}

Php::Value PhpArray::__get(const Php::Value &name)
{
    if (name == "type" || name == "t")
        return t_multi;
    if (name == "content" || name == "c")
        return this;
    throw Php::Exception("unknown property");
}

void PhpArray::__set(const Php::Value &name, const Php::Value &v)
{
    throw Php::Exception("MarTeX Array properties cannot be set. You can only push/insert or remove elements.");
}

long PhpArray::count()
{
    return content.size();
}

bool PhpArray::offsetExists(const Php::Value &key)
{
    return (key.isNumeric() && (int)key >= 0 && (int)key < content.size());
}

void PhpArray::offsetSet(const Php::Value &k, const Php::Value &v)
{
    if (!k.isNumeric() || (int)k < 0 || (int)k > content.size())
        throw Php::Exception("Array should grow by 1 at a time.");

    if (v.isString())
        content[(int)k] = Php::Object("\\MarTeX\\Value", new PhpValue(v));
    else if (v.isObject() && (v.instanceOf("\\MarTeX\\Value") || v.instanceOf("\\MarTeX\\Html")))
        content[(int)k] = v;
    else
        throw Php::Exception("Batch should contain just Values, Htmls or Strings.");
}

Php::Value PhpArray::offsetGet(const Php::Value &key)
{
    return content[key];
}

void PhpArray::offsetUnset(const Php::Value &key)
{
    content.unset(key);
}

Value PhpArray::val()
{
    std::vector<Value> vs;
    vs.reserve(content.size());

    for (auto vp : content)
    {
        Php::Value v = vp.second;
        if (v.instanceOf("\\MarTeX\\Value"))
        {
            PhpValue *_v = (PhpValue *)v.implementation();
            vs.push_back(_v->val());
        }
        else
        {
            PhpHtml *_v = (PhpHtml *)v.implementation();
            vs.push_back(_v->val());
        }
    }

    return Value(vs);
}

PhpHtml::PhpHtml() : php_constructed(true), tag("")
{
    content = new PhpArray();
    arrayobj = Php::Object("\\MarTeX\\Batch", content);
}

PhpHtml::PhpHtml(Value v) : php_constructed(false), tag(v.GetTag())
{
    content = new PhpArray(v.GetValues());
    arrayobj = Php::Object("\\MarTeX\\Batch", content);
}

PhpHtml::PhpHtml(std::string tag, std::vector<Php::Value> vp) : php_constructed(true), tag(tag)
{
    content = new PhpArray();
    content->setVals(vp);
    arrayobj = Php::Object("\\MarTeX\\Batch", content);
}

void PhpHtml::__construct(Php::Parameters &params)
{
    std::string temp = params[0];
    tag = temp;

    if (params.size() == 1)
    {
    }
    else if (params.size() == 2 && params[1].instanceOf("\\MarTeX\\Batch"))
    {
        content = ((PhpArray *)params[1].implementation());
        arrayobj = Php::Object("\\MarTeX\\Batch", content);
    }
    else if (params.size() == 2 && params[1].isArray())
    {
        std::vector<Php::Value> vs;

        for (Php::Value v : vs)
            vs.push_back(v);

        content->setVals(vs);
    }
    else
    {
        content->setVals(std::vector<Php::Value>(params.begin() + 1, params.end()));
    }
}

std::string PhpHtml::__toString()
{
    /// Don't replicate
    return val().GetContent();
}

Php::Value PhpHtml::__get(const Php::Value &name)
{
    if (name == "type" || name == "t")
        return t_html;
    if (name == "content" || name == "c")
        return arrayobj;
    if (name == "tag")
        return tag;
    throw Php::Exception("unknown property");
}

void PhpHtml::__set(const Php::Value &name, const Php::Value &v)
{
    if (name == "tag" || name == "t")
    {
        std::string temp = v;
        tag = temp;
    }
    else if ((name == "content" || name == "c") && v.instanceOf("\\MarTeX\\Batch"))
    {
        content = ((PhpArray *)v.implementation());
        arrayobj = Php::Object("\\MarTeX\\Batch", content);
    }
    else
    {
        throw Php::Exception("MarTeX html can only set content or tag.");
    }
}

Value PhpHtml::val()
{
    return Value(tag, content->val().GetValues());
}

Php::Value cast_to_php(Value v)
{
    switch (v.GetType())
    {
    case t_ampersand:
    case t_break:
    case t_string:
    case t_null:
    case t_attr:
        return Php::Object("\\MarTeX\\Value", new PhpValue(v));
    case t_multi:
        return Php::Object("\\MarTeX\\Batch", new PhpArray(v));
    case t_html:
        return Php::Object("\\MarTeX\\Html", new PhpHtml(v));
    default:
        break;
    }
    return Php::Value(nullptr);
}

Php::Value html(Php::Parameters &params)
{
    PhpHtml *v = new PhpHtml();
    v->__construct(params);
    return Php::Object("\\MarTeX\\Html", v);
}

Php::Value value(Php::Parameters &params)
{
    PhpValue *v = new PhpValue();
    v->__construct(params);
    return Php::Object("\\MarTeX\\Value", v);
}

Php::Value batch(Php::Parameters &params)
{
    PhpArray *v = new PhpArray();
    v->__construct(params);
    return Php::Object("\\MarTeX\\Batch", v);
}

Php::Value ampersand(Php::Parameters &params)
{
    return Php::Object("\\MarTeX\\Value", new PhpValue(Value(t_ampersand, "&amp;")));
}

Php::Value newline(Php::Parameters &params)
{
    return Php::Object("\\MarTeX\\Value", new PhpValue(Value(t_break, "<br />")));
}