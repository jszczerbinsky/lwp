clear

if [[ ! -a ~/Library/LaunchAgents/lwp.plist ]]; then
	mkdir -p ~/Library/LaunchAgents
	cp /Applications/Layered_WallPaper/lwp.template.plist ~/Library/LaunchAgents/lwp.plist
	sed -i "" "s/@user@/$(whoami)/g" ~/Library/LaunchAgents/lwp.plist
	sed -i "" "s^@binpath@^/Applications/Layered_WallPaper/lwp.app/Contents/MacOS/lwp^g" ~/Library/LaunchAgents/lwp.plist
	launchctl load ~/Library/LaunchAgents/lwp.plist

	echo Layered WallPaper has been added to autostart
else
	launchctl unload ~/Library/LaunchAgents/lwp.plist
	rm ~/Library/LaunchAgents/lwp.plist

	echo Layered WallPaper has been removed from autostart
fi
