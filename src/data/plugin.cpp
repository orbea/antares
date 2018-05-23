// Copyright (C) 1997, 1999-2001, 2008 Nathan Lamont
// Copyright (C) 2016-2017 The Antares Authors
//
// This file is part of Antares, a tactical space combat game.
//
// Antares is free software: you can redistribute it and/or modify it
// under the terms of the Lesser GNU General Public License as published
// by the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Antares is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with Antares.  If not, see http://www.gnu.org/licenses/

#include "data/plugin.hpp"

#include <glob.h>
#include <algorithm>
#include <pn/file>

#include "config/dirs.hpp"
#include "config/preferences.hpp"
#include "data/base-object.hpp"
#include "data/condition.hpp"
#include "data/field.hpp"
#include "data/initial.hpp"
#include "data/level.hpp"
#include "data/races.hpp"
#include "data/resource.hpp"
#include "game/sys.hpp"
#include "lang/defines.hpp"

using sfz::range;
using std::vector;

namespace antares {

static constexpr int kPluginFormat = 20;

ANTARES_GLOBAL ScenarioGlobals plug;

namespace {

struct ScopedGlob {
    glob_t data;
    ScopedGlob() { memset(&data, sizeof(data), 0); }
    ~ScopedGlob() { globfree(&data); }
};

}  // namespace

static void read_all_levels() {
    ScopedGlob g;
    pn::string dir;
    if (sys.prefs->scenario_identifier() == kFactoryScenarioIdentifier) {
        dir = application_path().copy();
    } else {
        dir = scenario_path();
    }
    glob(pn::format("{0}/levels/*.pn", dir).c_str(), 0, NULL, &g.data);

    plug.levels.clear();
    plug.chapters.clear();
    for (int i = 0; i < g.data.gl_pathc; ++i) {
        const pn::string_view full_path = g.data.gl_pathv[i];
        const pn::string_view name =
                full_path.substr(dir.size() + 8, full_path.size() - dir.size() - 11);

        pn::value x = Resource::level(name);
        try {
            auto it = plug.levels.emplace(name.copy(), level(x)).first;
            if (it->second.chapter.has_value()) {
                plug.chapters[*it->second.chapter] = &it->second;
            }
        } catch (...) {
            std::throw_with_nested(std::runtime_error(name.copy().c_str()));
        }
    }
}

void PluginInit() {
    pn::value x = Resource::info();
    try {
        plug.info = info(path_value{x});
        if (plug.info.format != kPluginFormat) {
            throw std::runtime_error(
                    pn::format("unknown plugin format {0}", plug.info.format).c_str());
        }
        plug.splash  = Resource::texture(plug.info.splash_screen);
        plug.starmap = Resource::texture(plug.info.starmap);
    } catch (...) {
        std::throw_with_nested(std::runtime_error("info.pn"));
    }

    read_all_levels();
}

void load_race(const NamedHandle<const Race>& r) {
    if (plug.races.find(r.name().copy()) != plug.races.end()) {
        return;  // already loaded.
    }

    pn::value x = Resource::race(r.name());
    try {
        plug.races.emplace(r.name().copy(), race(path_value{x}));
    } catch (...) {
        std::throw_with_nested(std::runtime_error(r.name().copy().c_str()));
    }
}

void load_object(const NamedHandle<const BaseObject>& o) {
    if (plug.objects.find(o.name().copy()) != plug.objects.end()) {
        return;  // already loaded.
    }

    pn::value x = Resource::object(o.name());
    try {
        plug.objects.emplace(o.name().copy(), base_object(x));
    } catch (...) {
        std::throw_with_nested(std::runtime_error(o.name().copy().c_str()));
    }
}

}  // namespace antares
