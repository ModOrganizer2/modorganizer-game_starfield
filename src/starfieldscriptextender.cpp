#include "starfieldscriptextender.h"

#include <QString>
#include <QStringList>

StarfieldScriptExtender::StarfieldScriptExtender(GameGamebryo const *game) :
  GamebryoScriptExtender(game)
{
}

QString StarfieldScriptExtender::BinaryName() const
{
  return "f4se_loader.exe";
}

QString StarfieldScriptExtender::PluginPath() const
{
  return "f4se/plugins";
}
