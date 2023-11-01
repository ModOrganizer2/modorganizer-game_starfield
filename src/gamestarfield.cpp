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

#include "scopeguard.h"
#include "utility.h"

using namespace MOBase;

GameStarfield::GameStarfield() {}

bool GameStarfield::init(IOrganizer* moInfo)
{
  if (!GameGamebryo::init(moInfo)) {
    return false;
  }

  registerFeature<ScriptExtender>(new StarfieldScriptExtender(this));
  registerFeature<DataArchives>(
      new StarfieldDataArchives(myGamesPath(), gameDirectory()));
  registerFeature<LocalSavegames>(
      new GamebryoLocalSavegames(myGamesPath(), "StarfieldCustom.ini"));
  registerFeature<ModDataChecker>(new StarfieldModDataChecker(this));
  registerFeature<ModDataContent>(new StarfieldModDataContent(this));
  registerFeature<SaveGameInfo>(new GamebryoSaveGameInfo(this));
  registerFeature<GamePlugins>(new StarfieldGamePlugins(moInfo));
  registerFeature<UnmanagedMods>(new StarfieldUnmangedMods(this));
  registerFeature<BSAInvalidation>(
      new StarfieldBSAInvalidation(feature<DataArchives>(), this));

  return true;
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
                           findInGameFolder(feature<ScriptExtender>()->loaderName()))
         << ExecutableInfo("Starfield", findInGameFolder(binaryName()))
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
  return VersionInfo(1, 0, 0, VersionInfo::RELEASE_CANDIDATE);
}

QList<PluginSetting> GameStarfield::settings() const
{
  return QList<PluginSetting>()
         << PluginSetting(
                "enable_esp_warning",
                tr("Show a warning when ESP plugins are enabled in the load order."),
                true)
         << PluginSetting(
                "enable_esl_warning",
                tr("Show a warning when light plugins are enabled in the load order."),
                true)
         << PluginSetting("enable_overlay_warning",
                          tr("Show a warning when overlay-flagged plugins ar enabled "
                             "in the load order."),
                          true);
}

MappingType GameStarfield::mappings() const
{
  MappingType result;
  if (testFilePlugins().isEmpty()) {
    for (const QString& profileFile : {"plugins.txt", "loadorder.txt"}) {
      result.push_back({m_Organizer->profilePath() + "/" + profileFile,
                        localAppFolder() + "/" + gameShortName() + "/" + profileFile,
                        false});
    }
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
  QStringList plugins = {"Starfield.esm", "Constellation.esm", "OldMars.esm"};

  auto testPlugins = testFilePlugins();
  if (loadOrderMechanism() == LoadOrderMechanism::None) {
    plugins << enabledPlugins();
    plugins << testPlugins;
  } else {
    plugins << CCPlugins();
  }

  return plugins;
}

QStringList GameStarfield::enabledPlugins() const
{
  return {"BlueprintShips-Starfield.esm"};
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
  QStringList plugins = {};
  QFile file(gameDirectory().absoluteFilePath("Starfield.ccc"));
  if (file.open(QIODevice::ReadOnly)) {
    ON_BLOCK_EXIT([&file]() {
      file.close();
    });

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

IPluginGame::SortMechanism GameStarfield::sortMechanism() const
{
  if (testFilePlugins().isEmpty())
    return IPluginGame::SortMechanism::LOOT;
  return IPluginGame::SortMechanism::NONE;
}

IPluginGame::LoadOrderMechanism GameStarfield::loadOrderMechanism() const
{
  if (testFilePlugins().isEmpty())
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
    if (m_Organizer->pluginSetting(name(), "enable_esl_warning").toBool() &&
        activeESL())
      result.push_back(PROBLEM_ESL);
    if (m_Organizer->pluginSetting(name(), "enable_overlay_warning").toBool() &&
        activeOverlay())
      result.push_back(PROBLEM_OVERLAY);
    if (testFilePresent())
      result.push_back(PROBLEM_TEST_FILE);
    else if (pluginsTxtEnabler())
      result.push_back(PROBLEM_PLUGINS_TXT);
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

bool GameStarfield::activeESL() const
{
  m_Active_ESLs.clear();
  std::set<QString> enabledPlugins;

  QStringList esps = m_Organizer->findFiles("", [](const QString& fileName) -> bool {
    return fileName.endsWith(".esp", FileNameComparator::CaseSensitivity) ||
           fileName.endsWith(".esm", FileNameComparator::CaseSensitivity) ||
           fileName.endsWith(".esl", FileNameComparator::CaseSensitivity);
  });

  for (const QString& esp : esps) {
    QString baseName = QFileInfo(esp).fileName();
    if (primaryPlugins().contains(baseName, Qt::CaseInsensitive))
      continue;
    if (m_Organizer->pluginList()->state(baseName) == IPluginList::STATE_ACTIVE &&
        !m_Organizer->pluginList()->hasNoRecords(baseName))
      if (m_Organizer->pluginList()->hasLightExtension(baseName) ||
          m_Organizer->pluginList()->isLightFlagged(baseName))
        m_Active_ESLs.insert(baseName);
  }

  if (!m_Active_ESLs.empty())
    return true;
  return false;
}

bool GameStarfield::activeOverlay() const
{
  m_Active_Overlays.clear();
  std::set<QString> enabledPlugins;

  QStringList esps = m_Organizer->findFiles("", [](const QString& fileName) -> bool {
    return fileName.endsWith(".esp", FileNameComparator::CaseSensitivity) ||
           fileName.endsWith(".esm", FileNameComparator::CaseSensitivity) ||
           fileName.endsWith(".esl", FileNameComparator::CaseSensitivity);
  });

  for (const QString& esp : esps) {
    QString baseName = QFileInfo(esp).fileName();
    if (primaryPlugins().contains(baseName, Qt::CaseInsensitive))
      continue;
    if (m_Organizer->pluginList()->state(baseName) == IPluginList::STATE_ACTIVE) {
      if (m_Organizer->pluginList()->isOverlayFlagged(baseName))
        m_Active_Overlays.insert(baseName);
    }
  }

  if (!m_Active_Overlays.empty())
    return true;
  return false;
}

bool GameStarfield::testFilePresent() const
{
  if (!testFilePlugins().isEmpty())
    return true;
  return false;
}

bool GameStarfield::pluginsTxtEnabler() const
{
  if (sortMechanism() != SortMechanism::NONE) {
    auto files = m_Organizer->findFiles("sfse\\plugins", {"sfpluginstxtenabler.dll"});
    if (files.isEmpty())
      return true;
  }
  return false;
}

QString GameStarfield::shortDescription(unsigned int key) const
{
  switch (key) {
  case PROBLEM_ESP:
    return tr("You have active ESP plugins in Starfield");
  case PROBLEM_ESL:
    return tr("You have active ESL plugins in Starfield");
  case PROBLEM_OVERLAY:
    return tr("You have active overlay plugins");
  case PROBLEM_TEST_FILE:
    return tr("sTestFile entries are present");
  case PROBLEM_PLUGINS_TXT:
    return tr("Plugins.txt Enabler missing");
  }
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
  case PROBLEM_ESL: {
    QString eslInfo = SetJoin(m_Active_ESLs, ", ");
    return tr("<p>Light plugins work differently in Starfield. They use a different "
              "base form ID compared with standard plugin files.</p>"
              "<p>What this means is that you can't just change a standard plugin to a "
              "light plugin at will, it can and will break any dependent plugin. If "
              "you do so, be absolutely certain no other plugins use that plugin as a "
              "master.</p>"
              "<p>Notably, xEdit does not currently support saving or loading ESL "
              "files under these conditions.<p>"
              "<h4>Current ESLs:</h4><p>%1</p>")
        .arg(eslInfo);
  }
  case PROBLEM_OVERLAY: {
    QString overlayInfo = SetJoin(m_Active_Overlays, ", ");
    return tr("<p>Overlay-flagged plugins are not currently recommended. In theory, "
              "they should allow you to update existing records without utilizing "
              "additional load order slots. Unfortunately, it appears that the game "
              "still allocates the slots as if these were standard plugins. Therefore, "
              "at the moment there is no real use for this plugin flag.</p>"
              "<p>Notably, xEdit does not currently support saving or loading "
              "overlay-flagged files under these conditions.</p>"
              "<h4>Current Overlays:</h4><p>%1</p>")
        .arg(overlayInfo);
  }
  case PROBLEM_TEST_FILE: {
    return tr("<p>You have plugin managment enabled but you still have sTestFile "
              "settings in your StarfieldCustom.ini. These must be removed or the game "
              "will not read the plugins.txt file. Management is still disabled.</p>");
  }
  case PROBLEM_PLUGINS_TXT: {
    return tr("<p>You have plugin management turned on but do not have the Plugins.txt "
              "Enabler SFSE plugin installed. Plugin file management for Starfield "
              "will not work without this SFSE plugin.</p>");
  }
  }
}

bool GameStarfield::hasGuidedFix(unsigned int key) const
{
  if (key == PROBLEM_PLUGINS_TXT)
    return true;
  return false;
}

void GameStarfield::startGuidedFix(unsigned int key) const
{
  if (key == PROBLEM_PLUGINS_TXT) {
    QDesktopServices::openUrl(
        QUrl("https://www.nexusmods.com/starfield/mods/4157?tab=files"));
  }
}
