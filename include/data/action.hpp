// Copyright (C) 1997, 1999-2001, 2008 Nathan Lamont
// Copyright (C) 2015-2017 The Antares Authors
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

#ifndef ANTARES_DATA_ACTION_HPP_
#define ANTARES_DATA_ACTION_HPP_

#include <stdint.h>
#include <pn/string>
#include <memory>
#include <vector>

#include "data/handle.hpp"
#include "math/fixed.hpp"
#include "math/geometry.hpp"
#include "math/units.hpp"

namespace antares {

class SpaceObject;
struct Level_Initial;

enum objectVerbIDEnum {
    kNoAction            = 0 << 8,
    kCreateObject        = 1 << 8,
    kPlaySound           = 2 << 8,
    kAlter               = 3 << 8,
    kMakeSparks          = 4 << 8,
    kReleaseEnergy       = 5 << 8,
    kLandAt              = 6 << 8,
    kEnterWarp           = 7 << 8,
    kDisplayMessage      = 8 << 8,
    kChangeScore         = 9 << 8,
    kDeclareWinner       = 10 << 8,
    kDie                 = 11 << 8,
    kSetDestination      = 12 << 8,
    kActivateSpecial     = 13 << 8,
    kActivatePulse       = 14 << 8,
    kActivateBeam        = 15 << 8,
    kColorFlash          = 16 << 8,
    kCreateObjectSetDest = 17 << 8,  // creates an object with the same destination as anObject's
                                     // (either subject or direct)
    kNilTarget           = 18 << 8,
    kDisableKeys         = 19 << 8,
    kEnableKeys          = 20 << 8,
    kSetZoom             = 21 << 8,
    kComputerSelect      = 22 << 8,  // selects a line & screen of the minicomputer
    kAssumeInitialObject = 23 << 8,  // assumes the identity of an intial object; for tutorial
};

enum alterVerbIDType {
    kAlterDamage      = kAlter | 0,
    kAlterVelocity    = kAlter | 1,
    kAlterThrust      = kAlter | 2,
    kAlterMaxThrust   = kAlter | 3,
    kAlterMaxVelocity = kAlter | 4,
    kAlterMaxTurnRate = kAlter | 5,
    kAlterLocation    = kAlter | 6,
    kAlterScale       = kAlter | 7,
    kAlterWeapon1     = kAlter | 8,
    kAlterWeapon2     = kAlter | 9,
    kAlterSpecial     = kAlter | 10,
    kAlterEnergy      = kAlter | 11,
    kAlterOwner       = kAlter | 12,
    kAlterHidden      = kAlter | 13,
    kAlterCloak       = kAlter | 14,
    kAlterOffline     = kAlter | 15,
    kAlterSpin        = kAlter | 16,
    kAlterBaseType    = kAlter | 17,
    kAlterConditionTrueYet =
            kAlter | 18,  // relative = state, min = which condition basically force to recheck
    kAlterOccupation = kAlter | 19,  // for special neutral death objects
    kAlterAbsoluteCash =
            kAlter |
            20,  // relative: true = cash to object : false = range = admiral who gets cash
    kAlterAge              = kAlter | 21,
    kAlterAttributes       = kAlter | 22,
    kAlterLevelKeyTag      = kAlter | 23,
    kAlterOrderKeyTag      = kAlter | 24,
    kAlterEngageKeyTag     = kAlter | 25,
    kAlterAbsoluteLocation = kAlter | 26,
};

//
// Action:
//  Defines any action that an object can take.  Conditions that can cause an action to execute
//  are:
//  destroy, expire, create, collide, activate, or message.
//

// TODO(sfiera): use std::variant<> when it’s available.
struct argumentType {
    argumentType() {}

    struct AlterBaseType {
        bool               keep_ammo;
        Handle<BaseObject> base;
    } alterBaseType;

    struct AlterLocation {
        bool    relative;
        int32_t by;
    } alterLocation;

    struct AlterAbsoluteLocation {
        bool  relative;
        Point at;
    } alterAbsoluteLocation;

    struct AlterHidden {
        int32_t first;
        int32_t count_minus_1;
    } alterHidden;

    struct AlterConditionTrueYet {
        bool    true_yet;
        int32_t first;
        int32_t count_minus_1;
    } alterConditionTrueYet;

    // release energy
    struct ReleaseEnergy {
        Fixed percent;
    };
    ReleaseEnergy releaseEnergy;
};

struct ActionBase;

class Action {
  public:
                operator bool() const { return _base != nullptr; }
    ActionBase* operator->() const { return _base.get(); }
    ActionBase& operator*() const { return *_base; }

    template <typename T>
    T* init() {
        T* out;
        _base = std::unique_ptr<ActionBase>(out = new T);
        return out;
    }

  private:
    std::unique_ptr<ActionBase> _base;
};

struct ActionBase {
    uint16_t verb;

    bool     reflexive;        // does it apply to object executing verb?
    uint32_t inclusiveFilter;  // if it has ALL these attributes, OK -- for non-reflective verbs
    uint32_t exclusiveFilter;  // don't execute if it has ANY of these
    uint8_t  levelKeyTag;
    int16_t  owner;  // 0 no matter, 1 same owner, -1 different owner
    ticks    delay;
    //  uint32_t                    reserved1;
    Handle<Level_Initial> initialSubjectOverride;
    Handle<Level_Initial> initialDirectOverride;
    uint32_t              reserved2;
    argumentType          argument;

    virtual ~ActionBase() = default;
    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset) = 0;

    virtual Handle<BaseObject>  created_base() const;
    virtual std::pair<int, int> sound_range() const;

    static const size_t byte_size = 48;
};
bool read_from(pn::file_view in, Action* action);

std::vector<Action> read_actions(int begin, int end);

struct NoAction : public ActionBase {
    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct CreateObjectAction : public ActionBase {
    Handle<BaseObject> base;                        // what type
    int32_t            count_minimum      = 1;      // # to make min
    int32_t            count_range        = 0;      // # to make range
    bool               relative_velocity  = false;  // is velocity relative to creator?
    bool               relative_direction = false;  // determines initial heading
    int32_t            distance           = 0;      // create at this distance in random direction
    bool               inherit            = false;  // if false, gets creator as target
                                                    // if true, gets creator’s target as target

    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
    virtual Handle<BaseObject> created_base() const;
};

struct PlaySoundAction : public ActionBase {
    uint8_t priority;                // 1-5; takes over a channel playing a lower-priority sound
    ticks   persistence;             // time before a lower-priority sound can take channel
    bool    absolute;                // plays at same volume, regardless of distance from player
    int32_t volume;                  // 1-255; volume at focus
    std::pair<int32_t, int32_t> id;  // pick ID randomly in [first, second)

    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
    virtual std::pair<int, int> sound_range() const;
};

struct MakeSparksAction : public ActionBase {
    int32_t count;     // number of sparks to create
    uint8_t hue;       // hue of sparks; they start bright and fade with time
    int32_t decay;     // sparks will be visible for 17.05/decay seconds
    Fixed   velocity;  // sparks fly at at random speed up to this

    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct LandAtAction : public ActionBase {
    int32_t speed;

    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct EnterWarpAction : public ActionBase {
    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct DisplayMessageAction : public ActionBase {
    int16_t                 id;     // identifies the message to a "message" condition
    std::vector<pn::string> pages;  // pages of message bodies to show

    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct ChangeScoreAction : public ActionBase {
    Handle<Admiral> player;  // which player’s score to change; -1 = owner of focus
    int32_t         which;   // 0-2; each player has three "scores"
    int32_t         value;   // amount to change by

    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct DeclareWinnerAction : public ActionBase {
    Handle<Admiral> player;  // victor; -1 = owner of focus
    int32_t         next;    // next chapter to play; -1 = none
    pn::string      text;    // "debriefing" text

    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct DieAction : public ActionBase {
    enum class Kind {
        // Removes the focus without any further fanfare.
        NONE = 0,

        // Removes the subject without any further fanfare.
        // Essentially, this is NONE, but always reflexive.
        EXPIRE = 1,

        // Removes the subject and executes its destroy action.
        DESTROY = 2,
    };
    Kind kind;

    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct SetDestinationAction : public ActionBase {
    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct ActivateSpecialAction : public ActionBase {
    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct ActivatePulseAction : public ActionBase {
    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct ActivateBeamAction : public ActionBase {
    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct ColorFlashAction : public ActionBase {
    int32_t length;  // length of color flash
    uint8_t hue;     // hue of flash
    uint8_t shade;   // brightness of flash

    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct NilTargetAction : public ActionBase {
    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct DisableKeysAction : public ActionBase {
    uint32_t disable = 0;  // keys to disable

    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct EnableKeysAction : public ActionBase {
    uint32_t enable = 0;  // keys to enable

    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct SetZoomAction : public ActionBase {
    int32_t value;

    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct ComputerSelectAction : public ActionBase {
    int32_t screen;
    int32_t line;

    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct AssumeInitialObjectAction : public ActionBase {
    int32_t which;  // which initial to become
                    // Note: player 1’s score 0 is added to this number

    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct AlterDamageAction : public ActionBase {
    int32_t value;

    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct AlterVelocityAction : public ActionBase {
    enum class Kind {
        STOP,        // set focus’s velocity to 0
        COLLIDE,     // impart velocity from subject like a collision (capped)
        DECELERATE,  // decrease focus’s velocity (capped)
        SET,         // set focus’s velocity to value in subject’s direction
        BOOST,       // add to focus’s velocity in subject’s direction
        CRUISE,      // set focus’s velocity in focus’s direction
    };
    Kind  kind;
    Fixed value;

    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct AlterThrustAction : public ActionBase {
    bool                    relative;  // if true, set to value; if false, add value
    std::pair<Fixed, Fixed> value;     // range

    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct AlterMaxVelocityAction : public ActionBase {
    Fixed value;  // if >= 0, set to value; if < 0, set to base type’s default

    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct AlterMaxTurnRateAction : public ActionBase {
    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct AlterLocationAction : public ActionBase {
    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct AlterScaleAction : public ActionBase {
    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct AlterWeapon1Action : public ActionBase {
    Handle<BaseObject> base;

    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct AlterWeapon2Action : public ActionBase {
    Handle<BaseObject> base;

    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct AlterSpecialAction : public ActionBase {
    Handle<BaseObject> base;

    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct AlterEnergyAction : public ActionBase {
    int32_t value;

    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct AlterOwnerAction : public ActionBase {
    bool relative;  // if true and reflexive, set subject’s owner to object’s
                    // if true and non-reflexive, set object’s owner to subject’s
                    // if false, set focus’s owner to `player`
    Handle<Admiral> player;

    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct AlterHiddenAction : public ActionBase {
    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct AlterCloakAction : public ActionBase {
    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct AlterOfflineAction : public ActionBase {
    std::pair<Fixed, Fixed> value;

    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct AlterSpinAction : public ActionBase {
    std::pair<Fixed, Fixed> value;

    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct AlterBaseTypeAction : public ActionBase {
    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct AlterConditionTrueYetAction : public ActionBase {
    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct AlterOccupationAction : public ActionBase {
    int32_t value;

    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct AlterAbsoluteCashAction : public ActionBase {
    bool            relative;  // if true, pay focus’s owner; if false, pay `player`
    Fixed           value;     // amount to pay; not affected by earning power
    Handle<Admiral> player;

    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct AlterAgeAction : public ActionBase {
    bool                    relative;  // if true, add value to age; if false, set age to value
    std::pair<ticks, ticks> value;     // age range

    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct AlterAttributesAction : public ActionBase {
    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct AlterLevelKeyTagAction : public ActionBase {
    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct AlterOrderKeyTagAction : public ActionBase {
    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct AlterEngageKeyTagAction : public ActionBase {
    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

struct AlterAbsoluteLocationAction : public ActionBase {
    virtual void apply(
            Handle<SpaceObject> subject, Handle<SpaceObject> focus, Handle<SpaceObject> object,
            Point* offset);
};

}  // namespace antares

#endif  // ANTARES_DATA_ACTION_HPP_
