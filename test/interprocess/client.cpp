#include <jakilid.hpp>

#include <functional>
#include <iostream>
#include <stdexcept>
#include <string>

using jakilid::SharedDict;

void raise_for_arguments(const std::vector<std::string>& args, unsigned int number_of_mandatory_arguments) {
    if (args.size() != number_of_mandatory_arguments)
        throw std::invalid_argument("Wrong number argument for insert");
}

bool insert(const std::string& instance_name, const std::vector<std::string>& args) {

    raise_for_arguments(args, 2);
    SharedDict(instance_name).Insert(args[0], args[1]);
    return true;
}

bool find(const std::string& instance_name, const std::vector<std::string>& args) {
    raise_for_arguments(args, 1);

    std::string value;
    auto success = SharedDict(instance_name).Find(args[0], value);
    if (success) {
        std::cout << value << std::endl;
        return true;
    }

    return false;
}

bool contains(const std::string& instance_name, const std::vector<std::string>& args) {
    raise_for_arguments(args, 1);
    return SharedDict(instance_name).Contains(args[0]);
}

bool erase(const std::string& instance_name, const std::vector<std::string>& args) {
    raise_for_arguments(args, 1);
    return SharedDict(instance_name).Erase(args[0]);
}

bool size(const std::string& instance_name, const std::vector<std::string>& args) {
    raise_for_arguments(args, 0);
    std::cout << SharedDict(instance_name).Size() << std::endl;
    return true;
}

bool drop(const std::string& instance_name, const std::vector<std::string>& args) {
    raise_for_arguments(args, 0);
    return SharedDict(instance_name).Drop();
}

using CommandHandler = std::function<bool(const std::string&, const std::vector<std::string>&)>;

auto command_handlers = std::map<std::string, CommandHandler> {
    {"insert", insert},
    {"find", find},
    {"contains", contains},
    {"erase", erase},
    {"size", size},
    {"drop", drop}
};

int main(int argc, char* argv[]) {

    if (argc < 3) {
        throw std::invalid_argument("Instance name and command are mandatory");
    }

    std::string instance_name = argv[1];
    std::string command = argv[2];

    std::vector<std::string> arguments;
    for(unsigned int i = 3; i < argc; ++i)
        arguments.emplace_back(argv[i]);
    
    auto handler = command_handlers[command];

    auto success = handler(instance_name, arguments);

    if (success)
        return 0;

    return 1;
}
