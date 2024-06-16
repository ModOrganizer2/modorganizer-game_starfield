#include "starfieldunmanagedmods.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

StarfieldUnmangedMods::StarfieldUnmangedMods(const GameStarfield* game,
                                             const QString appDataFolder)
    : GamebryoUnmangedMods(game), m_AppDataFolder(appDataFolder)
{}

StarfieldUnmangedMods::~StarfieldUnmangedMods() {}

QStringList StarfieldUnmangedMods::mods(bool onlyOfficial) const
{
  QStringList result;

  QStringList pluginList   = game()->primaryPlugins() + game()->enabledPlugins();
  QStringList otherPlugins = game()->DLCPlugins();
  otherPlugins.append(game()->CCPlugins());
  for (QString plugin : otherPlugins) {
    pluginList.removeAll(plugin);
  }
  QMap<QString, QDir> directories = {{"data", game()->dataDirectory()}};
  directories.insert(game()->secondaryDataDirectories());
  for (QDir directory : directories) {
    for (const QString& fileName : directory.entryList({"*.esp", "*.esl", "*.esm"})) {
      if (!pluginList.contains(fileName, Qt::CaseInsensitive)) {
        if (!onlyOfficial || pluginList.contains(fileName, Qt::CaseInsensitive)) {
          result.append(fileName.chopped(4));  // trims the extension off
        }
      }
    }
  }

  return result;
}

QFileInfo StarfieldUnmangedMods::referenceFile(const QString& modName) const
{
  QFileInfoList files;
  QMap<QString, QDir> directories = {{"data", game()->dataDirectory()}};
  directories.insert(game()->secondaryDataDirectories());
  for (QDir directory : directories) {
    files += directory.entryInfoList(QStringList() << modName + ".es*");
  }
  if (files.size() > 0) {
    return files.at(0);
  } else {
    return QFileInfo();
  }
}

QStringList StarfieldUnmangedMods::secondaryFiles(const QString& modName) const
{
  QFile content(m_AppDataFolder + "/" + game()->gameShortName() +
                "/ContentCatalog.txt");
  QStringList files;
  if (content.exists()) {
    if (content.open(QIODevice::OpenModeFlag::ReadOnly)) {
      auto contentData         = content.readAll();
      QJsonDocument contentDoc = QJsonDocument::fromJson(contentData);
      QJsonObject contentObj   = contentDoc.object();
      for (auto mod : contentObj.keys()) {
        if (mod == "ContentCatalog")
          continue;
        auto modData  = contentObj.value(mod).toObject();
        auto modFiles = modData.value("Files").toArray();
        bool found    = false;
        for (auto file : modFiles) {
          if (file.toString().startsWith(modName, Qt::CaseInsensitive)) {
            found = true;
          }
          if (found)
            break;
        }
        if (found) {
          for (auto file : modFiles) {
            if (!file.toString().endsWith(".esm") &&
                !file.toString().endsWith(".esl") && !file.toString().endsWith(".esp"))
              files.append(file.toString());
          }
          break;
        }
      }
    }
    content.close();
  }
  // file extension in FO4 is .ba2 instead of bsa
  QMap<QString, QDir> directories = {{"data", game()->dataDirectory()}};
  directories.insert(game()->secondaryDataDirectories());
  for (QDir directory : directories) {
    for (const QString& archiveName : directory.entryList({modName + "*.ba2"})) {
      files.append(directory.absoluteFilePath(archiveName));
    }
  }
  return files;
}

QString StarfieldUnmangedMods::displayName(const QString& modName) const
{
  QFile content(m_AppDataFolder + "/" + game()->gameShortName() +
                "/ContentCatalog.txt");
  QString name = modName;
  if (content.exists()) {
    if (content.open(QIODevice::OpenModeFlag::ReadOnly)) {
      auto contentData         = content.readAll();
      QJsonDocument contentDoc = QJsonDocument::fromJson(contentData);
      QJsonObject contentObj   = contentDoc.object();
      for (auto mod : contentObj.keys()) {
        if (mod == "ContentCatalog")
          continue;
        auto modData  = contentObj.value(mod).toObject();
        auto modFiles = modData.value("Files").toArray();
        bool found    = false;
        for (auto file : modFiles) {
          if (file.toString().startsWith(modName, Qt::CaseInsensitive)) {
            found = true;
          }
          if (found)
            break;
        }
        if (found) {
          name = modData.value("Title").toString();
          break;
        }
      }
    }
    content.close();
  }
  // unlike in earlier games, in fallout 4 the file name doesn't correspond to
  // the public name
  return name;
}
