m = Map("oled", translate("OLED"), translate("A LuCI app that helps you config your oled display (SSD1306, 0.91', 128X32) with screensavers! <br /> <br /> Any issues, please go to: ")..[[<a href="https://github.com/natelol/luci-app-oled" target="_blank">luci-app-oled</a>]])

--m.chain("luci")

m:section(SimpleSection).template="oled/status"

s = m:section(TypedSection, "oled", translate("OLED"))
s.anonymous=true
s.addremove=false

--OPTIONS
s:tab("info", translate("Info Display"))
s:tab("screensaver", translate("screensaver"))

o = s:taboption("info", Flag, "enable", translate("Enable"))
o.default=0
--informtion  options----
o = s:taboption("info", Flag, "date", translate("Date"), translate('Format YYYY-MM-DD_WEEK HH:MM:SS'))
o.default=0
o = s:taboption("info", Flag, "lanip", translate("IP"), translate("LAN IP address"))
o.default=0
o = s:taboption("info", Flag, "cputemp", translate("CPU temperature"))
o.default=0
o = s:taboption("info", Flag, "cpufreq", translate("CPU frequency"))
o.default=0
o = s:taboption("info", Flag, "netspeed", translate("Network speed"))
o.default=0
o = s:taboption("info", Value, "time", translate("Display time"))
o.default=0

--screensaver options--
o = s:taboption("screensaver", Flag, "drawline", translate("Draw Many Lines"))
o.default=0
o = s:taboption("screensaver", Flag, "drawrect", translate("Draw Rectangles"))
o.default=0
o = s:taboption("screensaver", Flag, "fillrect", translate("Draw Multiple Rectangles"))
o.default=0
o = s:taboption("screensaver", Flag, "drawcircle", translate("Draw Multiple Circles"))
o.default=0
o = s:taboption("screensaver", Flag, "drawroundrect", translate("Draw a white circle, 10 pixel radius"))
o.default=0
o = s:taboption("screensaver", Flag, "fillroundrect", translate("Fill the Round Rectangles"))
o.default=0
o = s:taboption("screensaver", Flag, "drawtriangle", translate("Draw Triangles"))
o.default=0
o = s:taboption("screensaver", Flag, "filltriangle", translate("Fill Triangles"))
o.default=0
o = s:taboption("screensaver", Flag, "displaybitmap", translate("Display miniature bitmap"))
o.default=0
o = s:taboption("screensaver", Flag, "displayinvertnormal", translate("Invert Display Normalize it"))
o.default=0
o = s:taboption("screensaver", Flag, "drawbitmap", translate("Draw a bitmap and animate movement"))
o.default=0
o = s:taboption("screensaver", Flag, "drawbitmapeg", translate("Draw a bitmap and animate"))
o.default=0

return m

















---
