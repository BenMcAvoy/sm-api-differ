#include <print>
#include <fstream>
#include <filesystem>
#include <optional>
#include <map>
#include <nlohmann/json.hpp>

namespace docs
{
    struct klass
    {
        std::string name;

        struct doc_t
        {
            std::vector<std::string> text;

            // TODO: proper handling?
            std::vector<std::vector<std::string>> fields;

            NLOHMANN_DEFINE_TYPE_INTRUSIVE(doc_t, text, fields)
        } doc;

        struct constant_t
        {
            std::string name;

            struct doc_t
            {
                std::vector<std::string> text;

                // TODO: proper handling?
                std::vector<std::vector<std::string>> returns;

                NLOHMANN_DEFINE_TYPE_INTRUSIVE(doc_t, text, returns)
            } doc;

            NLOHMANN_DEFINE_TYPE_INTRUSIVE(constant_t, name, doc)
        };
        std::vector<constant_t> constants;

        struct common_callback
        {
            std::string name;

            struct doc_t
            {
                std::vector<std::string> text;
                std::vector<std::vector<std::string>> params;
                std::string type;

                NLOHMANN_DEFINE_TYPE_INTRUSIVE(doc_t, text, params, type)
            } doc;

            NLOHMANN_DEFINE_TYPE_INTRUSIVE(common_callback, name, doc)
        };
        std::vector<common_callback> common_callbacks;

        struct callback
        {
            std::string name;

            struct doc_t
            {
                std::vector<std::string> text;
                std::vector<std::vector<std::string>> params;
                std::string type;

                NLOHMANN_DEFINE_TYPE_INTRUSIVE(doc_t, text, params, type)
            } doc;

            NLOHMANN_DEFINE_TYPE_INTRUSIVE(callback, name, doc)
        };
        std::vector<callback> callbacks;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(klass, name, doc, constants, common_callbacks, callbacks)
    };

    struct userdata
    {
        std::string name;
        std::string namespace_association;
        std::string usage;
        bool serializable = false;

        struct doc_t
        {
            std::vector<std::string> text;

            NLOHMANN_DEFINE_TYPE_INTRUSIVE(doc_t, text)
        } doc;

        struct accessor_t
        {
            struct doc_t
            {
                std::vector<std::string> text;
                std::vector<std::vector<std::string>> params;
                std::vector<std::vector<std::string>> returns;

                NLOHMANN_DEFINE_TYPE_INTRUSIVE(doc_t, text, params, returns)
            } doc;

            NLOHMANN_DEFINE_TYPE_INTRUSIVE(accessor_t, doc)
        };

        struct member_t
        {
            std::string name;
            std::optional<accessor_t> get;
            std::optional<accessor_t> set;

            NLOHMANN_DEFINE_TYPE_INTRUSIVE(member_t, name, get, set)
        };
        std::vector<member_t> members;

        struct function_t
        {
            std::string name;

            struct doc_t
            {
                std::vector<std::string> text;
                std::vector<std::vector<std::string>> params;
                std::vector<std::vector<std::string>> returns;

                NLOHMANN_DEFINE_TYPE_INTRUSIVE(doc_t, text, params, returns)
            } doc;

            NLOHMANN_DEFINE_TYPE_INTRUSIVE(function_t, name, doc)
        };
        std::vector<function_t> functions;

        struct metamethod_t
        {
            std::string name;

            struct doc_t
            {
                std::vector<std::vector<std::string>> meta;

                NLOHMANN_DEFINE_TYPE_INTRUSIVE(doc_t, meta)
            } doc;

            NLOHMANN_DEFINE_TYPE_INTRUSIVE(metamethod_t, name, doc)
        };
        std::vector<metamethod_t> metamethods;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(userdata, name, namespace_association, usage, serializable, doc, members, functions, metamethods)
    };

    struct namespaces
    {
        std::string name;
        std::string userdata_association;

        struct doc_t
        {
            std::vector<std::string> text;

            NLOHMANN_DEFINE_TYPE_INTRUSIVE(doc_t, text)
        } doc;

        struct constant_t
        {
            std::string name;

            struct doc_t
            {
                std::vector<std::string> text;
                std::vector<std::vector<std::string>> returns;

                NLOHMANN_DEFINE_TYPE_INTRUSIVE(doc_t, text, returns)
            } doc;

            NLOHMANN_DEFINE_TYPE_INTRUSIVE(constant_t, name, doc)
        };
        std::vector<constant_t> constants;

        struct function_t
        {
            std::string name;

            struct doc_t
            {
                std::vector<std::string> text;
                std::vector<std::vector<std::string>> params;
                std::vector<std::vector<std::string>> returns;

                NLOHMANN_DEFINE_TYPE_INTRUSIVE(doc_t, text, params, returns)
            } doc;

            NLOHMANN_DEFINE_TYPE_INTRUSIVE(function_t, name, doc)
        };
        std::vector<function_t> functions;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(namespaces, name, userdata_association, doc, constants, functions)
    };

    struct index
    {
        struct environment
        {
            std::vector<klass> classes;
            std::vector<userdata> userdatas;
            std::vector<namespaces> namespaces;

            NLOHMANN_DEFINE_TYPE_INTRUSIVE(environment, classes, userdatas, namespaces)
        };

        environment game;
        environment terrain;

        NLOHMANN_DEFINE_TYPE_INTRUSIVE(index, game, terrain)
    };
}

docs::klass load_class(const std::filesystem::path &path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::println("Failed to open file: {}", path.string());
        return {};
    }

    nlohmann::json json;
    try
    {
        file >> json;
    }
    catch (const nlohmann::json::parse_error &e)
    {
        std::println("Failed to parse JSON: {}", e.what());
        return {};
    }

    docs::klass klass;
    klass.name = json.value("class", std::string{});

    auto doc_json = json.value("doc", nlohmann::json::object());
    klass.doc.text = doc_json.value("text", std::vector<std::string>{});
    klass.doc.fields = doc_json.value("fields", std::vector<std::vector<std::string>>{});

    for (const auto &c : json.value("constants", nlohmann::json::array()))
    {
        docs::klass::constant_t constant;
        constant.name = c.value("name", std::string{});
        auto c_doc = c.value("doc", nlohmann::json::object());
        constant.doc.text = c_doc.value("text", std::vector<std::string>{});
        constant.doc.returns = c_doc.value("returns", std::vector<std::vector<std::string>>{});
        klass.constants.push_back(constant);
    }

    for (const auto &cc : json.value("common_callbacks", nlohmann::json::array()))
    {
        docs::klass::common_callback common_callback;
        common_callback.name = cc.value("name", std::string{});
        auto cc_doc = cc.value("doc", nlohmann::json::object());
        common_callback.doc.text = cc_doc.value("text", std::vector<std::string>{});
        common_callback.doc.params = cc_doc.value("params", std::vector<std::vector<std::string>>{});
        common_callback.doc.type = cc_doc.value("type", std::string{});
        klass.common_callbacks.push_back(common_callback);
    }

    for (const auto &c : json.value("callbacks", nlohmann::json::array()))
    {
        docs::klass::callback callback;
        callback.name = c.value("name", std::string{});
        auto c_doc = c.value("doc", nlohmann::json::object());
        callback.doc.text = c_doc.value("text", std::vector<std::string>{});
        callback.doc.params = c_doc.value("params", std::vector<std::vector<std::string>>{});
        callback.doc.type = c_doc.value("type", std::string{});
        klass.callbacks.push_back(callback);
    }

    return klass;
}

docs::userdata load_userdata(const std::filesystem::path &path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::println("Failed to open file: {}", path.string());
        return {};
    }

    nlohmann::json json;
    try
    {
        file >> json;
    }
    catch (const nlohmann::json::parse_error &e)
    {
        std::println("Failed to parse JSON: {}", e.what());
        return {};
    }

    docs::userdata userdata;
    userdata.name = json.value("userdata", std::string{});
    userdata.namespace_association = json.value("namespace_association", std::string{});
    userdata.usage = json.value("usage", std::string{});
    userdata.serializable = json.value("serializable", false);

    auto doc_json = json.value("doc", nlohmann::json::object());
    userdata.doc.text = doc_json.value("text", std::vector<std::string>{});

    auto load_accessor = [](const nlohmann::json &j) -> docs::userdata::accessor_t
    {
        docs::userdata::accessor_t accessor;
        auto a_doc = j.value("doc", nlohmann::json::object());
        accessor.doc.text = a_doc.value("text", std::vector<std::string>{});
        accessor.doc.params = a_doc.value("params", std::vector<std::vector<std::string>>{});
        accessor.doc.returns = a_doc.value("return", std::vector<std::vector<std::string>>{});
        return accessor;
    };

    for (const auto &m : json.value("members", nlohmann::json::array()))
    {
        docs::userdata::member_t member;
        member.name = m.value("name", std::string{});
        if (m.contains("get"))
        {
            member.get = load_accessor(m["get"]);
        }
        if (m.contains("set"))
        {
            member.set = load_accessor(m["set"]);
        }
        userdata.members.push_back(member);
    }

    for (const auto &f : json.value("functions", nlohmann::json::array()))
    {
        docs::userdata::function_t function;
        function.name = f.value("name", std::string{});
        auto f_doc = f.value("doc", nlohmann::json::object());
        function.doc.text = f_doc.value("text", std::vector<std::string>{});
        function.doc.params = f_doc.value("params", std::vector<std::vector<std::string>>{});
        function.doc.returns = f_doc.value("return", std::vector<std::vector<std::string>>{});
        userdata.functions.push_back(function);
    }

    for (const auto &mm : json.value("metamethods", nlohmann::json::array()))
    {
        docs::userdata::metamethod_t metamethod;
        metamethod.name = mm.value("name", std::string{});
        auto mm_doc = mm.value("doc", nlohmann::json::object());
        metamethod.doc.meta = mm_doc.value("meta", std::vector<std::vector<std::string>>{});
        userdata.metamethods.push_back(metamethod);
    }

    return userdata;
}

docs::namespaces load_namespace(const std::filesystem::path &path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::println("Failed to open file: {}", path.string());
        return {};
    }

    nlohmann::json json;
    try
    {
        file >> json;
    }
    catch (const nlohmann::json::parse_error &e)
    {
        std::println("Failed to parse JSON: {}", e.what());
        return {};
    }

    docs::namespaces ns;
    ns.name = json.value("namespace", std::string{});
    ns.userdata_association = json.value("userdata_association", std::string{});

    auto doc_json = json.value("doc", nlohmann::json::object());
    ns.doc.text = doc_json.value("text", std::vector<std::string>{});

    for (const auto &c : json.value("constants", nlohmann::json::array()))
    {
        docs::namespaces::constant_t constant;
        constant.name = c.value("name", std::string{});
        auto c_doc = c.value("doc", nlohmann::json::object());
        constant.doc.text = c_doc.value("text", std::vector<std::string>{});
        constant.doc.returns = c_doc.value("return", std::vector<std::vector<std::string>>{});
        ns.constants.push_back(constant);
    }

    for (const auto &f : json.value("functions", nlohmann::json::array()))
    {
        docs::namespaces::function_t function;
        function.name = f.value("name", std::string{});
        auto f_doc = f.value("doc", nlohmann::json::object());
        function.doc.text = f_doc.value("text", std::vector<std::string>{});
        function.doc.params = f_doc.value("params", std::vector<std::vector<std::string>>{});
        function.doc.returns = f_doc.value("return", std::vector<std::vector<std::string>>{});
        ns.functions.push_back(function);
    }

    return ns;
}

docs::index load_index(const std::filesystem::path &path)
{
    docs::index index;

    std::ifstream file(path);
    if (!file.is_open())
    {
        std::println("Failed to open file: {}", path.string());
        return index;
    }

    nlohmann::json json;
    try
    {
        file >> json;
    }
    catch (const nlohmann::json::parse_error &e)
    {
        std::println("Failed to parse JSON: {}", e.what());
        return index;
    }

    try
    {
        auto game = json.value("Game", nlohmann::json::object());

        auto g_classes = game.value("classes", nlohmann::json::array());
        auto g_userdatas = game.value("userdata", nlohmann::json::array());
        auto g_namespaces = game.value("namespaces", nlohmann::json::array());

        auto terrain = json.value("Terrain", nlohmann::json::object());
        auto t_classes = terrain.value("classes", nlohmann::json::array());
        auto t_userdatas = terrain.value("userdata", nlohmann::json::array());
        auto t_namespaces = terrain.value("namespaces", nlohmann::json::array());

        for (const auto &c : g_classes)
        {
            auto class_path = path.parent_path() / ("class_Game_" + c.get<std::string>() + ".json");
            index.game.classes.push_back(load_class(class_path));
        }
        for (const auto &u : g_userdatas)
        {
            auto userdata_path = path.parent_path() / ("userdata_Game_" + u.get<std::string>() + ".json");
            index.game.userdatas.push_back(load_userdata(userdata_path));
        }
        for (const auto &n : g_namespaces)
        {
            auto namespace_path = path.parent_path() / ("namespace_Game_" + n.get<std::string>() + ".json");
            index.game.namespaces.push_back(load_namespace(namespace_path));
        }

        for (const auto &c : t_classes)
        {
            auto class_path = path.parent_path() / ("class_Terrain_" + c.get<std::string>() + ".json");
            index.terrain.classes.push_back(load_class(class_path));
        }
        for (const auto &u : t_userdatas)
        {
            auto userdata_path = path.parent_path() / ("userdata_Terrain_" + u.get<std::string>() + ".json");
            index.terrain.userdatas.push_back(load_userdata(userdata_path));
        }
        for (const auto &n : t_namespaces)
        {
            auto namespace_path = path.parent_path() / ("namespace_Terrain_" + n.get<std::string>() + ".json");
            index.terrain.namespaces.push_back(load_namespace(namespace_path));
        }
    }
    catch (const nlohmann::json::type_error &e)
    {
        std::println("Type error while accessing JSON: {} (tried to get key: {})", e.what(), e.id);
        return index;
    }
    catch (const nlohmann::json::out_of_range &e)
    {
        std::println("Out of range error while accessing JSON: {}", e.what());
        return index;
    }

    return index;
}

// Diffs two name-keyed lists of the same type, producing an object keyed by
// name with a "status" of added/removed/modified/unchanged — every entry from
// both sides is included so the web UI can list (and optionally hide) the
// unchanged ones too, not just what differs. Both "before" and "after" are
// always included (null on the missing side) so the web UI can render a
// proper structured, field-by-field diff instead of just dumping raw JSON text.
template <typename T>
nlohmann::json diff_list(const std::vector<T> &a, const std::vector<T> &b)
{
    std::map<std::string, const T *> map_a, map_b;
    for (const auto &item : a)
    {
        map_a[item.name] = &item;
    }
    for (const auto &item : b)
    {
        map_b[item.name] = &item;
    }

    auto result = nlohmann::json::object();

    for (const auto &[name, ptr] : map_a)
    {
        if (!map_b.contains(name))
        {
            result[name] = {{"status", "removed"}, {"before", *ptr}, {"after", nullptr}};
        }
    }

    for (const auto &[name, ptr] : map_b)
    {
        auto it = map_a.find(name);
        if (it == map_a.end())
        {
            result[name] = {{"status", "added"}, {"before", nullptr}, {"after", *ptr}};
            continue;
        }

        nlohmann::json ja = *it->second;
        nlohmann::json jb = *ptr;
        result[name] = {
            {"status", ja != jb ? "modified" : "unchanged"},
            {"before", ja},
            {"after", jb},
        };
    }

    return result;
}

nlohmann::json diff_environment(const docs::index::environment &a, const docs::index::environment &b)
{
    return {
        {"classes", diff_list(a.classes, b.classes)},
        {"userdatas", diff_list(a.userdatas, b.userdatas)},
        {"namespaces", diff_list(a.namespaces, b.namespaces)},
    };
}

nlohmann::json diff_index(const docs::index &a, const docs::index &b)
{
    return {
        {"game", diff_environment(a.game, b.game)},
        {"terrain", diff_environment(a.terrain, b.terrain)},
    };
}

int main(int argc, char **argv)
{
    constexpr auto ch1_path = "json_ch1/index.json";
    constexpr auto ch2_path = "json_ch2/index.json";

    auto ch1_index = load_index(ch1_path);
    auto ch2_index = load_index(ch2_path);

    auto diff = diff_index(ch1_index, ch2_index);

    std::filesystem::create_directories("web");
    std::ofstream out("web/diff.json");
    out << diff.dump(2);

    std::println("Wrote web/diff.json");

    return 0;
}
