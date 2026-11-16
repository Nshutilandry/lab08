#include "roster_io.h"

#include <fstream>
#include <sstream>

namespace {

bool parse_integer(const std::string& field, int& value) {
    std::istringstream in(field);
    return in >> value && (in >> std::ws).eof();
}

bool parse_mech_line(const std::string& line, Mech& mech) {
    const std::size_t first_comma = line.find(',');
    if (line.empty() || first_comma == std::string::npos) {
        return false;
    }

    const std::size_t second_comma = line.find(',', first_comma + 1);
    if (second_comma == std::string::npos) {
        return false;
    }

    const std::size_t third_comma = line.find(',', second_comma + 1);
    if (third_comma == std::string::npos || line.find(',', third_comma + 1) != std::string::npos) {
        return false;
    }

    const std::string name = line.substr(0, first_comma);
    int hp = 0;
    int attack = 0;
    int armor = 0;
    if (name.empty() ||
        !parse_integer(line.substr(first_comma + 1, second_comma - first_comma - 1), hp) ||
        !parse_integer(line.substr(second_comma + 1, third_comma - second_comma - 1), attack) ||
        !parse_integer(line.substr(third_comma + 1), armor) || hp < 1 || attack < 0 ||
        armor < 0) {
        return false;
    }

    mech = Mech(name, hp, attack, armor);
    return true;
}

}  // namespace

bool save_roster(const std::vector<Mech>& roster, const std::string& path) {
    std::ofstream out(path);
    if (!out) {
        return false;
    }

    for (const Mech& mech : roster) {
        out << mech.name() << ',' << mech.hp() << ',' << mech.attack() << ',' << mech.armor()
            << '\n';
    }

    out.close();
    return static_cast<bool>(out);
}

bool load_roster(const std::string& path, std::vector<Mech>& roster, int& skipped_lines) {
    std::ifstream in(path);
    if (!in) {
        return false;
    }

    std::vector<Mech> loaded;
    int skipped = 0;
    std::string line;
    while (std::getline(in, line)) {
        Mech mech("", 0, 0, 0);
        if (parse_mech_line(line, mech)) {
            loaded.push_back(mech);
        } else {
            ++skipped;
        }
    }

    roster.swap(loaded);
    skipped_lines = skipped;
    return true;
}

bool append_line(const std::string& path, const std::string& text) {
    std::ofstream out(path, std::ios::app);
    if (!out) {
        return false;
    }

    out << text << '\n';
    out.close();
    return static_cast<bool>(out);
}
