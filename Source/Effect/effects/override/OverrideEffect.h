/*
  ==============================================================================

    OverrideEffect.h
    Created: 1 Oct 2020 12:31:17pm
    Author:  bkupe

  ==============================================================================
*/

#pragma once

class OverrideEffect :
    public Effect
{
public:
    OverrideEffect(const String &name = "Override", var params = var());
    virtual ~OverrideEffect();
};

class OverrideFloatEffect :
    public OverrideEffect
{
public:
    OverrideFloatEffect(var params = var());
    virtual ~OverrideFloatEffect();

    FloatParameter* value;
    
    void processComponentInternal(Object* o, ObjectComponent* c, const HashMap<Parameter*, var>& values, HashMap<Parameter*, var>& targetValues, int id, float time = -1) override;

    String getTypeString() const override { return getTypeStringStatic(); }
    const static String getTypeStringStatic() { return "Override (Number)"; }
    static OverrideFloatEffect* create(var params) { return new OverrideFloatEffect(params); }
};
