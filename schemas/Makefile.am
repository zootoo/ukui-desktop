desktop_gschemas_in = \
	org.ukui.accessibility-keyboard.gschema.xml.in \
	org.ukui.accessibility-startup.gschema.xml.in \
	org.ukui.applications-at-mobility.gschema.xml.in \
	org.ukui.applications-at-visual.gschema.xml.in \
	org.ukui.applications-browser.gschema.xml.in \
	org.ukui.applications-office.gschema.xml.in \
	org.ukui.applications-terminal.gschema.xml.in \
	org.ukui.background.gschema.xml.in \
	org.ukui.debug.gschema.xml.in \
	org.ukui.file-views.gschema.xml.in \
	org.ukui.interface.gschema.xml.in \
	org.ukui.lockdown.gschema.xml.in \
	org.ukui.peripherals-keyboard.gschema.xml.in \
	org.ukui.peripherals-mouse.gschema.xml.in \
	org.ukui.sound.gschema.xml.in \
	org.ukui.thumbnail-cache.gschema.xml.in \
	org.ukui.thumbnailers.gschema.xml.in \
	org.ukui.typing-break.gschema.xml.in

@INTLTOOL_XML_NOMERGE_RULE@

gsettings_SCHEMAS = $(desktop_gschemas_in:.xml.in=.xml)

@GSETTINGS_RULES@

EXTRA_DIST = \
	$(desktop_gschemas_in)

CLEANFILES = \
	$(gsettings_SCHEMAS) \
	*.gschema.valid
