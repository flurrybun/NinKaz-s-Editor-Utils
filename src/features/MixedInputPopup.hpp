#pragma once

#include <Geode/Geode.hpp>
#include "MultiEditTriggers.hpp"

using namespace geode::prelude;

enum Operator {
    Add, Subtract, Multiply, Divide, Equal
};

class MixedInputPopup : public Popup<const std::vector<Trigger>&, Trigger::PropType> {
protected:
    std::vector<Trigger> m_triggers;
    Trigger::PropType m_type;
    Operator m_operator;
    CCMenuItemToggler* m_operatorBtn;
    std::variant<int, float> m_value;

    bool setup(const std::vector<Trigger>&, Trigger::PropType) override;

    CCSprite* createOperatorSprite(const Operator&);
    CCMenuItemToggler* createOperatorButton(const Operator&);
    CCScale9Sprite* createOperatorBase(bool);
    void onOperator(CCObject*);
    void onApply(CCObject* sender);
public:
    static MixedInputPopup* create(const std::vector<Trigger>&, Trigger::PropType);
};