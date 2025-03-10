/*
  ==============================================================================

	ChainViz.cpp
	Created: 20 Oct 2020 4:34:50pm
	Author:  bkupe

  ==============================================================================
*/

#include "ChainViz.h"
#include "Effect/EffectIncludes.h"
#include "Object/ObjectIncludes.h"
#include "Sequence/SequenceIncludes.h"
#include "Scene/SceneIncludes.h"

const String ChainViz::panelName = "Effect Chain Viz";
const Colour ChainVizComponent::typeColors[ChainVizTarget::CHAINVIZ_TYPE_MAX]{ PANEL_COLOR, PANEL_COLOR, Colours::purple.brighter(), BLUE_COLOR, GREEN_COLOR, YELLOW_COLOR, RED_COLOR };

ChainViz::ChainViz(const String& name) :
	ShapeShifterContentComponent(name),
	currentObject(nullptr),
	resizing(false),
	componentSelect("Component", "Select the component to show in the chain viz"),
	showOnlyActives("Show Only Actives", "Only show effect that are currently affecting. This hides disabled effects and effects with 0 weight", false)
{
	viewport.setViewedComponent(&container, false);
	viewport.setScrollBarsShown(false, true, false, false);
	viewport.setScrollBarThickness(16);
	addAndMakeVisible(&viewport);

	componentSelect.addAsyncParameterListener(this);
	for (int i = 0; i < ComponentType::TYPES_MAX; i++) componentSelect.addOption(componentTypeNames[i], (ComponentType)i);

	componentSelectUI.reset(componentSelect.createUI());
	addAndMakeVisible(componentSelectUI.get());

	showOnlyActives.addAsyncParameterListener(this);
	showOnlyActivesUI.reset(showOnlyActives.createButtonToggle());
	addAndMakeVisible(showOnlyActivesUI.get());

	Engine::mainEngine->addAsyncContainerListener(this);
	SceneManager::getInstance()->addAsyncSceneManagerListener(this);
}

ChainViz::~ChainViz()
{
	if (SceneManager::getInstanceWithoutCreating()) SceneManager::getInstance()->removeAsyncSceneManagerListener(this);
	Engine::mainEngine->removeAsyncContainerListener(this);
	setCurrentObject(nullptr);
}

void ChainViz::clear()
{
	if (objectStartVizComponent != nullptr) removeVizComponent(objectStartVizComponent.get());
	if (objectEndVizComponent != nullptr) removeVizComponent(objectEndVizComponent.get());
	for (auto& c : objectEffectsVizComponents) removeVizComponent(c);
	for (auto& c : sceneVizComponents) removeVizComponent(c);
	for (auto& c : sequenceVizComponents) removeVizComponent(c);
	for (auto& c : groupVizComponents) removeVizComponent(c);
	for (auto& c : globalEffectsVizComponents) removeVizComponent(c);

	objectStartVizComponent.reset();
	objectEndVizComponent.reset();
	objectEffectsVizComponents.clear();
	sceneVizComponents.clear();
	sequenceVizComponents.clear();
	groupVizComponents.clear();
	globalEffectsVizComponents.clear();
}

void ChainViz::setCurrentObject(Object* o)
{
	if (o == currentObject) return;

	if (currentObject != nullptr && !objectRef.wasObjectDeleted())
	{
		currentObject->removeInspectableListener(this);
	}

	clear();

	currentObject = o;
	objectRef = (Inspectable*)o;

	if (currentObject != nullptr)
	{
		currentObject->addInspectableListener(this);
	}

	buildChain();
}

void ChainViz::buildChain()
{
	if (currentObject == nullptr || objectRef.wasObjectDeleted()) return;

	ComponentType t = componentSelect.getValueDataAsEnum<ComponentType>();

	objectStartVizComponent.reset(currentObject->createVizComponent(currentObject, t, ChainVizTarget::OBJECT_START));
	container.addAndMakeVisible(objectStartVizComponent.get());


	Array<ChainVizTarget*> objectEffects = currentObject->effectManager->getChainVizTargetsForObjectAndComponent(currentObject, t);
	Array<ChainVizTarget*> sceneEffects = SceneManager::getInstance()->getChainVizTargetsForObjectAndComponent(currentObject, t);
	Array<ChainVizTarget*> sequenceEffects = GlobalSequenceManager::getInstance()->getChainVizTargetsForObjectAndComponent(currentObject, t);
	Array<ChainVizTarget*> groupEffects = GroupManager::getInstance()->getChainVizTargetsForObjectAndComponent(currentObject, t);
	Array<ChainVizTarget*> globalEffects = GlobalEffectManager::getInstance()->getChainVizTargetsForObjectAndComponent(currentObject, t);

	rebuildTargetVizComponents(objectEffects, &objectEffectsVizComponents, ChainVizTarget::OBJECT_EFFECT);
	rebuildTargetVizComponents(sceneEffects, &sceneVizComponents, ChainVizTarget::SCENE_EFFECT);
	rebuildTargetVizComponents(sequenceEffects, &sequenceVizComponents, ChainVizTarget::SEQUENCE_EFFECT);
	rebuildTargetVizComponents(groupEffects, &groupVizComponents, ChainVizTarget::GROUP_EFFECT);
	rebuildTargetVizComponents(globalEffects, &globalEffectsVizComponents, ChainVizTarget::GLOBAL_EFFECT);

	objectEndVizComponent.reset(currentObject->createVizComponent(currentObject, t, ChainVizTarget::OBJECT_END));
	container.addAndMakeVisible(objectEndVizComponent.get());

	resized();
	repaint();
}


void ChainViz::rebuildTargetVizComponents(Array<ChainVizTarget*> effectsToAdd, OwnedArray<ChainVizComponent>* arrayToAdd, ChainVizTarget::ChainVizType type)
{
	for (auto& c : *arrayToAdd) removeVizComponent(c);
	arrayToAdd->clear();

	ComponentType t = componentSelect.getValueDataAsEnum<ComponentType>();

	for (auto& e : effectsToAdd)
	{
		ChainVizComponent* ec = e->createVizComponent(currentObject, t, type);
		container.addAndMakeVisible(ec);
		ec->addComponentListener(this);
		arrayToAdd->add(ec);
	}
}

void ChainViz::removeVizComponent(ChainVizComponent* c)
{
	c->removeComponentListener(this);
	removeChildComponent(c);
}

void ChainViz::paint(Graphics& g)
{
	ShapeShifterContentComponent::paint(g);

	if (currentObject == nullptr)
	{
		g.setColour(TEXT_COLOR.darker());
		g.setFont(20);
		g.drawFittedText("Double click on an object to see it's effect chain.", getLocalBounds().reduced(5), Justification::centred, 4);
		return;
	}

	if (!objectEffectsRect.isEmpty())
	{
		g.setColour(ChainVizComponent::typeColors[ChainVizTarget::OBJECT_EFFECT]);
		g.drawRoundedRectangle(getLocalArea(&container, objectEffectsRect.toFloat().expanded(5)), 4, 1);
	}

	if (!sceneRect.isEmpty())
	{
		g.setColour(ChainVizComponent::typeColors[ChainVizTarget::SCENE_EFFECT]);
		g.drawRoundedRectangle(getLocalArea(&container, sceneRect.toFloat().expanded(5)), 4, 1);
	}

	if (!sequenceRect.isEmpty())
	{
		g.setColour(ChainVizComponent::typeColors[ChainVizTarget::SEQUENCE_EFFECT]);
		g.drawRoundedRectangle(getLocalArea(&container, sequenceRect.toFloat().expanded(5)), 4, 1);
	}

	if (!groupRect.isEmpty())
	{
		g.setColour(ChainVizComponent::typeColors[ChainVizTarget::GROUP_EFFECT]);
		g.drawRoundedRectangle(getLocalArea(&container, groupRect.toFloat().expanded(5)), 4, 1);
	}

	if (!globalEffectsRect.isEmpty())
	{
		g.setColour(ChainVizComponent::typeColors[ChainVizTarget::GLOBAL_EFFECT]);
		g.drawRoundedRectangle(getLocalArea(&container, globalEffectsRect.toFloat().expanded(5)), 4, 1);
	}
}

void ChainViz::resized()
{
	if (objectStartVizComponent == nullptr || objectEndVizComponent == nullptr) return;

	resizing = true;

	Rectangle<int> r = getLocalBounds();

	Rectangle<int> hr = r.removeFromTop(24);
	componentSelectUI->setBounds(hr.removeFromLeft(150).reduced(2));
	r.removeFromLeft(2);
	showOnlyActivesUI->setBounds(hr.removeFromLeft(100).reduced(2));
	r.reduce(2, 2);
	r.removeFromBottom(20);

	r.setWidth(objectStartVizComponent->getWidth());
	objectStartVizComponent->setBounds(r);
	r.setX(r.getRight() + 20);

	r.setWidth(0);

	const int gap = 20;

	objectEffectsRect = placeVizComponents(&objectEffectsVizComponents, r);
	if (!objectEffectsRect.isEmpty()) r.translate(gap, 0);
	sceneRect = placeVizComponents(&sceneVizComponents, r);
	if (!sceneRect.isEmpty()) r.translate(gap, 0);
	sequenceRect = placeVizComponents(&sequenceVizComponents, r);
	if (!sequenceRect.isEmpty()) r.translate(gap, 0);
	groupRect = placeVizComponents(&groupVizComponents, r);
	if (!groupRect.isEmpty()) r.translate(gap, 0);
	globalEffectsRect = placeVizComponents(&globalEffectsVizComponents, r);
	if (!globalEffectsRect.isEmpty()) r.translate(gap, 0);

	r.setWidth(objectEndVizComponent->getWidth());
	objectEndVizComponent->setBounds(r);
	r.setX(r.getRight() + 20);

	container.setSize(r.getX(), getHeight() - viewport.getScrollBarThickness());
	viewport.setBounds(getLocalBounds());

	resizing = false;
}

Rectangle<int> ChainViz::placeVizComponents(OwnedArray<ChainVizComponent>* components, Rectangle<int>& r)
{
	Rectangle<int> result(r);
	for (auto& c : *components)
	{
		c->setVisible(!showOnlyActives.boolValue() || c->isReallyAffecting());

		if (!c->isVisible()) continue;

		int cw = c->getWidth();
		c->setBounds(r.withWidth(cw));
		r.translate(cw + 10, 0);
	}

	result.setRight(r.getX() - 10);

	return result;
}

void ChainViz::newMessage(const SceneManagerEvent& e)
{
	if (currentObject == nullptr || objectRef.wasObjectDeleted()) return;

	if (e.type == e.SCENE_LOAD_START)
	{
		ComponentType t = componentSelect.getValueDataAsEnum<ComponentType>();

		Array<ChainVizTarget*> sceneEffects = SceneManager::getInstance()->getChainVizTargetsForObjectAndComponent(currentObject, t);
		rebuildTargetVizComponents(sceneEffects, &sceneVizComponents, ChainVizTarget::OBJECT_EFFECT);
		resized();
		repaint();
	}
}

void ChainViz::newMessage(const ContainerAsyncEvent& e)
{
	if (e.type == ContainerAsyncEvent::ChildStructureChanged) buildChain();
}

void ChainViz::newMessage(const Parameter::ParameterEvent& e)
{
	if (e.parameter == &showOnlyActives)
	{
		resized();
	}
	else if (e.parameter == &componentSelect)
	{
		buildChain();
	}
}

void ChainViz::inspectableDestroyed(Inspectable* i)
{
	if (i == objectRef || objectRef.wasObjectDeleted()) setCurrentObject(nullptr);
}

void ChainViz::componentMovedOrResized(Component& c, bool wasMoved, bool wasResized)
{
	if (!resizing)
	{
		resized();
		repaint();
	}
}

ChainVizComponent::ChainVizComponent(ChainVizTarget* item, Object* o, ComponentType ct, ChainVizTarget::ChainVizType type) :
	item(item),
	object(o),
	componentType(ct),
	type(type),
	transparentBG(false)
{
	object->addInspectableListener(this);
	setSize(200, 100);
}

ChainVizComponent::~ChainVizComponent()
{
	if (object != nullptr) object->removeInspectableListener(this);
}


void ChainVizComponent::paint(Graphics& g)
{
	if (item == nullptr) return;

	if (!transparentBG)
	{
		g.setColour(isReallyAffecting() ? PANEL_COLOR.darker() : BG_COLOR.brighter(.2f));
		g.fillRoundedRectangle(getLocalBounds().toFloat(), 4);
		g.setColour(typeColors[(int)type]);
		g.drawRoundedRectangle(getLocalBounds().toFloat(), 4, 1);
	}
}

void ChainVizComponent::inspectableDestroyed(Inspectable* i)
{
	if (i == object) object = nullptr;
}

BaseItemChainVizComponent::BaseItemChainVizComponent(BaseItem* i, Object* o, ComponentType ct, ChainVizTarget::ChainVizType type) :
	ChainVizComponent((ChainVizTarget*)i, o, ct, type),
	baseItem(i),
	showItemName(true)
{
	if (baseItem->canBeDisabled)
	{
		enableUI.reset(baseItem->enabled->createToggle(AssetManager::getInstance()->powerOn, AssetManager::getInstance()->powerOff));
		addAndMakeVisible(enableUI.get());
	}

	baseItem->addInspectableListener(this);
}

BaseItemChainVizComponent::~BaseItemChainVizComponent()
{
	if (baseItem != nullptr) baseItem->removeInspectableListener(this);
}

void BaseItemChainVizComponent::mouseDown(const MouseEvent& e)
{
	if (e.eventComponent == this)
	{
		baseItem->selectThis();
	}
}

void BaseItemChainVizComponent::paint(Graphics& g)
{
	if (baseItem == nullptr) return;

	if (!transparentBG)
	{
		g.setColour(isReallyAffecting() ? PANEL_COLOR : PANEL_COLOR.darker());
		g.fillRoundedRectangle(getLocalBounds().toFloat(), 4);
	}

	if (showItemName)
	{
		g.setColour(TEXT_COLOR);
		g.setFont(16);
		g.drawText(getVizLabel(), getLocalBounds().toFloat(), Justification::centred);
	}

	if (baseItem->isSelected)
	{
		g.setColour(HIGHLIGHT_COLOR);
		g.drawRoundedRectangle(getLocalBounds().toFloat(), 4, 1);
	}
}

void BaseItemChainVizComponent::resized()
{
	if (enableUI != nullptr)
	{
		enableUI->setBounds(getLocalBounds().reduced(4).withSize(16, 16));
	}
}

void BaseItemChainVizComponent::inspectableSelectionChanged(Inspectable* i)
{
	repaint();
}

void BaseItemChainVizComponent::inspectableDestroyed(Inspectable* i)
{
	ChainVizComponent::inspectableDestroyed(i);
	if (i == baseItem) baseItem = nullptr;
}
