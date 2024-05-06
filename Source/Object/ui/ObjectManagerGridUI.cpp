/*
  ==============================================================================

    ObjectManagerGridUI.cpp
    Created: 26 Sep 2020 7:51:37pm
    Author:  bkupe

  ==============================================================================
*/

#include "Object/ObjectIncludes.h"
#include "Scene/SceneIncludes.h"

ObjectManagerGridUI::ObjectManagerGridUI(const String& name) :
	BaseManagerShapeShifterUI(name, ObjectManager::getInstance())
{
	highlightOnDragOver = false;

	contentIsFlexible = true;
	animateItemOnAdd = false;

	thumbSizeUI.reset(manager->gridThumbSize->createSlider());
	addAndMakeVisible(thumbSizeUI.get());
	thumbSizeUI->useCustomBGColor = true;
	thumbSizeUI->customBGColor = BG_COLOR.darker(.2f);

	flashValueUI.reset(manager->defaultFlashValue->createSlider());
	addAndMakeVisible(flashValueUI.get());
	flashValueUI->useCustomBGColor = true;
	flashValueUI->customBGColor = BG_COLOR.darker(.2f);

	blackOutUI.reset(manager->blackOut->createButtonToggle());
	addAndMakeVisible(blackOutUI.get());
	activeInSceneUI.reset(manager->filterActiveInScene->createButtonToggle());
	addAndMakeVisible(activeInSceneUI.get());

	manager->addAsyncCoalescedContainerListener(this);

	setShowSearchBar(true);

	addExistingItems();

	SceneManager::getInstance()->addAsyncSceneManagerListener(this);
}

ObjectManagerGridUI::~ObjectManagerGridUI()
{
	if(!inspectable.wasObjectDeleted()) manager->removeAsyncContainerListener(this);
	if(SceneManager::getInstanceWithoutCreating()) SceneManager::getInstance()->removeAsyncSceneManagerListener(this);
}

void ObjectManagerGridUI::paint(Graphics& g)
{
	BaseManagerUI::paint(g);

	if (isDraggingOver)
	{
		g.setColour(BLUE_COLOR);

		if (ObjectGridUI* bui = itemsUI[currentDropIndex >= 0 ? currentDropIndex : itemsUI.size() - 1])
		{
			juce::Rectangle<int> buiBounds = getLocalArea(bui, bui->getLocalBounds());
			int tx = currentDropIndex >= 0 ? buiBounds.getX() - 1 : buiBounds.getRight() + 1;
			g.drawLine(tx, buiBounds.getY(), tx, buiBounds.getBottom(), 2);
		}
	}
}

void ObjectManagerGridUI::resizedInternalHeader(Rectangle<int>& r)
{
	BaseManagerShapeShifterUI::resizedInternalHeader(r);
	r.removeFromLeft(4);
	activeInSceneUI->setBounds(r.removeFromLeft(100).reduced(3)); 
	r.removeFromLeft(16);
	thumbSizeUI->setBounds(r.removeFromLeft(150).reduced(3));
	r.removeFromLeft(16);
	flashValueUI->setBounds(r.removeFromLeft(150).reduced(3));
	r.removeFromLeft(4);
	blackOutUI->setBounds(r.removeFromLeft(100).reduced(3));
}

void ObjectManagerGridUI::resizedInternalContent(Rectangle<int>& r)
{
	viewport.setBounds(r); 
	
	const int thumbSize = manager->gridThumbSize->floatValue();

	int numThumbs = getFilteredItems().size();
	int numThumbPerLine = jmax(1, jmin(r.getWidth() / (thumbSize + gap), numThumbs));
	int numLines = numThumbs == 0 ? 0 : ceil(numThumbs * 1.f / numThumbPerLine);
	
	Rectangle<int> cr;
	cr.setSize(r.getWidth(), numLines * (thumbSize + gap) - gap);
	container.setSize(cr.getWidth(), cr.getHeight());

	int index = 0;
	int yIndex = 0;

	Rectangle<int> lr;

	for (auto& mui : itemsUI)
	{
		if (!checkFilterForItem(mui))
		{
			mui->setVisible(false);
			continue;
		}

		if (index % numThumbPerLine == 0)
		{
			int numThumbsInThisLine = jmin(numThumbs - index, numThumbPerLine);
			int lineWidth = numThumbsInThisLine * (thumbSize + gap) - gap;

			if (yIndex > 0) cr.removeFromTop(gap);
			lr = cr.removeFromTop(thumbSize);
			lr = lr.withSizeKeepingCentre(lineWidth, lr.getHeight());

			yIndex++;
		}

		mui->setBounds(lr.removeFromLeft(thumbSize));
		lr.removeFromLeft(gap);
		index++;
	}


}


void ObjectManagerGridUI::setPreviewData(var data)
{
	for (auto& i : itemsUI)
	{
		i->setPreviewData(data.getProperty(i->item->shortName, var()));
	}
}

bool ObjectManagerGridUI::hasFiltering()
{
	if (BaseManagerUI::hasFiltering()) return true;
	return manager->filterActiveInScene->boolValue();
}

bool ObjectManagerGridUI::checkFilterForItem(ObjectGridUI* ui)
{
	if (!BaseManagerShapeShifterUI::checkFilterForItem(ui)) return false;


	if (manager->filterActiveInScene->boolValue() && SceneManager::getInstance()->currentScene != nullptr)
	{
		if (!SceneManager::getInstance()->currentScene->isObjectActiveInScene(ui->item)) return false;
	}

	return true;
	
}


void ObjectManagerGridUI::newMessage(const ContainerAsyncEvent& e)
{
	switch (e.type)
	{
	case ContainerAsyncEvent::ControllableFeedbackUpdate:
		if (e.targetControllable == manager->gridThumbSize || e.targetControllable == manager->filterActiveInScene) resized();
		else if(e.targetControllable == manager->showIconForColor) for (auto& i : itemsUI) i->updateUI();
		break;
        default:
            break;
	}
}
void ObjectManagerGridUI::newMessage(const SceneManagerEvent& e)
{
	if (e.type == e.SCENE_LOAD_START || e.type == e.SCENE_LOAD_END) resized();
}

int ObjectManagerGridUI::getDropIndexForPosition(Point<int> localPosition)
{
	ObjectGridUI* closestUI = nullptr;;
	float minDist = INT32_MAX;;
	bool rightSide = false;

	for (int i = 0; i < itemsUI.size(); ++i)
	{
		ObjectGridUI * iui = itemsUI[i];
		Point<int> p = getLocalArea(iui, iui->getLocalBounds()).getCentre();
		
		float dist = p.getDistanceFrom(localPosition);
		if (dist < minDist)
		{
			closestUI = iui;
			minDist = dist;
			rightSide = localPosition.x > p.x;
		}
	}


	int index = itemsUI.indexOf(closestUI);
	if (rightSide) index++;

	return index;
}
