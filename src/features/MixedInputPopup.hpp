#pragma once

#include <Geode/Geode.hpp>
#include "MultiEditTriggers.hpp"

using namespace geode::prelude;

enum OperatorType {
    Add, Subtract, Multiply, Divide, Equal
};

class MixedInputPopup : public Popup<const std::vector<Trigger>&, std::function<void(std::variant<int, float>)>> {
protected:
    std::vector<Trigger> m_triggers;
    std::function<void(std::variant<int, float>)> m_setProperty;
    OperatorType m_operator;
    CCMenuItemSpriteExtra* m_operatorBtn;

    bool setup(const std::vector<Trigger>&, std::function<void(std::variant<int, float>)>) override;

    CCMenuItemSpriteExtra* createOperatorButton(const OperatorType&);
    void onOperator(CCObject*);
public:
    static MixedInputPopup* create(const std::vector<Trigger>&, std::function<void(std::variant<int, float>)>);
};