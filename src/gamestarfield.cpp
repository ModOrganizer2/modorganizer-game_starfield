#include "gamestarfield.h"

#include "starfielddataarchives.h"
#include "starfieldscriptextender.h"
#include "starfieldunmanagedmods.h"
#include "starfieldmoddatachecker.h"
#include "starfieldmoddatacontent.h"
#include "starfieldgameplugins.h"
#include "starfieldsavegame.h"
#include "starfieldbsainvalidation.h"

#include <pluginsetting.h>
#include <executableinfo.h>
#include <gamebryolocalsavegames.h>
#include <gamebryosavegameinfo.h>
#include "versioninfo.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>

#include <memory>

#include "scopeguard.h"
#include "vdf_parser.h"

using namespace MOBase;

GameStarfield::GameStarfield()
{
}

bool GameStarfield::init(IOrganizer *moInfo)
{
  if (!GameGamebryo::init(moInfo)) {
    return false;
  }

  registerFeature<ScriptExtender>(new StarfieldScriptExtender(this));
  registerFeature<DataArchives>(new StarfieldDataArchives(myGamesPath()));
  registerFeature<LocalSavegames>(new GamebryoLocalSavegames(myGamesPath(), "StarfieldCustom.ini"));
  registerFeature<ModDataChecker>(new StarfieldModDataChecker(this));
  registerFeature<ModDataContent>(new StarfieldModDataContent(this));
  registerFeature<SaveGameInfo>(new GamebryoSaveGameInfo(this));
  registerFeature<GamePlugins>(new StarfieldGamePlugins(moInfo));
  registerFeature<UnmanagedMods>(new StarfieldUnmangedMods(this));
  registerFeature<BSAInvalidation>(new StarfieldBSAInvalidation(feature<DataArchives>(), this));

  return true;
}

QString GameStarfield::gameName() const
{
  return "Starfield";
}

void GameStarfield::detectGame()
{
  m_GamePath = identifyGamePath();
  m_MyGamesPath = determineMyGamesPath("Starfield");
}

QString GameStarfield::identifyGamePath() const
{
  QString path = "Software\\Valve\\Steam";
  QString steamLocation = findInRegistry(HKEY_CURRENT_USER, path.toStdWString().c_str(), L"SteamPath");
  if (!steamLocation.isEmpty()) {
    QString steamLibraryLocation;
    QString steamLibraries(steamLocation + "\\" + "config" + "\\" + "libraryfolders.vdf");
    if (QFile(steamLibraries).exists()) {
      std::ifstream file(steamLibraries.toStdString());
      auto root = tyti::vdf::read(file);
      for (auto child : root.childs) {
        tyti::vdf::object *library = child.second.get();
        auto apps = library->childs["apps"];
        if (apps->attribs.contains(steamAPPId().toStdString())) {
          steamLibraryLocation = QString::fromStdString(library->attribs["path"]);
          break;
        }
      }
    }
    if (!steamLibraryLocation.isEmpty()) {
      QString gameLocation = steamLibraryLocation + "\\" + "steamapps" + "\\" + "common" + "\\" + "Starfield";
      if (QDir(gameLocation).exists() && QFile(gameLocation + "\\" + "Starfield.exe").exists())
        return gameLocation;
    }
  }
  return "";
}

QDir GameStarfield::dataDirectory() const
{
  return documentsDirectory().absoluteFilePath("Data");
}

QMap<QString, QDir> GameStarfield::secondaryDataDirectories() const
{
  QMap<QString, QDir> directories;
  directories.insert("game_data", gameDirectory().absoluteFilePath("Data"));
  return directories;
}

QList<ExecutableInfo> GameStarfield::executables() const
{
  return QList<ExecutableInfo>()
    << ExecutableInfo("SFSE", findInGameFolder(feature<ScriptExtender>()->loaderName()))
    << ExecutableInfo("Starfield", findInGameFolder(binaryName()))
       ;
}

QList<ExecutableForcedLoadSetting> GameStarfield::executableForcedLoads() const
{
  return QList<ExecutableForcedLoadSetting>();
}

QString GameStarfield::name() const
{
  return "Starfield Support Plugin";
}

QString GameStarfield::localizedName() const
{
  return tr("Starfield Support Plugin");
}


QString GameStarfield::author() const
{
  return "Silarn";
}

QString GameStarfield::description() const
{
  return tr("Adds support for the game Starfield.");
}

MOBase::VersionInfo GameStarfield::version() const
{
  return VersionInfo(0, 0, 1, VersionInfo::RELEASE_PREALPHA);
}

QList<PluginSetting> GameStarfield::settings() const
{
  return QList<PluginSetting>();
}

void GameStarfield::initializeProfile(const QDir &path, ProfileSettings settings) const
{
  if (settings.testFlag(IPluginGame::MODS)) {
    copyToProfile(localAppFolder() + "/Starfield", path, "plugins.txt");
  }

  if (settings.testFlag(IPluginGame::CONFIGURATION)) {
    if (settings.testFlag(IPluginGame::PREFER_DEFAULTS)
        || !QFileInfo(myGamesPath() + "/Starfield.ini").exists()) {
      copyToProfile(gameDirectory().absolutePath(), path, "StarfieldDefault.ini", "Starfield.ini");
    } else {
      copyToProfile(myGamesPath(), path, "Starfield.ini");
    }

    copyToProfile(myGamesPath(), path, "StarfieldPrefs.ini");
    copyToProfile(myGamesPath(), path, "StarfieldCustom.ini");
  }
}

QString GameStarfield::savegameExtension() const
{
  return "sfs";
}

QString GameStarfield::savegameSEExtension() const
{
  return "sfse";
}

std::shared_ptr<const GamebryoSaveGame> GameStarfield::makeSaveGame(QString filePath) const
{
  return std::make_shared<const StarfieldSaveGame>(filePath, this);
}

QString GameStarfield::steamAPPId() const
{
  return "1716740";
}

QStringList GameStarfield::primaryPlugins() const {
  QStringList plugins = { "Starfield.esm", "BlueprintShips-Starfield.esm", "Constellation.esm", "OldMars.esm" };

  plugins.append(CCPlugins());

  return plugins;
}

QStringList GameStarfield::gameVariants() const
{
  return { "Regular" };
}

QString GameStarfield::gameShortName() const
{
  return "Starfield";
}

QString GameStarfield::gameNexusName() const
{
  return "starfield";
}

QStringList GameStarfield::iniFiles() const
{
  return { "Starfield.ini", "StarfieldPrefs.ini", "StarfieldCustom.ini" };
}

QStringList GameStarfield::DLCPlugins() const
{
  return {};
}

QStringList GameStarfield::CCPlugins() const
{
  QStringList plugins = {};
  QFile file(gameDirectory().absoluteFilePath("Starfield.ccc"));
  if (file.open(QIODevice::ReadOnly)) {
    ON_BLOCK_EXIT([&file]() { file.close(); });

    if (file.size() == 0) {
      return plugins;
    }
    while (!file.atEnd()) {
      QByteArray line = file.readLine().trimmed();
      QString modName;
      if ((line.size() > 0) && (line.at(0) != '#')) {
        modName = QString::fromUtf8(line.constData()).toLower();
      }

      if (modName.size() > 0) {
        if (!plugins.contains(modName, Qt::CaseInsensitive)) {
          plugins.append(modName);
        }
      }
    }
  }
  return plugins;
}

IPluginGame::LoadOrderMechanism GameStarfield::loadOrderMechanism() const
{
  return IPluginGame::LoadOrderMechanism::PluginsTxt;
}

int GameStarfield::nexusModOrganizerID() const
{
  return 28715;
}

int GameStarfield::nexusGameID() const
{
  return 1151;
}
