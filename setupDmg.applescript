on run argv
	tell application "Finder"
		tell disk (item 1 of argv)
			open
			set current view of container window to icon view
			set toolbar visible of container window to false
			set statusbar visible of container window to false
			set the bounds of container window to {400, 100, 1000, 530}
			set theViewOptions to the icon view options of container window
			set arrangement of theViewOptions to not arranged
			set icon size of theViewOptions to 72
			set background picture of theViewOptions to file ".background:background.png"
			set position of item "Applications" of container window to {450,120}
			set position of item "Layered_WallPaper" of container window to {130,120}
			set position of item "Toggle_Autorun.command" of container window to {260,330}
			update without registering applications
			delay 5
			close
		end tell
	end tell
end run
