#include "gamestarfield.h"

#include "ipluginlist.h"

#include "starfieldbsainvalidation.h"
#include "starfielddataarchives.h"
#include "starfieldgameplugins.h"
#include "starfieldmoddatachecker.h"
#include "starfieldmoddatacontent.h"
#include "starfieldsavegame.h"
#include "starfieldscriptextender.h"
#include "starfieldunmanagedmods.h"

#include "versioninfo.h"
#include <executableinfo.h>
#include <gamebryolocalsavegames.h>
#include <gamebryosavegameinfo.h>
#include <pluginsetting.h>

#include <QCoreApplication>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QList>
#include <QObject>
#include <QString>
#include <QStringList>

#include <memory>

#include "utility.h"

using namespace MOBase;

GameStarfield::GameStarfield() {}

bool GameStarfield::init(IOrganizer* moInfo)
{
  if (!GameGamebryo::init(moInfo)) {
    return false;
  }

  auto dataArchives = std::make_shared<StarfieldDataArchives>(this);
  registerFeature(std::make_shared<StarfieldScriptExtender>(this));
  registerFeature(dataArchives);
  registerFeature(
      std::make_shared<GamebryoLocalSavegames>(this, "StarfieldCustom.ini"));
  registerFeature(std::make_shared<StarfieldModDataChecker>(this));
  registerFeature(
      std::make_shared<StarfieldModDataContent>(m_Organizer->gameFeatures()));
  registerFeature(std::make_shared<GamebryoSaveGameInfo>(this));
  registerFeature(std::make_shared<StarfieldGamePlugins>(moInfo));
  registerFeature(std::make_shared<StarfieldUnmanagedMods>(this, localAppFolder()));
  registerFeature(std::make_shared<StarfieldBSAInvalidation>(dataArchives.get(), this));

  m_Organizer->pluginList()->onRefreshed([&]() {
    setCCCFile();
  });

  return true;
}

/*
 * This is used to write the primary plugins to a profile-based Starfield.ccc file. We
 * map this into the game directory with the VFS. The game does not currently ship with
 * this file but does still read it like SkyrimSE and Fallout 4. We can make use of it
 * to correct the current behavior where core plugins are loaded after parsing
 * plugins.txt leading to ambiguous load orders.
 */
void GameStarfield::setCCCFile() const
{
  if (m_Organizer->profilePath().isEmpty())
    return;
  if (m_Organizer->pluginSetting(name(), "enable_loadorder_fix").toBool()) {
    QFile cccFile(m_Organizer->profilePath() + "/Starfield.ccc");
    if (cccFile.open(QIODevice::WriteOnly)) {
      auto plugins = primaryPlugins();
      for (auto plugin : plugins) {
        cccFile.write(plugin.toUtf8());
        cccFile.write("\n");
      }
    }
  }
}

QString GameStarfield::gameName() const
{
  return "Starfield";
}

void GameStarfield::detectGame()
{
  m_GamePath    = identifyGamePath();
  m_MyGamesPath = determineMyGamesPath("Starfield");
}

QString GameStarfield::identifyGamePath() const
{
  return parseSteamLocation(steamAPPId(), gameName());
}

QDir GameStarfield::dataDirectory() const
{
  QDir dataDir = documentsDirectory().absoluteFilePath("Data");
  if (!dataDir.exists())
    dataDir.mkdir(dataDir.path());
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
         << ExecutableInfo("SFSE",
                           findInGameFolder(m_Organizer->gameFeatures()
                                                ->gameFeature<MOBase::ScriptExtender>()
                                                ->loaderName()))
         << ExecutableInfo("Starfield", findInGameFolder(binaryName()))
         << ExecutableInfo("Creation Kit", findInGameFolder("CreationKit.exe"))
                .withSteamAppId("2722710")
         << ExecutableInfo("LOOT", QFileInfo(getLootPath()))
                .withArgument("--game=\"Starfield\"");
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
  return VersionInfo(1, 1, 0, VersionInfo::RELEASE_FINAL);
}

QList<PluginSetting> GameStarfield::settings() const
{
  return QList<PluginSetting>()
         << PluginSetting(
                "enable_esp_warning",
                tr("Show a warning when ESP plugins are enabled in the load order."),
                true)
         << PluginSetting("enable_management_warnings",
                          tr("Show a warning when plugins.txt management is invalid."),
                          true)
         << PluginSetting("enable_loot_sorting",
                          tr("As of this release LOOT Starfield support is minimal to "
                             "nonexistant. Toggle this to enable it anyway."),
                          false)
         << PluginSetting("enable_loadorder_fix",
                          tr("Utilize Starfield.ccc to affix core plugin load order "
                             "(will override existing file)."),
                          true);
}

MappingType GameStarfield::mappings() const
{
  if (m_Organizer->pluginSetting(name(), "enable_loadorder_fix").toBool()) {
    setCCCFile();
  }
  MappingType result;
  if (testFilePlugins().isEmpty()) {
    for (const QString& profileFile : {"plugins.txt", "loadorder.txt"}) {
      result.push_back({m_Organizer->profilePath() + "/" + profileFile,
                        localAppFolder() + "/" + gameShortName() + "/" + profileFile,
                        false});
    }
  }
  if (m_Organizer->pluginSetting(name(), "enable_loadorder_fix").toBool()) {
    result.push_back({m_Organizer->profilePath() + "/" + "Starfield.ccc",
                      gameDirectory().absolutePath() + "/" + "Starfield.ccc", false});
  }
  return result;
}

void GameStarfield::initializeProfile(const QDir& path, ProfileSettings settings) const
{
  if (settings.testFlag(IPluginGame::MODS)) {
    copyToProfile(localAppFolder() + "/Starfield", path, "plugins.txt");
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

std::shared_ptr<const GamebryoSaveGame>
GameStarfield::makeSaveGame(QString filePath) const
{
  return std::make_shared<const StarfieldSaveGame>(filePath, this);
}

QString GameStarfield::steamAPPId() const
{
  return "1716740";
}

QStringList GameStarfield::testFilePlugins() const
{
  QStringList plugins;
  if (m_Organizer != nullptr && m_Organizer->profile() != nullptr) {
    QString customIni(
        m_Organizer->profile()->absoluteIniFilePath("StarfieldCustom.ini"));
    if (QFile(customIni).exists()) {
      for (int i = 1; i <= 10; ++i) {
        QString setting("sTestFile");
        setting += std::to_string(i);
        WCHAR value[MAX_PATH];
        DWORD length = ::GetPrivateProfileStringW(
            L"General", setting.toStdWString().c_str(), L"", value, MAX_PATH,
            customIni.toStdWString().c_str());
        if (length && wcscmp(value, L"") != 0) {
          QString plugin = QString::fromWCharArray(value, length);
          if (!plugin.isEmpty() && !plugins.contains(plugin))
            plugins.append(plugin);
        }
      }
    }
  }
  return plugins;
}

QStringList GameStarfield::primaryPlugins() const
{
  QStringList plugins = {"Starfield.esm", "Constellation.esm",
                         "OldMars.esm",   "BlueprintShips-Starfield.esm",
                         "SFBGS007.esm",  "SFBGS008.esm",
                         "SFBGS006.esm",  "SFBGS003.esm"};

  auto testPlugins = testFilePlugins();
  if (loadOrderMechanism() == LoadOrderMechanism::None) {
    plugins << enabledPlugins();
    plugins << testPlugins;
  }

  return plugins;
}

QStringList GameStarfield::enabledPlugins() const
{
  return {};
}

QStringList GameStarfield::gameVariants() const
{
  return {"Regular"};
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
  return {"StarfieldPrefs.ini", "StarfieldCustom.ini"};
}

bool GameStarfield::prepareIni(const QString& exec)
{
  return true;  // no need to write to Starfield.ini
}

QStringList GameStarfield::DLCPlugins() const
{
  return {};
}

QStringList GameStarfield::CCPlugins() const
{
  // While the CCC file appears to be mostly legacy, we need to parse it since the game
  // will still read it and there are some compatibility reason to use it for
  // force-loading the core game plugins.
  QStringList plugins     = {};
  QStringList corePlugins = primaryPlugins() + DLCPlugins();
  if (!testFilePresent()) {
    QFile file(gameDirectory().absoluteFilePath("Starfield.ccc"));
    if (m_Organizer->pluginSetting(name(), "enable_loadorder_fix").toBool() &&
        !m_Organizer->profilePath().isEmpty()) {
      file.setFileName(m_Organizer->profilePath() + "/Starfield.ccc");
    }
    if (file.open(QIODevice::ReadOnly)) {
      if (file.size() > 0) {
        while (!file.atEnd()) {
          QByteArray line = file.readLine().trimmed();
          QString modName;
          if ((line.size() > 0) && (line.at(0) != '#')) {
            modName = QString::fromUtf8(line.constData()).toLower();
          }

          if (modName.size() > 0) {
            if (!plugins.contains(modName, Qt::CaseInsensitive) &&
                !corePlugins.contains(modName, Qt::CaseInsensitive)) {
              plugins.append(modName);
            }
          }
        }
      }
    }
  }

  std::shared_ptr<StarfieldUnmanagedMods> unmanagedMods =
      std::static_pointer_cast<StarfieldUnmanagedMods>(
          m_Organizer->gameFeatures()->gameFeature<MOBase::UnmanagedMods>());

  // The ContentCatalog.txt appears to be the main repository where Starfiled stores
  // info about the installed Creations. We parse this to correctly mark unmanaged mods
  // as Creations. The StarfieldUnmanagedMods class handles parsing mod names and files.
  if (unmanagedMods.get()) {
    auto contentCatalog = unmanagedMods->parseContentCatalog();
    for (const auto& mod : contentCatalog) {
      if (!plugins.contains(mod.first, Qt::CaseInsensitive)) {
        plugins.append(mod.first);
      }
    }
  }
  return plugins;
}

IPluginGame::SortMechanism GameStarfield::sortMechanism() const
{
  if (!testFilePresent() &&
      m_Organizer->pluginSetting(name(), "enable_loot_sorting").toBool())
    return IPluginGame::SortMechanism::LOOT;
  return IPluginGame::SortMechanism::NONE;
}

IPluginGame::LoadOrderMechanism GameStarfield::loadOrderMechanism() const
{
  if (!testFilePresent())
    return IPluginGame::LoadOrderMechanism::PluginsTxt;
  return IPluginGame::LoadOrderMechanism::None;
}

int GameStarfield::nexusModOrganizerID() const
{
  return 0;
}

int GameStarfield::nexusGameID() const
{
  return 4187;
}

// Start Diagnose
std::vector<unsigned int> GameStarfield::activeProblems() const
{
  std::vector<unsigned int> result;
  if (m_Organizer->managedGame() == this) {
    if (m_Organizer->pluginSetting(name(), "enable_esp_warning").toBool() &&
        activeESP())
      result.push_back(PROBLEM_ESP);
    if (m_Organizer->pluginSetting(name(), "enable_management_warnings").toBool()) {
      if (testFilePresent())
        result.push_back(PROBLEM_TEST_FILE);
    }
  }
  return result;
}

bool GameStarfield::activeESP() const
{
  m_Active_ESPs.clear();
  std::set<QString> enabledPlugins;

  QStringList esps = m_Organizer->findFiles("", [](const QString& fileName) -> bool {
    return fileName.endsWith(".esp", FileNameComparator::CaseSensitivity);
  });

  for (const QString& esp : esps) {
    QString baseName = QFileInfo(esp).fileName();
    if (m_Organizer->pluginList()->state(baseName) == IPluginList::STATE_ACTIVE) {
      m_Active_ESPs.insert(baseName);
    }
  }

  if (!m_Active_ESPs.empty())
    return true;
  return false;
}

bool GameStarfield::testFilePresent() const
{
  if (!testFilePlugins().isEmpty())
    return true;
  return false;
}

QString GameStarfield::shortDescription(unsigned int key) const
{
  switch (key) {
  case PROBLEM_ESP:
    return tr("You have active ESP plugins in Starfield");
  case PROBLEM_TEST_FILE:
    return tr("sTestFile entries are present");
  }
  return "";
}

QString GameStarfield::fullDescription(unsigned int key) const
{
  switch (key) {
  case PROBLEM_ESP: {
    QString espInfo = SetJoin(m_Active_ESPs, ", ");
    return tr("<p>ESP plugins are not ideal for Starfield. In addition to being unable "
              "to sort them alongside ESM or master-flagged plugins, certain record "
              "references are always kept loaded by the game. This consumes "
              "unnecessary resources and limits the game's ability to load what it "
              "needs.</p>"
              "<p>Ideally, plugins should be saved as ESM files upon release. It can "
              "also be released as an ESL plugin, however there are additional "
              "concerns with the way light plugins are currently handled and should "
              "only be used when absolutely certain about what you're doing.</p>"
              "<p>Notably, xEdit does not currently support saving ESP files.</p>"
              "<h4>Current ESPs:</h4><p>%1</p>")
        .arg(espInfo);
  }
  case PROBLEM_TEST_FILE: {
    return tr("<p>You have plugin managment enabled but you still have sTestFile "
              "settings in your StarfieldCustom.ini. These must be removed or the game "
              "will not read the plugins.txt file. Management is still disabled.</p>");
  }
  }
  return "";
}

bool GameStarfield::hasGuidedFix(unsigned int key) const
{
  return false;
}

void GameStarfield::startGuidedFix(unsigned int key) const {}
