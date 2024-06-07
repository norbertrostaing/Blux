#include "Main.h"
#include "MainIncludes.h"

BluxApplication::BluxApplication() : 
    OrganicApplication(ProjectInfo::projectName, 
        true, 
        BluxAssetManager::getImage("icon3"))
{
}

void BluxApplication::initialiseInternal(const String&)
{
    engine.reset(new BluxEngine());
	mainComponent.reset(new MainComponent());


	//Call after engine init
	AppUpdater::getInstance()->setURLs("http://benjamin.kuperberg.fr/blux/releases/update.json", "http://benjamin.kuperberg.fr/blux/download/app/", "Blux");
	HelpBox::getInstance()->helpURL = URL("http://benjamin.kuperberg.fr/blux/help/");
	CrashDumpUploader::getInstance()->init("http://benjamin.kuperberg.fr/blux/support/crash_report.php", ImageCache::getFromMemory(BinaryData::crash_png, BinaryData::crash_pngSize));

	DashboardManager::getInstance()->setupDownloadURL("http://benjamin.kuperberg.fr/download/dashboard/dashboard.php");

	ShapeShifterManager::getInstance()->setDefaultFileData(BinaryData::default_bluxlayout);
	ShapeShifterManager::getInstance()->setLayoutInformations("bluxlayout", "Blux/layouts");
}
