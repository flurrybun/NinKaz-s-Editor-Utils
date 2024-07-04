#include "MixedInputPopup.hpp"
#include <Geode/ui/TextInput.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>

#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include <vector>
#include <map>
#include <algorithm>
#include <variant>

bool MixedInputPopup::setup(const std::vector<Trigger>& triggers, Trigger::PropType type) {
    m_triggers = triggers;
    m_type = type;
    
    auto winSize = m_mainLayer->getContentSize();

    // INFO BUTTON

    auto infoText = "Modify the <cg>mixed value</c> of the <cy>selected input</c>.\n"
        "Use the <cl>equals button</c> to override the old value. Use the <cp>operator buttons</c> to add, subtract, multiply, or divide the old value.\n";
    
    auto infoBtn = InfoAlertButton::create("Info", infoText, 0.7f);
    infoBtn->setPosition(winSize - ccp(18, 18));
    m_buttonMenu->addChild(infoBtn);

    // SCROLL CONTAINER

    auto scrollSize = CCSize(300, 150);
    auto scrollPosition = winSize / 2 + ccp(0, 48);

    auto scroll = ScrollLayer::create(scrollSize);
    scroll->setAnchorPoint({0, 0});
    scroll->setPosition(scrollPosition - scrollSize / 2);

    auto scrollContent = scroll->m_contentLayer;

    // SCROLL BORDER

    auto topBorder = CCSprite::createWithSpriteFrameName("GJ_commentTop_001.png");
    topBorder->setPosition(scrollPosition + ccp(0, scrollSize.height / 2 - 4));
    topBorder->setZOrder(10);
    topBorder->setScale(0.885);
    auto bottomBorder = CCSprite::createWithSpriteFrameName("GJ_commentTop_001.png");
    bottomBorder->setPosition(scrollPosition + ccp(0, -scrollSize.height / 2 + 4));
    bottomBorder->setZOrder(10);
    bottomBorder->setFlipY(true);
    bottomBorder->setScale(0.885);
    auto leftBorder = CCSprite::createWithSpriteFrameName("GJ_commentSide_001.png");
    leftBorder->setPosition(scrollPosition + ccp(-scrollSize.width / 2 - 3.4, 0));
    leftBorder->setZOrder(9);
    leftBorder->setScaleX(0.885);
    leftBorder->setScaleY(4.5);
    auto rightBorder = CCSprite::createWithSpriteFrameName("GJ_commentSide_001.png");
    rightBorder->setPosition(scrollPosition + ccp(scrollSize.width / 2 + 3.4, 0));
    rightBorder->setZOrder(9);
    rightBorder->setFlipX(true);
    rightBorder->setScaleX(0.885);
    rightBorder->setScaleY(4.5);

    m_mainLayer->addChild(topBorder);
    m_mainLayer->addChild(bottomBorder);
    m_mainLayer->addChild(leftBorder);
    m_mainLayer->addChild(rightBorder);

    // SCROLL CONTENT

    std::variant<std::map<int, std::vector<int>>, std::map<float, std::vector<float>>> values;

    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::map<int, std::vector<int>>>) {
            for (const auto& trigger : m_triggers) {
                if (!trigger.hasProperty(m_type)) continue;

                auto property = std::get<int>(trigger.getProperty(m_type));
                arg[property].push_back(property);
            }
        } else if constexpr (std::is_same_v<T, std::map<float, std::vector<float>>>) {
            for (const auto& trigger : m_triggers) {
                if (!trigger.hasProperty(m_type)) continue;
                
                auto property = std::get<float>(trigger.getProperty(m_type));
                arg[property].push_back(property);
            }
        }

        auto length = arg.size();
        auto index = 0;
        auto rowSize = CCSize(scrollSize.width, 40.f);

        scrollContent->setContentSize({rowSize.width, rowSize.height * length});
        
        for (const auto& [key, value] : arg) {
            index++;

            auto bg = CCLayerColor::create();
            bg->setColor((index % 2 == 0) ? ccc3(144, 79, 39) : ccc3(156, 85, 42));
            bg->setOpacity(255);
            bg->setContentSize(rowSize);
            bg->setPosition({0, rowSize.height * length - (index * rowSize.height)});

            auto menu = CCMenu::create();
            menu->setPosition({0, 0});
            menu->setContentSize({0, 0});
            menu->ignoreAnchorPointForPosition(false);
            menu->setAnchorPoint({0, 0});

            auto label = CCLabelBMFont::create(std::to_string(key).c_str(), "bigFont.fnt");
            label->setScale(0.6);
            label->setPosition({20, 15});

            auto calcLayout = CCMenu::create();
            calcLayout->setLayout(
                RowLayout::create()
                    ->setGap(10.f)
            );
            calcLayout->setPosition({rowSize.width / 2 + 20, rowSize.height / 2});
            calcLayout->setScale(0.6f);

            auto sign = createOperatorSprite(m_operator);
            sign->setScale(0.6f);
            auto equalSign = createOperatorSprite(Operator::Equal);
            equalSign->setScale(0.6f);

            const char* changeValueStr = std::visit([](const auto& value) {
                if (!value) return "0";
                return std::to_string(value).c_str();
            }, m_value);

            auto changeValue = CCLabelBMFont::create(changeValueStr, "bigFont.fnt");

            const char* finalValueStr = std::visit([&](const auto& value) {
                if (!value) {
                    if (m_operator == Operator::Add || m_operator == Operator::Subtract) return std::to_string(value).c_str();
                    return "0";
                }
                
                auto finalValue = m_operator == Operator::Add ? key + value :
                    m_operator == Operator::Subtract ? key - value :
                    m_operator == Operator::Multiply ? key * value :
                    m_operator == Operator::Divide ? key / value : key;
                
                return std::to_string(finalValue).c_str();
            }, m_value);

            auto finalLabel = CCLabelBMFont::create(finalValueStr, "bigFont.fnt");
            label->setScale(0.6);
            label->setPosition({rowSize.width - 20, 15});

            calcLayout->addChild(sign);
            calcLayout->addChild(changeValue);
            calcLayout->addChild(equalSign);

            calcLayout->setScale(0.6f);

            menu->addChild(calcLayout);
            calcLayout->updateLayout();

            menu->addChild(label);
            menu->addChild(finalLabel);

            bg->addChild(menu);
            scrollContent->addChild(bg);
        }
    }, values);

    scroll->moveToTop();
    m_mainLayer->addChild(scroll);

    // LABEL

    auto label = CCLabelBMFont::create("Modify Mixed Value", "goldFont.fnt");
    label->setScale(0.6);
    label->setPosition({winSize. width / 2, 98});
    m_mainLayer->addChild(label);

    // OPERATOR BUTTONS AND INPUT ROW

    auto bottomLayout = CCMenu::create();
    bottomLayout->setLayout(
        RowLayout::create()
            ->setGap(10.f)
    );
    bottomLayout->setPosition({winSize.width / 2, 65});
    
    bottomLayout->addChild(createOperatorButton(Operator::Equal));
    bottomLayout->addChild(createOperatorButton(Operator::Add));
    bottomLayout->addChild(createOperatorButton(Operator::Subtract));
    bottomLayout->addChild(createOperatorButton(Operator::Multiply));
    bottomLayout->addChild(createOperatorButton(Operator::Divide));

    auto input = TextInput::create(70.f, "Num", "bigFont.fnt");
    input->setFilter("0123456789");
    input->setMaxCharCount(4);
    input->setScale(0.8);
    input->setLayoutOptions(
        AxisLayoutOptions::create()
            ->setNextGap(10.f)
            ->setPrevGap(10.f)
    );

    auto decArrowSpr = CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
    decArrowSpr->setScale(0.96);
    auto decArrowBtn = CCMenuItemSpriteExtra::create(
        decArrowSpr, this, nullptr
    );
    decArrowBtn->setTag(-1);
    decArrowBtn->setLayoutOptions(
        AxisLayoutOptions::create()
            ->setPrevGap(15.f)
    );

    auto incArrowSpr = CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
    incArrowSpr->setScale(0.96);
    auto incArrowBtn = CCMenuItemSpriteExtra::create(
        incArrowSpr, this, nullptr
    );
    incArrowBtn->setTag(1);

    bottomLayout->addChild(decArrowBtn);
    bottomLayout->addChild(input);
    bottomLayout->addChild(incArrowBtn);

    m_buttonMenu->addChild(bottomLayout);
    bottomLayout->updateLayout();

    // APPLY BUTTON

    auto applySpr = ButtonSprite::create("Apply", "goldFont.fnt", "GJ_button_01.png", .9f);
    auto applyBtn = CCMenuItemSpriteExtra::create(
        applySpr, this, nullptr
    );
    applyBtn->setPosition({winSize.width / 2, 24});
    m_buttonMenu->addChild(applyBtn);

    return true;
}

CCSprite* MixedInputPopup::createOperatorSprite(const Operator& sign) {
    auto name = sign == Operator::Add ? "plus-operator.png"_spr :
        sign == Operator::Subtract ? "minus-operator.png"_spr :
        sign == Operator::Multiply ? "multiply-operator.png"_spr :
        sign == Operator::Divide ? "divide-operator.png"_spr : "equal-operator.png"_spr;

    return CCSprite::createWithSpriteFrameName(name);
}

CCMenuItemToggler* MixedInputPopup::createOperatorButton(const Operator& sign) {
    auto topSpr = createOperatorSprite(sign);
    topSpr->setScale(0.9f);
    topSpr->setPosition({20, 20});

    auto offSpr = createOperatorBase(false);
    auto onSpr = createOperatorBase(true);
    
    offSpr->addChild(topSpr);
    onSpr->addChild(topSpr);

    auto btn = CCMenuItemToggler::create(
        offSpr, onSpr, this, menu_selector(MixedInputPopup::onOperator)
    );

    btn->setTag(static_cast<int>(sign));

    if (sign == Operator::Equal) {
        btn->toggle(true);
        m_operatorBtn = btn;
    };

    return btn;
}

CCScale9Sprite* MixedInputPopup::createOperatorBase(bool isSelected) {
    auto spr = CCScale9Sprite::create(isSelected ? "GJ_button_02.png" : "GJ_button_04.png", {0, 0, 40, 40});
    spr->setContentSize({40, 40});
    spr->setScale(0.7f);
    return spr;
}

void MixedInputPopup::onOperator(CCObject* sender) {
    auto btn = static_cast<CCMenuItemToggler*>(sender);

    if (m_operatorBtn) m_operatorBtn->toggle(false);
    
    // don't need to toggle the button manually since it's automatically toggled when the player clicks on it

    m_operatorBtn = btn;
    m_operator = static_cast<Operator>(btn->getTag());
}

void MixedInputPopup::onApply(CCObject* sender) {
    for (auto& trigger : m_triggers) {
        auto value = trigger.getProperty(Trigger::PropType::Item);

        std::visit([&](const auto& value) {
            trigger.setProperty(Trigger::PropType::Item, value);
        }, m_value);

        // if (std::holds_alternative<int>(value)) {
        //     auto value = std::get<int>(m_value);

        //     switch (m_operator) {
        //         case Operator::Add:
        //             trigger.setProperty(Trigger::PropType::Item, value);
        //             break;
        //         case Operator::Subtract:
        //             trigger.setProperty(Trigger::PropType::Item, value);
        //             break;
        //         case Operator::Multiply:
        //             trigger.setProperty(Trigger::PropType::Item, value);
        //             break;
        //         case Operator::Divide:
        //             trigger.setProperty(Trigger::PropType::Item, value);
        //             break;
        //         case Operator::Equal:
        //             trigger.setProperty(Trigger::PropType::Item, value);
        //             break;
        //     }
        // } else if (std::holds_alternative<float>(value)) {
        //     auto value = std::get<float>(m_value);

        //     switch (m_operator) {
        //         case Operator::Add:
        //             trigger.setProperty(Trigger::PropType::Item, value);
        //             break;
        //         case Operator::Subtract:
        //             trigger.setProperty(Trigger::PropType::Item, value);
        //             break;
        //         case Operator::Multiply:
        //             trigger.setProperty(Trigger::PropType::Item, value);
        //             break;
        //         case Operator::Divide:
        //             trigger.setProperty(Trigger::PropType::Item, value);
        //             break;
        //         case Operator::Equal:
        //             trigger.setProperty(Trigger::PropType::Item, value);
        //             break;
        //     }
        // }
    }
    
    onClose(sender);
}

// MixedInputPopup* MixedInputPopup::create(const std::vector<Trigger>& triggers, std::function<void(std::variant<int, float>)> setProperty, std::function<void(Trigger::PropType)> getProperty){
MixedInputPopup* MixedInputPopup::create(const std::vector<Trigger>& triggers, Trigger::PropType type){
    auto ret = new MixedInputPopup();
    if (ret && ret->initAnchored(380.f, 280.f, triggers, type)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}