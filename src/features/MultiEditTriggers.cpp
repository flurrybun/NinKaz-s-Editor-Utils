#include "MultiEditTriggers.hpp"
#include "MixedInputPopup.hpp"
#include <Geode/modify/EditTriggersPopup.hpp>
#include <Geode/ui/TextInput.hpp>

#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include <map>
#include <vector>
#include <algorithm>
#include <iterator>

// class $modify(MultiEditTriggersPopup, EditTriggersPopup) {
//     void onButton(CCObject* sender) {
//         FLAlertLayer::create(
//             "Button",
//             "Button",
//             "Button"
//         )->show();
//     }
    
//     bool init(EffectGameObject* p0, CCArray* p1) {
//         if (!EditTriggersPopup::init(p0, p1)) return false;

//         auto menuSpr = ButtonSprite::create("in menu");
//         auto menuBtn = CCMenuItemSpriteExtra::create(menuSpr, this, menu_selector(MultiEditTriggersPopup::onButton));
//         menuBtn->setPosition(-100, 0);

//         auto notInMenuSpr = ButtonSprite::create("not in menu");
//         auto notInMenuBtn = CCMenuItemSpriteExtra::create(notInMenuSpr, this, menu_selector(MultiEditTriggersPopup::onButton));
//         notInMenuBtn->setPosition(100, 0);

//         auto menu = AxisLayout::create();

//         menu->addChild(menuBtn);
//         m_buttonMenu->addChild(menu);
//         m_buttonMenu->addChild(notInMenuBtn);

//         return true;
//     }
// };

class $modify(MultiEditTriggersPopup, EditTriggersPopup) {
    std::vector<Trigger> m_triggers;
    short easingIndex = 0;

    std::string floatToRoundedString(float value) {
        value = std::round(value * 100) / 100;
        auto valueStr = std::to_string(value);
        valueStr.erase(valueStr.size() - 4);

        return valueStr;
    }

    CCMenu* createGroupMenu(GroupMenuType type, std::optional<int> initialValue) {
        auto menu = CCMenu::create();
        menu->setContentSize({ 122, 50 });
        auto center = ccp(61, 25);

        auto input = TextInput::create(70.f, "Num", "bigFont.fnt");
        input->setFilter("0123456789");
        input->setMaxCharCount(4);
        input->setScale(0.8);
        input->setPosition(center + ccp(0, -12));
        
        input->setString(initialValue.has_value() ? std::to_string(*initialValue) : "Mixed");

        input->setCallback([this, type](const std::string &text) {
            if (text.empty()) return;

            auto value = std::stoi(text);
            if (value < 0) value = 0;
            if (value > 9999) value = 9999;

            for (auto& trigger : m_fields->m_triggers) {
                if (type == GroupMenuType::TargetGroup && trigger.hasTargetGroup) {
                    trigger.object->m_targetGroupID = value;
                } else if (type == GroupMenuType::CenterGroup && trigger.hasCenterGroup) {
                    trigger.object->m_centerGroupID = value;
                } else if (type == GroupMenuType::Item && trigger.hasItem) {
                    trigger.object->m_itemID = value;
                }
            }
        });

        auto labelText = type == GroupMenuType::TargetGroup ? "Target Group ID:" :
            type == GroupMenuType::CenterGroup ? "Center Group ID:" : "Item ID:";
        auto label = CCLabelBMFont::create(labelText, "goldFont.fnt");
        label->setPosition(center + ccp(0, 17)); // prev. 0, 30
        label->setScale(0.56);

        auto decArrowSpr = CCSprite::createWithSpriteFrameName("edit_leftBtn_001.png");
        decArrowSpr->setScale(0.96);
        auto decArrowBtn = CCMenuItemSpriteExtra::create(
            decArrowSpr, this, nullptr
        );
        decArrowBtn->setPosition(center + ccp(-45, -12));
        decArrowBtn->setTag(-1);

        auto incArrowSpr = CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
        incArrowSpr->setScale(0.96);
        auto incArrowBtn = CCMenuItemSpriteExtra::create(
            incArrowSpr, this, nullptr
        );
        incArrowBtn->setPosition(center + ccp(45, -12));
        incArrowBtn->setTag(1);

        menu->addChild(input);
        menu->addChild(label);
        menu->addChild(decArrowBtn);
        menu->addChild(incArrowBtn);

        menu->setTouchPriority(-505);

        return menu;
    };

    void onSliderChanged(CCObject* sender) {
        auto sliderThumb = typeinfo_cast<SliderThumb*>(sender);
        auto tag = sliderThumb->getTag();
        auto value = sliderThumb->getValue();

        if (tag == SliderMenuType::Duration) value *= 10;

        auto menu = typeinfo_cast<CCMenu*>(sliderThumb->getParent()->getParent()->getParent());
        auto input = getChildOfType<TextInput>(menu, 0);

        input->setString(floatToRoundedString(value), true);
    }

    CCMenu* createSliderMenu(SliderMenuType type, std::optional<float> initialValue) {
        auto menu = CCMenu::create();
        menu->setContentSize({ 134, 46 });
        auto center = ccp(67, 23);

        auto slider = Slider::create(this, menu_selector(MultiEditTriggersPopup::onSliderChanged), 0.64);
        slider->setPosition(center + ccp(0, -15));
        slider->getThumb()->setTag(type);

        if (type == SliderMenuType::Duration) slider->setValue(initialValue.has_value() ? *initialValue / 10 : 0.f);
        else if (type == SliderMenuType::Opacity) slider->setValue(initialValue.has_value() ? *initialValue : 0.f);

        auto input = TextInput::create(70.f, "Num", "bigFont.fnt");
        input->setFilter("0123456789");
        input->setMaxCharCount(4);
        input->setScale(0.8);
        input->setPosition(center + ccp(31, 11));

        input->setString(initialValue.has_value() ? floatToRoundedString(*initialValue) : "Mixed");

        input->setCallback([this, type, slider](const std::string &text) {
            if (text.empty()) return;

            auto value = std::stof(text);
            if (type == SliderMenuType::Opacity) value = std::clamp(value, 0.0f, 1.0f);

            slider->setValue(value / 10);

            for (auto& trigger : m_fields->m_triggers) {
                if (type == SliderMenuType::Duration && trigger.hasDuration) {
                    trigger.object->m_duration = value;
                } else if (type == SliderMenuType::Opacity && trigger.hasOpacity) {
                    trigger.object->m_opacity = value;
                }
            }
        });
        
        auto labelText = type == SliderMenuType::Duration ? "Duration:" : "Opacity:";
        auto label = CCLabelBMFont::create(labelText, "goldFont.fnt");
        label->setPosition(center + ccp(0, 11));
        label->setAnchorPoint({ 1, 0.5 });
        label->setScale(0.56);

        menu->addChild(input);
        menu->addChild(slider);
        menu->addChild(label);

        menu->setTouchPriority(-505);

        return menu;
    };

    short getIndexOfEasingType(EasingType type) {
        switch (type) {
            case EasingType::None: return 0;
            case EasingType::EaseInOut: return 1; case EasingType::EaseIn: return 2; case EasingType::EaseOut: return 3;
            case EasingType::ElasticInOut: return 4; case EasingType::ElasticIn: return 5; case EasingType::ElasticOut: return 6;
            case EasingType::BounceInOut: return 7; case EasingType::BounceIn: return 8; case EasingType::BounceOut: return 9;
            case EasingType::ExponentialInOut: return 10; case EasingType::ExponentialIn: return 11; case EasingType::ExponentialOut: return 12;
            case EasingType::SineInOut: return 13; case EasingType::SineIn: return 14; case EasingType::SineOut: return 15;
            case EasingType::BackInOut: return 16; case EasingType::BackIn: return 17; case EasingType::BackOut: return 18;
        }

        return 0;
    }
    
    CCMenu* createEasingMenu(std::optional<EasingType> initialEasingType) {
        auto menu = CCMenu::create();
        menu->setContentSize({ 134, 46 });
        auto center = ccp(67, 23);

        // auto easeVector = std::vector<std::string> {
        //     "None",
        //     "Ease In Out", "Ease In", "Ease Out", "Ease Out In",
        //     "Elastic In Out", "Elastic In", "Elastic Out",
        //     "Bounce In Out", "Bounce In", "Bounce Out",
        //     "Exponential In Out", "Exponential In", "Exponential Out",
        //     "Sine In Out", "Sine In", "Sine Out",
        //     "Back In Out", "Back In", "Back Out"
        // };

        auto easingMap = std::map<std::string, EasingType> {
            { "None", EasingType::None },
            { "Ease In Out", EasingType::EaseInOut }, { "Ease In", EasingType::EaseIn }, { "Ease Out", EasingType::EaseOut },
            { "Elastic In Out", EasingType::ElasticInOut }, { "Elastic In", EasingType::ElasticIn }, { "Elastic Out", EasingType::ElasticOut },
            { "Bounce In Out", EasingType::BounceInOut }, { "Bounce In", EasingType::BounceIn }, { "Bounce Out", EasingType::BounceOut },
            { "Exponential In Out", EasingType::ExponentialInOut }, { "Exponential In", EasingType::ExponentialIn }, { "Exponential Out", EasingType::ExponentialOut },
            { "Sine In Out", EasingType::SineInOut }, { "Sine In", EasingType::SineIn }, { "Sine Out", EasingType::SineOut },
            { "Back In Out", EasingType::BackInOut }, { "Back In", EasingType::BackIn }, { "Back Out", EasingType::BackOut }
        };

        auto titleText = "Easing:";
        auto title = CCLabelBMFont::create(titleText, "goldFont.fnt");
        title->setPosition(center + ccp(0, 24));
        title->setScale(0.64);

        auto labelText = "Bounce In"; //487.5, 106
        auto label = CCLabelBMFont::create(labelText, "bigFont.fnt");
        label->setPosition(center);
        label->limitLabelWidth(330.25, 0.56, 0.1);

        auto decArrowSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        decArrowSpr->setScale(0.56);
        auto decArrowBtn = CCMenuItemSpriteExtra::create(
            decArrowSpr, this, nullptr
        );
        decArrowBtn->setPosition(center + ccp(-80, 0));
        decArrowBtn->setTag(-1);

        auto incArrowSpr = CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
        incArrowSpr->setScale(0.56);
        incArrowSpr->setFlipX(true);
        auto incArrowBtn = CCMenuItemSpriteExtra::create(
            incArrowSpr, this, nullptr
        );
        incArrowBtn->setPosition(center + ccp(80, 0));
        incArrowBtn->setTag(1);

        menu->addChild(title);
        menu->addChild(label);
        menu->addChild(decArrowBtn);
        menu->addChild(incArrowBtn);

        menu->setTouchPriority(-505);

        return menu;
    };

    void onMixedInput(CCObject* sender) {
        auto& triggers = m_fields->m_triggers;
        auto alert = MixedInputPopup::create(triggers, [triggers](std::variant<int, float> newValue) {
            for (auto& trigger : triggers) {
                trigger.object->m_targetGroupID = std::get<int>(newValue);
            }
        });

        alert->m_noElasticity = true;
        alert->show();

        // MixedInputPopup::create(triggers, Trigger::PropType::TargetGroup)->show();
    }

    void setMenuToMixed(CCMenu* menu) {
        auto input = getChildOfType<TextInput>(menu, 0);
        if (input) {
            input->setEnabled(false);
            input->setVisible(false);

            auto spr = CCScale9Sprite::create("square02_001.png", {0, 0, 80, 80});
            spr->setContentSize(input->getBGSprite()->getContentSize());
            spr->setScale(0.4);
            spr->setOpacity(100);

            auto label = CCLabelBMFont::create("Mixed", "bigFont.fnt");
            label->setScale(1.2);
            label->setPosition(spr->getContentSize() / 2);

            spr->addChild(label);

            auto btn = CCMenuItemSpriteExtra::create(spr, menu, menu_selector(MultiEditTriggersPopup::onMixedInput));
            btn->setPosition(input->getPosition());

            menu->addChild(btn);
        };

        auto slider = getChildOfType<Slider>(menu, 1);
        if (slider) slider->setValue(100);
    }

    template<typename T>
    std::optional<T> getInitialTriggerValue(const std::vector<Trigger>& triggers, Trigger::PropType type) {
        std::variant<std::monostate, int, float, EasingType> firstResult = std::monostate{};

        for (const auto& trigger : triggers) {
            if (trigger.hasProperty(type)) {
                if (std::holds_alternative<std::monostate>(firstResult)) {
                    firstResult = trigger.getProperty(type);
                } else if (firstResult != trigger.getProperty(type)) {
                    return std::nullopt;
                }
            }
        }

        if (std::holds_alternative<std::monostate>(firstResult))
            return std::nullopt;

        return std::get<T>(firstResult);
    }

    bool init(EffectGameObject* p0, CCArray* p1) {
        if (!EditTriggersPopup::init(p0, p1)) return false;

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        // GET TRIGGERS
        auto& triggers = m_fields->m_triggers;
        auto triggerGameObjects = static_cast<CCArrayExt<EffectGameObject*>>(p1);

        std::transform(triggerGameObjects.begin(), triggerGameObjects.end(), std::back_inserter(m_fields->m_triggers), [](EffectGameObject* trigger) {
            return Trigger(trigger);
        });

        auto initialDuration = getInitialTriggerValue<float>(triggers, Trigger::PropType::Duration);
        auto initialOpacity = getInitialTriggerValue<float>(triggers, Trigger::PropType::Opacity);
        auto initialTargetGroupID = getInitialTriggerValue<int>(triggers, Trigger::PropType::TargetGroup);
        auto initialCenterGroupID = getInitialTriggerValue<int>(triggers, Trigger::PropType::CenterGroup);
        auto initialEasingType = getInitialTriggerValue<EasingType>(triggers, Trigger::PropType::Easing);
        auto initialItemID = getInitialTriggerValue<int>(triggers, Trigger::PropType::Item);

        // INCREASE ALERT SIZE

        // old alert size: { 330, 120 }
        // new alert size: { 440, 310 }

        auto bg = as<CCScale9Sprite*>(m_mainLayer->getChildren()->objectAtIndex(0));
        auto title = as<CCLabelBMFont*>(getChildren()->objectAtIndex(1));
        auto okBtn = as<CCMenuItemSpriteExtra*>(m_buttonMenu->getChildren()->objectAtIndex(0));
        auto infoBtn = as<InfoAlertButton*>(m_buttonMenu->getChildren()->objectAtIndex(1));
        
        bg->setContentSize({ 440, 310 });
        title->setPositionY(title->getPositionY() + 95);
        okBtn->setPositionY(okBtn->getPositionY() - 95);
        infoBtn->setPosition(infoBtn->getPosition() + ccp(-55, 95));

        auto touchLabel = as<CCLabelBMFont*>(m_mainLayer->getChildren()->objectAtIndex(2));
        auto touchBtn = as<CCMenuItemSpriteExtra*>(m_buttonMenu->getChildren()->objectAtIndex(2));
        auto spawnLabel = as<CCLabelBMFont*>(m_mainLayer->getChildren()->objectAtIndex(3));
        auto spawnBtn = as<CCMenuItemSpriteExtra*>(m_buttonMenu->getChildren()->objectAtIndex(3));
        auto multiLabel = as<CCLabelBMFont*>(m_mainLayer->getChildren()->objectAtIndex(4));
        auto multiBtn = as<CCMenuItemSpriteExtra*>(m_buttonMenu->getChildren()->objectAtIndex(4));

        touchLabel->setPosition(touchLabel->getPosition() + ccp(-55, -95 - 30));
        touchBtn->setPosition(touchBtn->getPosition() + ccp(-55, -95 - 30));
        spawnLabel->setPosition(spawnLabel->getPosition() + ccp(-55 + 90, -95));
        spawnBtn->setPosition(spawnBtn->getPosition() + ccp(-55 + 90, -95));
        multiLabel->setPosition(multiLabel->getPosition() + ccp(55, -95));
        multiBtn->setPosition(multiBtn->getPosition() + ccp(55, -95));

        auto infoText = "Edit attributes of the selected triggers.\n"
            "<cy>Touch trigger</c> makes the triggers activate on player touch.\n"
            "<cl>Spawn trigger</c> makes the triggers activate from a spawn trigger.\n"
            "Prefix a number with + or - to change the values rather than setting them to a fixed value.";  

        // ADD MENUS

        auto groupRow = CCMenu::create();
        groupRow->setLayout(
            RowLayout::create()
                ->setGap(20)
                ->setGrowCrossAxis(true)
        );

        auto timeRow = CCMenu::create();
        timeRow->setLayout(
            RowLayout::create()
                ->setGap(40)
                ->setGrowCrossAxis(true)
        );

        auto hasProperty = [triggers](auto predicate) {
            return std::any_of(triggers.begin(), triggers.end(), predicate);
        };

        bool hasTargetGroup = hasProperty([](const Trigger& trigger){ return trigger.hasTargetGroup; });
        bool hasCenterGroup = hasProperty([](const Trigger& trigger){ return trigger.hasCenterGroup; });
        bool hasItem = hasProperty([](const Trigger& trigger){ return trigger.hasItem; });
        bool hasDuration = hasProperty([](const Trigger& trigger){ return trigger.hasDuration; });
        bool hasOpacity = hasProperty([](const Trigger& trigger){ return trigger.hasOpacity; });
        bool hasEasing = hasProperty([](const Trigger& trigger){ return trigger.hasEasing; });

        auto targetGroupMenu = createGroupMenu(GroupMenuType::TargetGroup, initialTargetGroupID);
        auto centerGroupMenu = createGroupMenu(GroupMenuType::CenterGroup, initialCenterGroupID);
        auto itemMenu = createGroupMenu(GroupMenuType::Item, initialItemID);
        auto durationMenu = createSliderMenu(SliderMenuType::Duration, initialDuration);
        auto opacityMenu = createSliderMenu(SliderMenuType::Opacity, initialOpacity);
        auto easingMenu = createEasingMenu(initialEasingType);

        if (hasTargetGroup) groupRow->addChild(targetGroupMenu);
        if (hasCenterGroup) groupRow->addChild(centerGroupMenu);
        if (hasItem) groupRow->addChild(itemMenu);
        if (hasDuration) timeRow->addChild(durationMenu);
        if (hasOpacity) timeRow->addChild(opacityMenu);
        if (hasEasing) groupRow->addChild(easingMenu);

        if (!initialTargetGroupID.has_value()) setMenuToMixed(targetGroupMenu);
        if (!initialCenterGroupID.has_value()) setMenuToMixed(centerGroupMenu);
        if (!initialItemID.has_value()) setMenuToMixed(itemMenu);
        if (!initialDuration.has_value()) setMenuToMixed(durationMenu);
        if (!initialOpacity.has_value()) setMenuToMixed(opacityMenu);
        if (!initialEasingType.has_value()) setMenuToMixed(easingMenu);

        groupRow->setPosition(winSize / 2 + ccp(0, 80));
        groupRow->updateLayout();
        this->addChild(groupRow);

        timeRow->setPosition(winSize / 2 + ccp(0, 20));
        timeRow->updateLayout();
        this->addChild(timeRow);

        return true;
    }
};