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
  if (m_Organizer
          ->pluginSetting(m_Organizer->managedGame()->name(),
                          "enable_plugin_management")
          .toBool()) {
    CreationGamePlugins::writePluginList(pluginList, filePath);
  }
}