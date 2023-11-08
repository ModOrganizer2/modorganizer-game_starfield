<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE TS>
<TS version="2.1" language="en_US">
<context>
    <name>GameStarfield</name>
    <message>
        <location filename="gamestarfield.cpp" line="113"/>
        <source>Starfield Support Plugin</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="gamestarfield.cpp" line="123"/>
        <source>Adds support for the game Starfield.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="gamestarfield.cpp" line="136"/>
        <source>Show a warning when ESP plugins are enabled in the load order.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="gamestarfield.cpp" line="140"/>
        <source>Show a warning when light plugins are enabled in the load order.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="gamestarfield.cpp" line="143"/>
        <source>Show a warning when overlay-flagged plugins ar enabled in the load order.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="gamestarfield.cpp" line="147"/>
        <source>Show a warning when plugins.txt management is invalid.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="gamestarfield.cpp" line="150"/>
        <source>Bypass check for Plugins.txt Enabler. This may be useful if you use the ASI loader.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="gamestarfield.cpp" line="154"/>
        <source>As of this release LOOT Starfield support is minimal to nonexistant. Toggle this to enable it anyway.</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="gamestarfield.cpp" line="451"/>
        <source>You have active ESP plugins in Starfield</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="gamestarfield.cpp" line="453"/>
        <source>You have active ESL plugins in Starfield</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="gamestarfield.cpp" line="455"/>
        <source>You have active overlay plugins</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="gamestarfield.cpp" line="457"/>
        <source>sTestFile entries are present</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="gamestarfield.cpp" line="459"/>
        <source>Plugins.txt Enabler missing</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="gamestarfield.cpp" line="468"/>
        <source>&lt;p&gt;ESP plugins are not ideal for Starfield. In addition to being unable to sort them alongside ESM or master-flagged plugins, certain record references are always kept loaded by the game. This consumes unnecessary resources and limits the game&apos;s ability to load what it needs.&lt;/p&gt;&lt;p&gt;Ideally, plugins should be saved as ESM files upon release. It can also be released as an ESL plugin, however there are additional concerns with the way light plugins are currently handled and should only be used when absolutely certain about what you&apos;re doing.&lt;/p&gt;&lt;p&gt;Notably, xEdit does not currently support saving ESP files.&lt;/p&gt;&lt;h4&gt;Current ESPs:&lt;/h4&gt;&lt;p&gt;%1&lt;/p&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="gamestarfield.cpp" line="483"/>
        <source>&lt;p&gt;Light plugins work differently in Starfield. They use a different base form ID compared with standard plugin files.&lt;/p&gt;&lt;p&gt;What this means is that you can&apos;t just change a standard plugin to a light plugin at will, it can and will break any dependent plugin. If you do so, be absolutely certain no other plugins use that plugin as a master.&lt;/p&gt;&lt;p&gt;Notably, xEdit does not currently support saving or loading ESL files under these conditions.&lt;p&gt;&lt;h4&gt;Current ESLs:&lt;/h4&gt;&lt;p&gt;%1&lt;/p&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="gamestarfield.cpp" line="496"/>
        <source>&lt;p&gt;Overlay-flagged plugins are not currently recommended. In theory, they should allow you to update existing records without utilizing additional load order slots. Unfortunately, it appears that the game still allocates the slots as if these were standard plugins. Therefore, at the moment there is no real use for this plugin flag.&lt;/p&gt;&lt;p&gt;Notably, xEdit does not currently support saving or loading overlay-flagged files under these conditions.&lt;/p&gt;&lt;h4&gt;Current Overlays:&lt;/h4&gt;&lt;p&gt;%1&lt;/p&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="gamestarfield.cpp" line="507"/>
        <source>&lt;p&gt;You have plugin managment enabled but you still have sTestFile settings in your StarfieldCustom.ini. These must be removed or the game will not read the plugins.txt file. Management is still disabled.&lt;/p&gt;</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="gamestarfield.cpp" line="512"/>
        <source>&lt;p&gt;You have plugin management turned on but do not have the Plugins.txt Enabler SFSE plugin installed. Plugin file management for Starfield will not work without this SFSE plugin.&lt;/p&gt;</source>
        <translation type="unfinished"></translation>
    </message>
</context>
<context>
    <name>StarfieldModDataContent</name>
    <message>
        <location filename="starfieldmoddatacontent.h" line="29"/>
        <source>Materials</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="starfieldmoddatacontent.h" line="30"/>
        <source>Geometries</source>
        <translation type="unfinished"></translation>
    </message>
    <message>
        <location filename="starfieldmoddatacontent.h" line="31"/>
        <source>Video</source>
        <translation type="unfinished"></translation>
    </message>
</context>
</TS>
