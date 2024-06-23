#include "MixedInputPopup.hpp"
#include <Geode/ui/TextInput.hpp>
#include <Geode/binding/ButtonSprite.hpp>

#include <Geode/Geode.hpp>
using namespace geode::prelude;

#include <vector>

bool MixedInputPopup::setup(const std::vector<Trigger>& triggers, std::function<void(std::variant<int, float>)> setProperty) {
    m_triggers = triggers;
    m_setProperty = setProperty;
    
    auto winSize = m_mainLayer->getContentSize();

    // INFO BUTTON

    auto infoText = "Modify the <cg>mixed value</c> of the <cy>selected input</c>.\n"
        "Use the <cl>equals button</c> to override the old value. Use the <cp>operator buttons</c> to add, subtract, multiply, or divide the old value.\n";
    
    auto infoBtn = InfoAlertButton::create("Info", infoText, 0.7f);
    infoBtn->setPosition(winSize - ccp(30, 30));
    m_buttonMenu->addChild(infoBtn);

    // OPERATOR BUTTONS AND INPUT ROW

    auto bottomLayout = CCMenu::create();
    bottomLayout->setLayout(
        RowLayout::create()
            ->setGap(5.f)
    );
    bottomLayout->setPosition({winSize.width / 2, 70});
    
    bottomLayout->addChild(createOperatorButton(OperatorType::Add));
    bottomLayout->addChild(createOperatorButton(OperatorType::Subtract));
    bottomLayout->addChild(createOperatorButton(OperatorType::Multiply));
    bottomLayout->addChild(createOperatorButton(OperatorType::Divide));
    bottomLayout->addChild(createOperatorButton(OperatorType::Equal));

    m_buttonMenu->addChild(bottomLayout);
    bottomLayout->updateLayout();

    // APPLY BUTTON

    auto applySpr = ButtonSprite::create("Apply", "goldFont.fnt", "GJ_button_01.png", .9f);
    auto applyBtn = CCMenuItemSpriteExtra::create(
        applySpr, this, nullptr
    );
    applyBtn->setPosition({winSize.width / 2, 30});
    m_buttonMenu->addChild(applyBtn);

    return true;
}

CCMenuItemSpriteExtra* MixedInputPopup::createOperatorButton(const OperatorType& sign) {
    auto signStr = sign == OperatorType::Add ? "+" :
        sign == OperatorType::Subtract ? "-" :
        sign == OperatorType::Multiply ? "*" :
        sign == OperatorType::Divide ? "/" : "=";
    
    // auto spr = CCScale9Sprite::create("GJ_button_04.png", {0, 0, 80, 80});
    // auto selectedSpr = CCScale9Sprite::create("GJ_button_02.png", {0, 0, 80, 80});
    // spr->setContentSize({50, 50});

    // auto label = CCLabelBMFont::create(signStr, "bigFont.fnt");
    // label->setPosition(spr->getContentSize() / 2);

    // spr->addChild(label);

    auto unselectedSpr = ButtonSprite::create(signStr, "gjFont59.fnt", "GJ_colorBtn_001.png", .7f);
    unselectedSpr->setContentSize({30, 30});
    unselectedSpr->setOpacity(100);
    unselectedSpr->setColor({153, 85, 51});

    auto selectedSpr = ButtonSprite::create(signStr, "gjFont59.fnt", "GJ_colorBtn_001.png", .7f);
    selectedSpr->setContentSize({30, 30});
    selectedSpr->setOpacity(100);
    selectedSpr->setColor({0, 255, 0});

    auto btn = CCMenuItemSpriteExtra::create(
        unselectedSpr, selectedSpr, this, menu_selector(MixedInputPopup::onOperator)
    );

    if (sign == OperatorType::Add) {
        btn->selected();
        m_operatorBtn = btn;
    }

    return btn;
}

void MixedInputPopup::onOperator(CCObject* sender) {
    auto btn = static_cast<CCMenuItemSpriteExtra*>(sender);

    btn->selected();
    m_operatorBtn->unselected();
    m_operatorBtn = btn;
    
    auto sign = static_cast<OperatorType>(btn->getTag());
    m_operator = sign;
}

MixedInputPopup* MixedInputPopup::create(const std::vector<Trigger>& triggers, std::function<void(std::variant<int, float>)> setProperty) {
    auto ret = new MixedInputPopup();
    if (ret && ret->initAnchored(300.f, 200.f, triggers, setProperty)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}