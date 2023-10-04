#include "starfieldgameplugins.h"

using namespace MOBase;

StarfieldGamePlugins::StarfieldGamePlugins(MOBase::IOrganizer* organizer)
    : CreationGamePlugins(organizer)
{}

bool StarfieldGamePlugins::overridePluginsAreSupported()
{
  return true;
}

void StarfieldGamePlugins::writePluginList(const IPluginList* pluginList,
                                           const QString& filePath)
{
  if (m_Organizer->managedGame()->sortMechanism() !=
      MOBase::IPluginGame::SortMechanism::NONE) {
    CreationGamePlugins::writePluginList(pluginList, filePath);
  }
}

QStringList StarfieldGamePlugins::readPluginList(MOBase::IPluginList* pluginList)
{
  if (m_Organizer->managedGame()->sortMechanism() !=
      MOBase::IPluginGame::SortMechanism::NONE) {
    return CreationGamePlugins::readPluginList(pluginList);
  }
  return {};
}