#pragma once

#include <Geode/Geode.hpp>

using namespace geode::prelude;

enum RoundingType {
    Round, Floor, Ceiling
};

struct MixedInputSettings {
    RoundingType rounding;
};

class MixedInputPopup : public Popup<const CCArrayExt<EffectGameObject*>&, const short&, const std::function<void (std::optional<float>)>&> {
protected:
    enum Operator {
        Add, Subtract, Multiply, Divide, Equal
    };

    enum DirectionType {
        None, Up, Down, Left, Right
    };
    
    CCArrayExt<EffectGameObject*> m_triggers;
    std::function<void (std::optional<float>)> m_callback;
    short m_property;
    Operator m_operator;
    float m_modifierValue;
    float m_initialValue;
    short m_decimalPlaces;
    bool m_canBeNegative;
    RoundingType m_rounding;
    DirectionType m_direction;
    bool m_isFirstPage;

    CCMenuItemToggler* m_operatorBtn;
    CCMenuItemToggler* m_directionBtn;
    ScrollLayer* m_scroll;
    CCMenu* m_pageMenu;

    struct CalculationInfo {
        std::string propertyString;
        std::string changeString;
        std::string newPropertyString;
        CCArrayExt<EffectGameObject*> triggers;

        CalculationInfo(const std::string& propStr, const std::string& changeStr, const std::string& newPropStr, const CCArrayExt<EffectGameObject*>& trig)
            : propertyString(propStr), changeString(changeStr), newPropertyString(newPropStr), triggers(trig) {}
    };

    bool setup(const CCArrayExt<EffectGameObject*>&, const short&, const std::function<void (std::optional<float>)>&) override;

    void createFirstPageRow();
    void createSecondPageRow();
    void createScrollLayer(bool);
    void onOperator(CCObject*);
    void onDirection(CCObject*);
    void onValueArrow(CCObject*);
    void onChangePage(CCObject*);
    void onSettings(CCObject*);
    void onApply(CCObject*);

    std::string toTruncatedString(float);
    float applyOperation(float, float, Operator);
    std::vector<MixedInputPopup::CalculationInfo> createStringMap();
public:
    static MixedInputPopup* create(const CCArrayExt<EffectGameObject*>&, const short&, const std::function<void (std::optional<float>)>&);
};

class SettingsPopup : public Popup<MixedInputSettings, std::function<void(MixedInputSettings)>> {
protected:
    MixedInputSettings m_settings;
    std::function<void(MixedInputSettings)> m_callback;
    CCMenuItemToggler* m_roundingBtn;

    bool setup(MixedInputSettings, std::function<void(MixedInputSettings)>) override;
    void onRoundingButton(CCObject*);
public:
    static SettingsPopup* create(MixedInputSettings, std::function<void(MixedInputSettings)>);
};