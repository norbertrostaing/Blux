/*
  ==============================================================================

    PixelShapeChooser.cpp
    Created: 8 Nov 2020 10:06:49am
    Author:  bkupe

  ==============================================================================
*/

PixelShapeChooser::PixelShapeChooser() :
    TextButton("No Shape")
{
}

PixelShapeChooser::~PixelShapeChooser()
{
}

void PixelShapeChooser::clicked()
{
    PopupMenu m;

    const int numTypes = 3;
    const String typeNames[numTypes]{"Point", "Line", "Circle" };
    for (int i = 0; i < numTypes; i++) m.addItem(i + 1, typeNames[i]);

    m.showMenuAsync(PopupMenu::Options(), [this, typeNames](int result)
        {
            if (result == 0) return;
            this->chooserListeners.call(&ChooserListener::shapeChosen, typeNames[result - 1]);
        }
    );
}
