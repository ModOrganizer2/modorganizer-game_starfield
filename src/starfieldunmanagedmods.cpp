#include "starfieldunmanagedmods.h"


StarfieldUnmangedMods::StarfieldUnmangedMods(const GameGamebryo *game)
  : GamebryoUnmangedMods(game)
{}

StarfieldUnmangedMods::~StarfieldUnmangedMods()
{}

QStringList StarfieldUnmangedMods::mods(bool onlyOfficial) const {
  QStringList result;

  QStringList pluginList = game()->primaryPlugins();
  QStringList otherPlugins = game()->DLCPlugins();
  otherPlugins.append(game()->CCPlugins());
  for (QString plugin : otherPlugins) {
    pluginList.removeAll(plugin);
  }
  QDir dataDir(game()->dataDirectory());
  for (const QString &fileName : dataDir.entryList({ "*.esp", "*.esl", "*.esm" })) {
    if (!pluginList.contains(fileName, Qt::CaseInsensitive)) {
      if (!onlyOfficial || pluginList.contains(fileName, Qt::CaseInsensitive)) {
        result.append(fileName.chopped(4)); // trims the extension off
      }
    }
  }

  return result;
}

QStringList StarfieldUnmangedMods::secondaryFiles(const QString &modName) const {
  // file extension in FO4 is .ba2 instead of bsa
  QStringList archives;
  QDir dataDir = game()->dataDirectory();
  for (const QString &archiveName : dataDir.entryList({modName + "*.ba2"})) {
    archives.append(dataDir.absoluteFilePath(archiveName));
  }
  return archives;
}

QString StarfieldUnmangedMods::displayName(const QString &modName) const
{
  // unlike in earlier games, in fallout 4 the file name doesn't correspond to
  // the public name
  return modName;
}
